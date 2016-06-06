#ifndef __NES_NES__
#define __NES_NES__

#include <cstdint>

enum ProcessorStatusIndex {
    N,      //is number negative
    V,      //overflow
    D,      //BCD mode
    I,      //disable interrupts
    Z,      //zero flag
    C       //carry flag
};

enum AddressMode {
    ABS,
    ABSX,
    ABSY,
    ACC,
    IMM,
    IMP,
    IND,
    INDX,
    INDY,
    REL,
    ZRP,
    ZRPX,
    ZRPY,
    NONE
};

class NES {

public:
    uint16_t PC;        //program counter
    uint8_t SP;         //stack pointer
    uint8_t A;          //accumulator
    uint8_t X;          //register X
    uint8_t Y;          //register Y
    bool PS[8];         //processor status word

    uint8_t ioRegisters[0x20];       
    uint8_t ppuRegisters[0x8];  

    int numRomBanks, numRamBanks;
    uint8_t * PRG_ROM, * PRG_RAM, * CHR_ROM;

    uint8_t cpuRAM[0x800];
    uint8_t cpuMem[0x10000];    //fallback memory addresses

    int count;

    //initialize
    NES();
    bool openROM(const char *);


    uint16_t retrieveAddress(enum AddressMode, bool *);

    int debugPrintVal(enum AddressMode);
    void printDebugLine(enum AddressMode, uint16_t);

    //get and set CPU memory
    uint8_t getByte(uint16_t);
    bool setByte(uint16_t, uint8_t);

    int executeNextOpcode(bool, bool);
    

};
                            //0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
const int opcodeLens[0x20] = {2,2,0,2,2,2,2,2,1,2,1,2,3,3,3,3,  //0 2 4 6 8 A C E
                              2,2,0,2,2,2,2,2,1,3,1,3,3,3,3,3}; //1 3 5 7 9 B D F

const enum AddressMode addressModes[] = {
  //0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
    IMP, INDX,NONE,INDX,ZRP ,ZRP ,ZRP ,ZRP ,IMP, IMM, ACC, NONE,ABS, ABS ,ABS, ABS,   //0
    REL, INDY,NONE,INDY,ZRPX,ZRPX,ZRPX,ZRPX,IMP, ABSY,IMP, ABSY,ABSX,ABSX,ABSX,ABSX,  //1
    ABS, INDX,NONE,INDX,ZRP, ZRP, ZRP, ZRP, IMP, IMM, ACC, NONE,ABS, ABS, ABS, ABS,   //2
    REL, INDY,NONE,INDY,ZRPX,ZRPX,ZRPX,ZRPX,IMP, ABSY,IMP, ABSY,ABSX,ABSX,ABSX,ABSX,  //3
    IMP, INDX,NONE,INDX,ZRP, ZRP, ZRP, ZRP, IMP, IMM, ACC, NONE,ABS, ABS, ABS, ABS,   //4
    REL, INDY,NONE,INDY,ZRPX,ZRPX,ZRPX,ZRPX,IMP, ABSY,IMP, ABSY,ABSX,ABSX,ABSX,ABSX,  //5
    IMP, INDX,NONE,INDX,ZRP, ZRP, ZRP, ZRP, IMP, IMM, ACC, NONE,IND, ABS, ABS, ABS,   //6
    REL, INDY,NONE,INDY,ZRPX,ZRPX,ZRPX,ZRPX,IMP, ABSY,IMP, ABSY,ABSX,ABSX,ABSX,ABSX,  //7
    IMM, INDX,NONE,INDX,ZRP, ZRP, ZRP, ZRP, IMP, NONE,IMP, NONE,ABS, ABS, ABS, ABS,   //8
    REL, INDY,NONE,INDY,ZRPX,ZRPX,ZRPY,ZRPY,IMP, ABSY,IMP, NONE,NONE,ABSX,NONE,NONE,  //9
    IMM, INDX,IMM, INDX,ZRP, ZRP, ZRP, ZRP, IMP, IMM, IMP, NONE,ABS, ABS, ABS, ABS,   //A
    REL, INDY,NONE,INDY,ZRPX,ZRPX,ZRPY,ZRPY,IMP, ABSY,IMP, NONE,ABSX,ABSX,ABSY,ABSY,  //B
    IMM, INDX,NONE,INDX,ZRP, ZRP, ZRP, ZRP, IMP, IMM, IMP, NONE,ABS, ABS, ABS, ABS,   //C
    REL, INDY,NONE,INDY,ZRPX,ZRPX,ZRPX,ZRPX,IMP, ABSY,IMP, ABSY,ABSX,ABSX,ABSX,ABSX,  //D
    IMM, INDX,NONE,INDX,ZRP, ZRP, ZRP, ZRP, IMP, IMM, IMP, IMM, ABS, ABS, ABS, ABS,   //E
    REL, INDY,NONE,INDY,ZRPX,ZRPX,ZRPX,ZRPX,IMP, ABSY,IMP, ABSY,ABSX,ABSX,ABSX,ABSX}; //F

#endif
/* DEFINED __NES_NES__ */
