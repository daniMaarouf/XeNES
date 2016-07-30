#include <iostream>
#include <cstring>
#include <bitset>
#include <SDL2/SDL.h>

#include "NES.hpp"

const int FPS = 60;
const int TICKS_PER_FRAME = 1000/FPS;
const int scaleFactor = 2;      //size of each NES display pixel in real pixels
const bool removeOverscan = false;

//obtained from blargg's Full Palette demo
const uint32_t paletteTable [] =
 {  
    ( 84 << 16) | ( 84 << 8) | ( 84),  (  0 << 16) | ( 30 << 8) | (116),  (  8 << 16) | ( 16 << 8) | (144),  ( 48 << 16) | (  0 << 8) | (136),
    ( 68 << 16) | (  0 << 8) | (100),  ( 92 << 16) | (  0 << 8) | ( 48),  ( 84 << 16) | (  4 << 8) | (  0),  ( 60 << 16) | ( 24 << 8) | (  0),
    ( 32 << 16) | ( 42 << 8) | (  0),  (  8 << 16) | ( 58 << 8) | (  0),  (  0 << 16) | ( 64 << 8) | (  0),  (  0 << 16) | ( 60 << 8) | (  0),
    (  0 << 16) | ( 50 << 8) | ( 60),  (  0 << 16) | (  0 << 8) | (  0),  (  0 << 16) | (  0 << 8) | (  0),  (  0 << 16) | (  0 << 8) | (  0),
    (152 << 16) | (150 << 8) | (152),  (  8 << 16) | ( 76 << 8) | (196),  ( 48 << 16) | ( 50 << 8) | (236),  ( 92 << 16) | ( 30 << 8) | (228),
    (136 << 16) | ( 20 << 8) | (176),  (160 << 16) | ( 20 << 8) | (100),  (152 << 16) | ( 34 << 8) | ( 32),  (120 << 16) | ( 60 << 8) | (  0),
    ( 84 << 16) | ( 90 << 8) | (  0),  ( 40 << 16) | (114 << 8) | (  0),  (  8 << 16) | (124 << 8) | (  0),  (  0 << 16) | (118 << 8) | ( 40), 
    (  0 << 16) | (102 << 8) | (120),  (  0 << 16) | (  0 << 8) | (  0),  (  0 << 16) | (  0 << 8) | (  0),  (  0 << 16) | (  0 << 8) | (  0),
    (236 << 16) | (238 << 8) | (236),  ( 76 << 16) | (154 << 8) | (236),  (120 << 16) | (124 << 8) | (236),  (176 << 16) | ( 98 << 8) | (236), 
    (228 << 16) | ( 84 << 8) | (236),  (236 << 16) | ( 88 << 8) | (180),  (236 << 16) | (106 << 8) | (100),  (212 << 16) | (136 << 8) | ( 32),
    (160 << 16) | (170 << 8) | (  0),  (116 << 16) | (196 << 8) | (  0),  ( 76 << 16) | (208 << 8) | ( 32),  ( 56 << 16) | (204 << 8) | (108),
    ( 56 << 16) | (180 << 8) | (204),  ( 60 << 16) | ( 60 << 8) | ( 60),  (  0 << 16) | (  0 << 8) | (  0),  (  0 << 16) | (  0 << 8) | (  0),
    (236 << 16) | (238 << 8) | (236),  (168 << 16) | (204 << 8) | (236),  (188 << 16) | (188 << 8) | (236),  (212 << 16) | (178 << 8) | (236),
    (236 << 16) | (174 << 8) | (236),  (236 << 16) | (174 << 8) | (212),  (236 << 16) | (180 << 8) | (176),  (228 << 16) | (196 << 8) | (144),
    (204 << 16) | (210 << 8) | (120),  (180 << 16) | (222 << 8) | (120),  (168 << 16) | (226 << 8) | (144),  (152 << 16) | (226 << 8) | (180),
    (160 << 16) | (214 << 8) | (228),  (160 << 16) | (162 << 8) | (160),  (  0 << 16) | (  0 << 8) | (  0),  (  0 << 16) | (  0 << 8) | (  0)
};

SDL_Window * window = NULL;     //SDL window
SDL_Renderer * renderer = NULL; //SDL renderer
SDL_Texture * texture = NULL;   //SDL texture

uint32_t localPixels[256 * 240];

static bool initSDL(const char *);
static void closeSDL();
static void draw(uint8_t *);
static bool processEvents(SDL_Event *, uint8_t *);

void loop(NES nesSystem, const char * fileLoc) {

    if (!initSDL(fileLoc)) {
        std::cerr << "SDL did not initialize, quitting" << std::endl;
        return;
    }

    for (int x = 0; x < 256 * 240; x++) {
        localPixels[x] = 0;
    }

    bool running = true;
    int startTime = SDL_GetTicks();;
    SDL_Event event;

    draw(nesSystem.nesPPU.pixels);

    while (running) {

        //1. process events
        if (!processEvents(&event, &nesSystem.controllerByte)) {
            running = false;
            break;
        }

        do {

            //2.1 logic (cpu)
            int executeResult = nesSystem.executeOpcode(false);
            if (executeResult == 0) {
                std::cerr << "Error executing opcode" << std::endl;
                running = false;
                break;
            }

            //2.2. logic (ppu)
            nesSystem.tickPPU(3 * executeResult);

            
            if (SDL_GetTicks() - startTime >= TICKS_PER_FRAME) {
                break;
            }
            
            

        } while (!nesSystem.drawFlag());

        //3.1 draw
        draw(nesSystem.nesPPU.pixels);
        
        //3.2 sync framerate
        int frameTime;
        frameTime = SDL_GetTicks() - startTime;

        if (frameTime < TICKS_PER_FRAME) {
            SDL_Delay(TICKS_PER_FRAME - frameTime);
            
        }

        startTime = SDL_GetTicks();
    }
    
    

    closeSDL();

    return;
}

static bool initSDL(const char * fileLoc) {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Could not initialize SDL : " << SDL_GetError() << std::endl;
        return false;
    }

    //set window title
    char windowTitle[100];
    strcpy(windowTitle, "XeNES: ");
    strcat(windowTitle, fileLoc);

    int outputHeight;

    if (removeOverscan) {
        outputHeight = (NES_SCREEN_HEIGHT - 16);
    } else {
        outputHeight = NES_SCREEN_HEIGHT;
    }

    window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        NES_SCREEN_WIDTH * scaleFactor, outputHeight * scaleFactor, 0);

    if (window == NULL) {
        std::cerr << "Could not create SDL window : " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (renderer == NULL) {
        std::cerr << "Could not create SDL renderer : " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, NES_SCREEN_WIDTH, outputHeight);

    if (texture == NULL) {
        std::cerr << "Could not create SDL texture : " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0); 

    return true;
}

static void closeSDL() {
    if (texture != NULL) {
        SDL_DestroyTexture(texture);
        texture = NULL;
    }
    if (renderer != NULL) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if (window != NULL) {
        SDL_DestroyWindow(window);
        window = NULL;
    }

    SDL_Quit();
    return;
}

static void draw(uint8_t * pixels) {

    for (int x = 0; x < 256 * 240; x++) {
        localPixels[x] = paletteTable[pixels[x]];
    }

    uint32_t * sdlPixels;

    if (removeOverscan) {
        sdlPixels = localPixels + NES_SCREEN_WIDTH * 8;
    } else {
        sdlPixels = localPixels;
    }

    SDL_UpdateTexture(texture, NULL, sdlPixels, NES_SCREEN_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    return;
}

static bool processEvents(SDL_Event * event, uint8_t * controller) {

    SDL_PollEvent(event);
    switch(event->type) {
        case SDL_QUIT:
        return false;

        case SDL_KEYDOWN: {
            switch(event->key.keysym.sym) {

                case SDLK_SPACE:    //A
                (*controller) |= 0x1;
                break;

                case SDLK_LCTRL:    //B
                (*controller) |= 0x2;
                break;

                case SDLK_c:    //select
                (*controller) |= 0x4;
                break;

                case SDLK_x:    //start
                (*controller) |= 0x8;
                break;

                case SDLK_w:    //up
                (*controller) |= 0x10;
                break;

                case SDLK_s:    //down
                (*controller) |= 0x20;
                break;

                case SDLK_a:    //left
                (*controller) |= 0x40;
                break;

                case SDLK_d:    //right
                (*controller) |= 0x80;
                break;

                default:
                break;

            }
            break;
        }

        case SDL_KEYUP: {
            switch(event->key.keysym.sym) {

                case SDLK_SPACE:    //A
                (*controller) &= ~0x1;
                break;

                case SDLK_LCTRL:    //B
                (*controller) &= ~0x2;
                break;

                case SDLK_c:    //select
                (*controller) &= ~0x4;
                break;

                case SDLK_x:    //start
                (*controller) &= ~0x8;
                break;

                case SDLK_w:    //up
                (*controller) &= ~0x10;
                break;

                case SDLK_s:    //down
                (*controller) &= ~0x20;
                break;

                case SDLK_a:    //left
                (*controller) &= ~0x40;
                break;

                case SDLK_d:    //right
                (*controller) &= ~0x80;
                break;

                default:
                break;

            }
            break;
        }

        default:
        break;
    }

    return true;
}