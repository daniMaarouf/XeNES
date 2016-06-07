#include <iostream>
#include <fstream>
#include <bitset>

#include "NES.hpp"

NES::NES() {

    for (int x = 0; x < 0x10000; x++) cpuMem[x] = 0x0;
    for (int x = 0; x < 0x800; x++) cpuRAM[x] = 0x0;
    for (int x = 0; x < 8; x++) PS[x] = false;

    PC = 0x8000;
    SP = 0xFD;
    A  = 0x0;
    X  = 0x0;
    Y  = 0x0;

    cpuCycle = 0;
    ppuCycle = 0;

    PS[I] = true;

    return;
}

void NES::freeCartridgePointers() {
	if (PRG_ROM != NULL) {
		delete [] PRG_ROM;
	}
	if (PRG_RAM != NULL) {
		delete [] PRG_RAM;
	}
	if (CHR_ROM != NULL) {
		delete [] CHR_ROM;
	}
    
    return;
}

bool NES::openCartridge(const char * fileLoc) {

    if (fileLoc == NULL) {
        return false;
    }

    std::ifstream romFile;
    romFile.open(fileLoc, std::ios::binary);

    if (!romFile.is_open()) {
        return false;
    }

    uint8_t flags[16];

    uint8_t prg_rom_size = 0;   //number of 16KB ROM blocks
    uint8_t chr_rom_size = 0;   //number of 8KB VROM blocks
    uint8_t prg_ram_size = 0;   //number of 8KB RAM blocks

    uint8_t mapperNumber;

    bool batteryRAM = false;
    bool trainer = false;

    bool NTSC;      //false = PAL

    int prgRomBytes, chrRomBytes, prgRamBytes;

    int index;
    index = 0;

    while (romFile) {
        char c;
        romFile.get(c);
        unsigned char u = (unsigned char) c;
        int binaryValue = (int) u;

        if (index < 16) {
            flags[index] = binaryValue;
            index++;
        } else {
            if (index == 16) {
                if (flags[0] != 0x4E || flags[1] != 0x45 || flags[2] != 0x53 || flags[3] != 0x1A) {
                    std::cerr << "iNes file format not recognized!" << std::endl;
                    romFile.close();
                    return false;
                }
                prg_rom_size = flags[4];
                chr_rom_size = flags[5];

                if (prg_rom_size == 1) {
                	PC = 0xC000;
                } else if (prg_rom_size == 2) {
                	PC = 0x8000;
                } else {
                	std::cerr << "Unrecognized rom size " << (int) prg_rom_size << std::endl;
                	return false;
                }

                prgRomBytes = prg_rom_size * 0x4000;

                if (chr_rom_size == 0) {
                	chrRomBytes = 0x2000;
                	usesRAM = true;
                } else {
					chrRomBytes = chr_rom_size * 0x2000;	
					usesRAM = false;
                }

                PRG_ROM = new uint8_t[prgRomBytes];

                if (PRG_ROM == NULL) {
                    return false;
                }

                CHR_ROM = new uint8_t[chrRomBytes];

                if (CHR_ROM == NULL) {
                    delete [] PRG_ROM;
                    return false;
                }

                PRG_RAM = new uint8_t[prgRamBytes];

                if (PRG_RAM == NULL) {
                    delete [] CHR_ROM;
                    delete [] PRG_ROM;
                    return false;
                }

                for (int a = 0; a < prgRamBytes; a++) {
                    PRG_RAM[a] = 0x0;
                }

                //upper nybble of flag 6 is lower nybble of mapper number 
                mapperNumber = (flags[6] >> 4);

                //flags 6
                if ((flags[6] & 0x9) == 0x0) {
                	mirroring = HORIZONTAL;
                    //std::cout << "vertical arrangement, horizontal mirroring, CIRAM A10 = PPU A11" << std::endl;
                } else if ((flags[6] & 0x9) == 0x1) {
                	mirroring = VERTICAL;
                    //std::cout << "horizontal arrangement, vertical mirroring, CIRAM A10 = PPU A10" << std::endl;
                } else if ((flags[6] & 0x8) == 0x8) {
                	mirroring = FOUR_SCREEN;
                    std::cout << "four screen VRAM" << std::endl;
                    return false;
                } else {
                    //std::cerr << "Could not identify mirroring for " << (std::bitset<8>) flags[6] << std::endl;
                    romFile.close();
                    return false;
                }
                if ((flags[6] & 0x2) == 0x2) {
                    batteryRAM = true;
                }
                if ((flags[6] & 0x4) == 0x4) {
                    std::cout << "Trainer present" << std::endl;
                    trainer = true;
                    return false;
                }

                mapperNumber |= (flags[7] & 0xF0);

                if ((flags[7] & 0xC) == 0x8) {
                    std::cout << "NES 2.0 format" << std::endl;
                    return false;
                }
                if ((flags[7] & 0x1) == 0x1) {
                    std::cout << "VS Unisystem" << std::endl;
                    return false;
                }
                if ((flags[7] & 0x2) == 0x2) {
                    std::cout << "Playchoice-10" << std::endl;
                    return false;
                }

                prg_ram_size = flags[8];

                if (prg_ram_size == 0) {
                    prgRamBytes = 0x2000;
                } else {
                    prgRamBytes = (prg_ram_size * 0x2000);
                }


                if ((flags[9] & 0x1) == 0x1) {
                    NTSC = false;
                } else {
                    NTSC = true;
                }
            }

            if (trainer) {
                romFile.close();
                return false;
            }

            if (index < 16 + prgRomBytes) {
                PRG_ROM[index - 16] = binaryValue;
            } else if (index < 16 + prgRomBytes + chrRomBytes) {
                CHR_ROM[index - (16 + prgRomBytes)] = binaryValue;
            }
            
            index++;
        }
    }

    numRomBanks = prg_rom_size;

    romFile.close();
    return true;
}
