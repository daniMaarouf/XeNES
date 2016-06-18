#ifndef __NES_PPU__
#define __NES_PPU__

#include <cstdint>

const int NES_SCREEN_WIDTH = 256;
const int NES_SCREEN_HEIGHT = 240;

/* nametable mirroring configuration */
enum Mirroring {
    VERTICAL,
    HORIZONTAL,
    FOUR_SCREEN,
};

struct PPU {

private:

    uint8_t palette[0x20];      
    uint8_t RAM[0x800];          //2kB PPU internal RAM
    uint8_t OAM[0x100];          //256 byte PPU OAM
    
    int ppuCycle;                   //0-341 per scanline
    bool evenFrame;                 //tracks even and odd frames

    uint16_t ppuReadAddress;        //current VRAM address

    uint8_t getPpuByte(uint16_t);   //get byte from PPU address space
    bool setPpuByte(uint16_t, uint8_t);     //set byte in PPU address space
    void printSprites();            //sprint sprites to stdout
    void drawSprites();             //draw sprites to pixel display

public:

    PPU();

    bool ppuGetAddr;            //CPU has written half of address to 0x2006 in CPU address space
    bool readLower;             //lower part of write to 2006 is occuring
    bool ppuReadByte;           //CPU has written byte to 0x2007 in CPU address space

    bool usesRAM;               //true if CHR_RAM is used rather than CHR_ROM

    uint8_t ppuRegisters[0x8];  //PPU registers
    enum Mirroring mirroring;   //nametable arrangement
    uint8_t * CHR_ROM;          //cartridge video ROM
    int scanline;               //current scanline

    bool draw;                  //draw frame?
    uint32_t * pixels;          //pixel display

    int tick(bool *);           //one PPU tick is executed
    void freePointers();        //free memory
};

#endif
/* DEFINED __NES_PPU__ */
