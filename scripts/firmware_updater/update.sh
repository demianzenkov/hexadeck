# Install dependencies if not present
if ! brew list libusb &>/dev/null; then
  echo "Installing libusb via Homebrew..."
  brew install libusb
fi

if ! command -v python3 &>/dev/null; then
  echo "Python3 not found. Please install Python3"
  exit 1
fi

if ! python3 -m pip show pyusb &>/dev/null; then
  echo "Installing pyusb for Python3..."
  python3 -m pip install --user pyusb
fi

if ! python3 -m pip show pyfu-usb &>/dev/null; then
  echo "Installing pyfu-usb for Python3..."
  python3 -m pip install --user pyfu-usb
fi

export DYLD_LIBRARY_PATH=$(brew --prefix libusb)/lib

BIN_FILE="./scripts/firmware_updater/binaries/8dof_controller.bin"
ADDRESS="0x08000000"

if [ ! -f "$BIN_FILE" ]; then
  echo "Firmware binary not found: $BIN_FILE"
  exit 1
fi

# Check for DFU devices
DFU_DEVICES=$(pyfu-usb -l | grep -E "ID [0-9a-fA-F]+:[0-9a-fA-F]+")
if [ -z "$DFU_DEVICES" ]; then
  echo "No DFU devices found. Please connect your Hexadeck controller and send /fw/update/ command."
  exit 1
fi

pyfu-usb -D "$BIN_FILE" -a "$ADDRESS"