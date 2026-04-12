/*
 * nvs.h
 *
 *  Created on: Jun 12, 2024
 * 	Author: demian
 */

#ifndef NVS_H_
#define NVS_H_

#include "stdio.h"
#include "module_state.h"


#ifdef __cplusplus
extern "C"
{
#endif

typedef enum {
	NVS_SECTOR_MIDI_PRESETS = 0,
	NVS_SECTOR_UI_PRESETS,
} nvs_key_e;

class NVS
{
private:
	void read(nvs_key_e sector, void * out_data, size_t data_size);
	void write(nvs_key_e sector, void * in_data, size_t data_size);
	static uint32_t getSector(uint32_t Address);
	static uint32_t getSectorSize(uint32_t Sector);
	static uint32_t getPresetAddress(uint8_t preset_bank);
	static uint32_t getSectorNumber(uint32_t address);
	
public:
	int saveModulePreset(uint8_t preset_bank, module_state_t * preset);
	int loadModulePreset(uint8_t preset_bank, module_state_t * preset);
	bool isInitialized();
	void writeInitMarker();
};

#ifdef __cplusplus
}
#endif


#endif /* NVS_H_ */
