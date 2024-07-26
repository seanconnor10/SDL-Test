#define KEYS_count 6 // Should be the total of keys defined below 

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

#ifndef GAME_STRUCTS
    #define GAME_STRUCTS
    typedef struct PLAYER{
        double x;
        double y;
        double angle;
    } Player;

    typedef struct WALL{
        int x1; int y1;
        int x2; int y2;
    } Wall;

    typedef struct GAME_OBJ_LISTS {
        Wall *walls;
    } GameObjLists;

    GameObjLists gObjs;
    Player p1 = {.x=0.0, .y=0.0, .angle=0.0};
#endif

void gameInit();
void gameRun(double deltaTime, bool keyIsDown[KEYS_count], bool keyJustPressed[KEYS_count], bool keyJustReleased[KEYS_count] );
void gameDraw(uint32_t *pixels, int frameWidth, int frameHeight);
void gameDrawMap(uint32_t *pixels, int frameWidth, int frameHeight);
void gameDrawPlayer(uint32_t *pixels, int frameWidth, int frameHeight);
void gameDrawWorld(uint32_t *pixels, int frameWidth, int frameHeight);
void gameDrawWall(uint32_t *pixels, int wallIndex, int frameWidth, int frameHeight);
void gameSetPixel(uint32_t *pixels, int x, int y, SDL_Color col, int frameWidth, int frameHeight);