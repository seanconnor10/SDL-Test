#ifndef FIRST_IMPORTS
    #define FIRST_IMPORTS
    #include <stdio.h>
    #include <stdbool.h>
    #include <stdint.h> 
    #include <inttypes.h>
    #include <math.h>
    #include <SDL2/SDL.h>
#endif

#include "getLangVersion.c"
#include "game.c"

#define KEYS_numberOfKeys 6 // Should be the total of keys defined below 

#define KEYS_INDEX_FORWARD 0
#define KEYS_INDEX_BACK 1
#define KEYS_INDEX_LEFT 2
#define KEYS_INDEX_RIGHT 3
#define KEYS_INDEX_TURNLEFT 4
#define KEYS_INDEX_TURNRIGHT 5

#define KEYS_CODE_FORWARD SDLK_w
#define KEYS_CODE_BACK SDLK_s
#define KEYS_CODE_LEFT SDLK_a
#define KEYS_CODE_RIGHT SDLK_d
#define KEYS_CODE_TURNLEFT SDLK_j
#define KEYS_CODE_TURNRIGHT SDLK_k

void clearScreen(SDL_Renderer  *r);
void clearFrame(uint32_t *pixels);
void setFrameToCheckerboard(uint32_t *pixels);
void setFrameSize(int w, int h, SDL_Texture *tex, SDL_Renderer *r);
void drawFrame(SDL_Renderer *r, SDL_Texture *frameTex, void* pixels, int pitch);
void beginDrawFrame(SDL_Renderer *r, SDL_Texture *frameTex, void *pixels, int *pitch);
void setPixel(uint32_t* pixels, int x, int y, uint8_t r, uint8_t b, uint8_t g, uint8_t a);
void setPixelCol(uint32_t *pixels, int x, int y, SDL_Color col);
void setKeyInputs(SDL_Event *e, bool state[KEYS_numberOfKeys], bool justPressed[KEYS_numberOfKeys], bool justReleased[KEYS_numberOfKeys] );
void setDeltaTime(uint64_t *stamp, double *dt);

int frameWidth = 480, frameHeight = 270;

int main(int argc, char *argv[]) {

    printf("\n\n  --== WELCOME ==--  \n\n");

    getLangVersion();

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Window* window = SDL_CreateWindow("SDL TEST", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, frameWidth, frameHeight, 0);
    printf("-= PIXEL FORMAT: %s ( code %d ) =-\n\n", SDL_GetPixelFormatName(SDL_GetWindowPixelFormat(window)), SDL_GetWindowPixelFormat(window));
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE  );
    SDL_Texture* frameBuffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, frameWidth, frameHeight);
    uint32_t* framePixels = malloc( sizeof(*framePixels) * frameWidth * frameHeight );
    int framePitch = frameWidth * sizeof(uint32_t);
    void* framePixelsVoidPoint = framePixels;
    SDL_Event events;

    bool gameRunning = true;

    bool keyIsDown[KEYS_numberOfKeys];
    bool keyJustPressed[KEYS_numberOfKeys];
    bool keyJustReleased[KEYS_numberOfKeys];
    for (int i=0; i<KEYS_numberOfKeys; i++) { keyIsDown[i] = false; keyJustPressed[i] = false; keyJustReleased[i] = false; }

    uint64_t timestamp = SDL_GetPerformanceCounter();
    double deltaTime = 0.0;

    double tempPlayerX = frameWidth - 10;
    double tempPlayerY = frameHeight -10;

    gameInit();

    while (gameRunning) { 

        setDeltaTime( &timestamp, &deltaTime);

        //EVENTS =============================
        
        while(SDL_PollEvent(&events)) {
            if ( events.type == SDL_QUIT ) {
                gameRunning = false;
                break;
            } else if ( events.type == SDL_KEYDOWN || events.type == SDL_KEYUP ) {
                setKeyInputs( &events, keyIsDown, keyJustPressed, keyJustReleased );
                break; 
            }
        }

        //GAME LOGIC ========================

        gameRun(deltaTime, keyIsDown, keyJustPressed, keyJustReleased);

        //DRAW ==============================
        //clearScreen(renderer);
        //clearFrame(framePixels);
        setFrameToCheckerboard(framePixels);
        beginDrawFrame(renderer, frameBuffer, framePixels, &framePitch);

        //setFrameToCheckerboard(framePixels);
        gameDraw(framePixels, frameWidth, frameHeight);

        drawFrame(renderer, frameBuffer, framePixelsVoidPoint, framePitch);
        SDL_RenderPresent(renderer);
    }

    printf("\n  --== GOODBYE ==--  \n\n");
        
}

void setFrameSize(int w, int h, SDL_Texture *tex, SDL_Renderer *r) {
    SDL_DestroyTexture(tex);
    tex = SDL_CreateTexture(r, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, frameWidth, frameHeight);
}

void clearScreen(SDL_Renderer *r) {
    SDL_SetRenderDrawColor(r, 100, 10, 20, 0);   
    SDL_RenderClear(r);
}

void clearFrame(uint32_t *pixels) {
    int num = frameHeight*frameWidth;
    for (int i=0; i<num; i++)
        pixels[i] = 0x200510FF;
}

void setFrameToCheckerboard(uint32_t *pixels) {
    SDL_Color red = { .a = 0xFF, .r = 0xCF, .g=0x20, .b = 0x10 };
    SDL_Color green = { .a = 0xFF, .r = 0x10, .g=0xAE, .b = 0x40 };
    SDL_Color blue = { .a = 0xFF, .r = 0x10, .g=0x15, .b = 0xB0 };
    for (int ix=0; ix<frameWidth; ix++) {
        for (int iy=0; iy<frameHeight; iy++) {
            if ( (ix%40 < 20) == (iy%40 < 20) )
                setPixelCol(pixels, ix, iy, blue);
            else
                setPixelCol(pixels, ix, iy, green);
        }
    }
}

void drawFrame(SDL_Renderer *r, SDL_Texture *frameTex, void* pixels, int pitch) {
    SDL_UnlockTexture(frameTex);
    SDL_UpdateTexture(frameTex, NULL, pixels, pitch);
    SDL_RenderCopy(r, frameTex, NULL, NULL);
}

void beginDrawFrame(SDL_Renderer *r, SDL_Texture *frameTex, void* pixels, int *pitch) {
    if( SDL_LockTexture(frameTex, NULL, &pixels, pitch) < 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Lock Tex Failure %s\n", SDL_GetError);  
    }
}

void setPixel(uint32_t* pixels, int x, int y, uint8_t r, uint8_t b, uint8_t g, uint8_t a) {
    if (x >= frameWidth || x<0 || y>= frameHeight || y<0) return;
    uint32_t pix = ( a | (g << 8) | (b << 16) | (r << 24) );
    //printf("%08"PRIx32": ", col); //Prints col as hex
    int arrPos = x + y*frameWidth;
    pixels[arrPos] = pix;
}

void setPixelCol(uint32_t *pixels, int x, int y, SDL_Color col) {
    setPixel(pixels, x, y, col.r, col.g, col.b, col.a);
}

int mapKeyIndex(int code) {
    switch(code) {
        case KEYS_CODE_FORWARD:
            return KEYS_INDEX_FORWARD;
        case KEYS_CODE_BACK:
            return KEYS_INDEX_BACK;
        case KEYS_CODE_RIGHT:
            return KEYS_INDEX_RIGHT;
        case KEYS_CODE_LEFT:
            return KEYS_INDEX_LEFT;
        case KEYS_CODE_TURNLEFT:
            return KEYS_INDEX_TURNLEFT;
        case KEYS_CODE_TURNRIGHT:
            return KEYS_INDEX_TURNRIGHT;
        default:
    }
    return -1;
}

void setKeyInputs(SDL_Event *e, bool state[KEYS_numberOfKeys], bool justPressed[KEYS_numberOfKeys], bool justReleased[KEYS_numberOfKeys] ) {
    bool isDownEvent = e->type == SDL_KEYDOWN;

    for (int i=0; i<KEYS_numberOfKeys; i++) { justPressed[i] = false; justReleased[i] = false; }

    int keyInd = mapKeyIndex( e->key.keysym.sym );

    if (keyInd >= 0 && keyInd < KEYS_numberOfKeys) {
        justPressed[keyInd]  = ( isDownEvent && state[keyInd] == false );
        justReleased[keyInd] = ( !isDownEvent && state[keyInd] == true  );
        state[ keyInd ] = isDownEvent;
    }

    // for (int i=0; i<KEYS_numberOfKeys; i++) { //Print Key States
    // if ( justPressed[i] )
    //     printf("Key %d just pressed!!\n", i);
    // else if ( justReleased[i])
    //     printf("Key %d just released...\n", i);
    // else
    //     printf("Key %d is %d!!\n", i, state[i]);
    // }
    // printf("\n");
    
}

void setDeltaTime(uint64_t *stamp, double *dt) {
    uint64_t thisTime = SDL_GetPerformanceCounter();
    double newDelta = (double) ( (thisTime - *stamp) / (double) SDL_GetPerformanceFrequency() );
    //if (newDelta < 0) newDelta = 0;
    //if (newDelta > 0.1) newDelta = 0.1;
    *dt = newDelta;
    *stamp = thisTime;
    //Print FPS Draw FPS
    int framesPerSecond = 1.0 / newDelta;
    int temp = framesPerSecond;
    if ( temp % 10 == 0) {
        printf("FPS: %d\n", framesPerSecond);
    }
}
