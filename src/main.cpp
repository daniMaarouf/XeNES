#include <iostream>

#include "NES.hpp"
#include "gameLoop.hpp"

int main(int argc, char ** argv) {

    if (argc != 2) {
        std::cerr << "Invalid number of arguments. Expected one. Received " << argc - 1 << std::endl;
        std::cout << "Please call program with format: '" << argv[0] << " ROM.nes'" << std::endl;
        return 1;
    }

    /*
    todo:

    real ntsc ppu rendering algorithm
    configurable getByte and setByte for mappers
    read/write PPUDATA fix
    oamdata read/write
    clean up code
    automated testing where test programs are run, pixels are hashed and compared to known good result

    add 8x16 sprites

    */

    NES nesSystem;

    if (!nesSystem.openCartridge(argv[1])) {
        std::cerr << "Could not load ROM :" << argv[1] << std::endl;
        nesSystem.closeCartridge();
        return 1;
    }

    loop(nesSystem, argv[1]);

    nesSystem.closeCartridge();
    return 0;
}
