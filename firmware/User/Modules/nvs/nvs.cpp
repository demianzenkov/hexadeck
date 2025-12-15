#include "nvs.h"
#include "stm32f4xx_hal.h"
#include "string.h"

// STM32F446 512KB device: Sectors 0-7
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) // 16 KB
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) // 16 KB
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) // 16 KB
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) // 16 KB
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) // 64 KB
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) // 128 KB
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) // 128 KB
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) // 128 KB

// End of user flash area (last address in sector 7)
#define FLASH_END_ADDRESS      (ADDR_FLASH_SECTOR_7 + getSectorSize(ADDR_FLASH_SECTOR_7))

#define MIDI_PRESETS_COUNT        4
#define MIDI_PRESET_SIZE         (sizeof(module_state_t) * 16) // 16 presets per bank
#define MIDI_PRESETS_TOTAL_SIZE  (MIDI_PRESET_SIZE * MIDI_PRESETS_COUNT)
// Store all banks in the last sector (sector 7)
#define MIDI_PRESETS_BASE_ADDR    (ADDR_FLASH_SECTOR_7)



void NVS::read(nvs_key_e sector, void * out_data, size_t data_size)
{
	if (sector != NVS_SECTOR_MIDI_PRESETS) return;
	// Only MIDI presets supported for now
	// Read from flash directly
	uint8_t *dst = (uint8_t *)out_data;
	uint32_t src_addr = getPresetAddress(0); // Only bank 0 for generic read
	for (size_t i = 0; i < data_size; ++i) {
		dst[i] = *(volatile uint8_t *)(src_addr + i);
	}
}

void NVS::write(nvs_key_e sector, void * in_data, size_t data_size)
{
	if (sector != NVS_SECTOR_MIDI_PRESETS) return;
	// Only MIDI presets supported for now
	// Write to bank 0 for generic write
	uint32_t address = getPresetAddress(0);
	uint32_t sector_num = getSectorNumber(address);
	if (sector_num == 0xFFFFFFFFU) return;

	HAL_FLASH_Unlock();

	// Erase sector before writing
	FLASH_EraseInitTypeDef eraseInitStruct;
	uint32_t sectorError = 0;
	eraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	eraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	eraseInitStruct.Sector = sector_num;
	eraseInitStruct.NbSectors = 1;
	if (HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError) != HAL_OK) {
		HAL_FLASH_Lock();
		return;
	}

	// Program data word by word (32-bit)
	uint8_t *src = (uint8_t *)in_data;
	for (size_t i = 0; i < data_size; i += 4) {
		uint32_t word = 0xFFFFFFFFU;
		size_t remain = data_size - i;
		if (remain >= 4) {
			word = *(uint32_t *)(src + i);
		} else {
			// Handle last partial word
			for (size_t j = 0; j < remain; ++j) {
				((uint8_t *)&word)[j] = src[i + j];
			}
		}
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address + i, word) != HAL_OK) {
			break;
		}
	}
	HAL_FLASH_Lock();
}



int NVS::saveModulePreset(uint8_t preset_bank, module_state_t *preset)
{
	if (preset_bank >= MIDI_PRESETS_COUNT) {
		return -1;
	}
	// Allocate RAM buffer for all banks
	uint8_t sector_buffer[MIDI_PRESETS_TOTAL_SIZE];
	// Read current sector contents
	const uint8_t *flash_ptr = (const uint8_t *)MIDI_PRESETS_BASE_ADDR;
	for (size_t i = 0; i < MIDI_PRESETS_TOTAL_SIZE; ++i) {
		sector_buffer[i] = flash_ptr[i];
	}
	// Update the relevant bank in RAM
	memcpy(&sector_buffer[preset_bank * MIDI_PRESET_SIZE], preset, MIDI_PRESET_SIZE);

	// Erase the sector
	uint32_t sector_num = getSectorNumber(MIDI_PRESETS_BASE_ADDR);
	if (sector_num == 0xFFFFFFFFU) return -2;

	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef eraseInitStruct;
	uint32_t sectorError = 0;
	eraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	eraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;
	eraseInitStruct.Sector = sector_num;
	eraseInitStruct.NbSectors = 1;
	if (HAL_FLASHEx_Erase(&eraseInitStruct, &sectorError) != HAL_OK) {
		HAL_FLASH_Lock();
		return -3;
	}

	// Write back the whole sector from RAM
	for (size_t i = 0; i < MIDI_PRESETS_TOTAL_SIZE; i += 4) {
		uint32_t word = 0xFFFFFFFFU;
		size_t remain = MIDI_PRESETS_TOTAL_SIZE - i;
		if (remain >= 4) {
			word = *(uint32_t *)(&sector_buffer[i]);
		} else {
			for (size_t j = 0; j < remain; ++j) {
				((uint8_t *)&word)[j] = sector_buffer[i + j];
			}
		}
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, MIDI_PRESETS_BASE_ADDR + i, word) != HAL_OK) {
			HAL_FLASH_Lock();
			return -4;
		}
	}
	HAL_FLASH_Lock();
	return 0;
}


int NVS::loadModulePreset(uint8_t preset_bank, module_state_t *preset)
{
	if (preset_bank >= MIDI_PRESETS_COUNT) {
		return -1;
	}
	uint32_t address = getPresetAddress(preset_bank);
	// Check if the bank is erased (all 0xFF)
	bool erased = true;
	for (size_t i = 0; i < MIDI_PRESET_SIZE; ++i) {
		if (*(volatile uint8_t *)(address + i) != 0xFF) {
			erased = false;
			break;
		}
	}
	if (erased) return -2;
	// Read only the relevant bank
	uint8_t *dst = (uint8_t *)preset;
	for (size_t i = 0; i < MIDI_PRESET_SIZE; ++i) {
		dst[i] = *(volatile uint8_t *)(address + i);
	}
	return 0;
}


// Get sector number for a given address (ST reference)
uint32_t NVS::getSector(uint32_t Address)
{
	uint32_t sector = 0;

	if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
	{
		sector = FLASH_SECTOR_0;
	}
	else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
	{
		sector = FLASH_SECTOR_1;
	}
	else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
	{
		sector = FLASH_SECTOR_2;
	}
	else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
	{
		sector = FLASH_SECTOR_3;
	}
	else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
	{
		sector = FLASH_SECTOR_4;
	}
	else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
	{
		sector = FLASH_SECTOR_5;
	}
	else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
	{
		sector = FLASH_SECTOR_6;
	}
	else /* (Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_7) */
	{
		sector = FLASH_SECTOR_7;
	}

	return sector;
}

// Get sector size for a given sector (ST reference)
uint32_t NVS::getSectorSize(uint32_t Sector)
{
	uint32_t sectorsize = 0x00;

	if((Sector == FLASH_SECTOR_0) || (Sector == FLASH_SECTOR_1) || (Sector == FLASH_SECTOR_2) || (Sector == FLASH_SECTOR_3))
	{
		sectorsize = 16 * 1024;
	}
	else if(Sector == FLASH_SECTOR_4)
	{
		sectorsize = 64 * 1024;
	}
	else
	{
		sectorsize = 128 * 1024;
	}
	return sectorsize;
}

// Helper: Get address for a given MIDI bank (0..3)
uint32_t NVS::getPresetAddress(uint8_t preset_bank) {
	return MIDI_PRESETS_BASE_ADDR + (preset_bank * MIDI_PRESET_SIZE);
}

// Helper: Get sector number for a given address (for erase)
uint32_t NVS::getSectorNumber(uint32_t address) {
		return getSector(address);
}
