#include <iostream>

#include "NES.hpp"

uint8_t NES::getPpuByte(uint16_t address) {
	address %= 0x4000;

	if (address >= 0x3000 && address < 0x3F00) {
		address -= 0x1000;		//0x3000-0x3EFF map to 0x2000-0x2EFF
	}

	if (address >= 0x0 && address < 0x2000) {
		return CHR_ROM[address];
	} else {

		if (mirroring == HORIZONTAL) {

			if (address >= 0x2000 && address < 0x2400) {
				return ppuRAM[address - 0x2000];
			} else if (address >= 0x2400 && address < 0x2800) {
				return ppuRAM[address - 0x2400];
			} else if (address >= 0x2800 && address < 0x2C00) {
				return ppuRAM[address - 0x2400];
			} else if (address >= 0x2C00 && address < 0x3000) {
				return ppuRAM[address - 0x2800];
			}

		} else if (mirroring == VERTICAL) {

			if (address >= 0x2000 && address < 0x2400) {
				return ppuRAM[address - 0x2000];
			} else if (address >= 0x2400 && address < 0x2800) {
				return ppuRAM[address - 0x2000];
			} else if (address >= 0x2800 && address < 0x2C00) {
				return ppuRAM[address - 0x2800];
			} else if (address >= 0x2C00 && address < 0x3000) {
				return ppuRAM[address - 0x2800];
			}

		} else {
			std::cerr << "Mirroring not recognized in getPpuByte()" << std::endl;
			return 0;
		}
	}

	if (address >= 0x3F00 && address < 0x4000) {
		return palette[(address - 0x3F00) % 0x20];
	}

	return 0;
}

bool NES::setPpuByte(uint16_t address, uint8_t byte) {
	address %= 0x4000;

	if (address >= 0x3000 && address < 0x3F00) {
		address -= 0x1000;		//0x3000-0x3EFF map to 0x2000-0x2EFF
	}

	if (address >= 0x0 && address < 0x2000) {
		CHR_ROM[address] = byte;
		if (usesRAM == false) {
			std::cerr << "Warning, modified ROM" << std::endl;
		}
		return true;
	} else {

		if (mirroring == HORIZONTAL) {

			if (address >= 0x2000 && address < 0x2400) {
				ppuRAM[address - 0x2000] = byte;
				return true;
			} else if (address >= 0x2400 && address < 0x2800) {
				ppuRAM[address - 0x2400] = byte;
				return true;
			} else if (address >= 0x2800 && address < 0x2C00) {
				ppuRAM[address - 0x2400] = byte;
				return true;
			} else if (address >= 0x2C00 && address < 0x3000) {
				ppuRAM[address - 0x2800] = byte;
				return true;
			}

		} else if (mirroring == VERTICAL) {

			if (address >= 0x2000 && address < 0x2400) {
				ppuRAM[address - 0x2000] = byte;
				return true;
			} else if (address >= 0x2400 && address < 0x2800) {
				ppuRAM[address - 0x2000] = byte;
				return true;
			} else if (address >= 0x2800 && address < 0x2C00) {
				ppuRAM[address - 0x2800] = byte;
				return true;
			} else if (address >= 0x2C00 && address < 0x3000) {
				ppuRAM[address - 0x2800] = byte;
				return true;
			}
		} else {
			std::cerr << "Mirroring not recognized in getPpuByte()" << std::endl;
			return false;
		}


	}

	if (address >= 0x3F00 && address < 0x4000) {
		palette[(address - 0x3F00) % 0x20] = byte;
		return true;
	}

	return false;
}

void NES::ppuTick() {

	return;
}

int NES::getPpuCycle() {
	return ppuCycle;
}

void NES::setPpuCycle(int num) {
	ppuCycle = num;
	return;
}
