#ifndef FIRST_IMPORTS
    #define FIRST_IMPORTS
    #include <stdio.h>
    #include <stdbool.h>
    #include <stdint.h> 
    #include <inttypes.h>
    #include <math.h>
    #include <SDL2/SDL.h>
#endif

#include "game.h"

#include "mathhelpers.h"
#include "drawhelpers.h"

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

void gameInit() {
    #define NUM_WALLS 4

    gObjs.walls = malloc( sizeof(Wall) * NUM_WALLS );

    gObjs.walls[0].x1 = 50.0;
    gObjs.walls[0].y1 = 10.0;
    gObjs.walls[0].x2 = 150.0;
    gObjs.walls[0].y2 = 10.0;
    gObjs.walls[1].x1 = 150.0;
    gObjs.walls[1].y1 = 10.0;
    gObjs.walls[1].x2 = 150.0;
    gObjs.walls[1].y2 = 50.0;
    gObjs.walls[2].x1 = 150.0;
    gObjs.walls[2].y1 = 50.0;
    gObjs.walls[2].x2 = 450.0;
    gObjs.walls[2].y2 = 50.0;
    gObjs.walls[3].x1 = 200.0;
    gObjs.walls[3].y1 = 50.0;
    gObjs.walls[3].x2 = 180.0;
    gObjs.walls[3].y2 = 200.0;
  
    p1.x = 10.0;
    p1.y = 150.0;
    p1.angle = 0.001;
}

void gameRun(double deltaTime, bool keyIsDown[KEYS_count], bool keyJustPressed[KEYS_count], bool keyJustReleased[KEYS_count] ) {

        double speed = 150.0 * deltaTime;
        double pCos = cos(p1.angle);
        double pSin = sin(p1.angle);

        if (keyIsDown[KEYS_INDEX_RIGHT]) {
            p1.y += speed*pCos;
            p1.x += speed*pSin;
        }
        if (keyIsDown[KEYS_INDEX_LEFT]) {
            p1.y -= speed*pCos;
            p1.x -= speed*pSin;
        }
        if (keyIsDown[KEYS_INDEX_BACK]) {
            p1.x -= speed*pCos;
            p1.y += speed*pSin;
        }
        if (keyIsDown[KEYS_INDEX_FORWARD]) {
            p1.x += speed*pCos;
            p1.y -= speed*pSin;
        }
        if (keyIsDown[KEYS_INDEX_TURNLEFT]) {
            p1.angle += 2.0 * deltaTime;
            if (p1.angle > 6.27) p1.angle -= 6.27;
        }
        if (keyIsDown[KEYS_INDEX_TURNRIGHT]) {
            p1.angle -= 2.0 * deltaTime;
            if (p1.angle < 0.0) p1.angle += 6.27;
        }

}

void gameDraw(uint32_t *pixels, int frameWidth, int frameHeight) {
    gameDrawWorld(pixels, frameWidth, frameHeight);
    gameDrawPlayer(pixels, frameWidth, frameHeight);
    gameDrawMap(pixels, frameWidth, frameHeight);
    myPlotLine(20, 20, 300, 100, pixels);
}

void gameDrawPlayer(uint32_t *pixels, int frameWidth, int frameHeight) {
    SDL_Color col = { .r = 0xFF, .g = 0x00, .b = 0x00, .a = 0xFF};
    int radius = 5;
    for (int w = -radius; w<=radius; w++ )
        for (int h = -radius; h<=radius; h++) 
            gameSetPixel(pixels, p1.x-w, p1.y-h, col, frameWidth, frameHeight);
}

void gameDrawWorld(uint32_t *pixels, int frameWidth, int frameHeight) {
    for (int i=0; i<NUM_WALLS; i++){
        gameDrawWall(pixels, i, frameWidth, frameHeight);
    }
}

void gameDrawMap(uint32_t *pixels, int frameWidth, int frameHeight) {
   for (int i=0; i<NUM_WALLS; i++){
        Wall *w = &gObjs.walls[i];
        SDL_Color col = { .r =0xFF, .g=0xAA, .b=0x00, .a=0xFF};
        //v2_int_array *line = plotLine(w->x1, w->y1, w->x2, w->y2);
        v2_int_array *line = plotLine(w->x1, w->y1, w->x2, w->y2, pixels);
        v2_int *points = line->address;
        int l = line->length;
        /*for (int i=0; i<l; i++) {
            gameSetPixel(pixels, points[i].x, points[i].y, col, frameWidth, frameHeight);
        }*/
        free(line->address);
        free(line);
    }

}

void gameDrawWall(uint32_t *pixels, int wallIndex, int frameWidth, int frameHeight) {
    Wall *w = &gObjs.walls[wallIndex];

    SDL_Color col = {.r = 0xCC, .g = 0x90, .b = 0xAA, .a = 0xFF};
    
    double x1, y1, x2, y2;

    //Translate
    x1 = w->x1 - p1.x;
    y1 = w->y1 - p1.y;
    x2 = w->x2 - p1.x;
    y2 = w->y2 - p1.y;

    //Rotate
    double rCos = cos(p1.angle) , rSin = sin(p1.angle);
    double tempX = x1;
    x1 = x1 * rCos - y1 * rSin;
    y1 = y1 * rCos + tempX * rSin;
    tempX = x2;
    x2 = x2 * rCos - y2 * rSin;
    y2 = y2 * rCos + tempX * rSin;

    if (x1 < 0.0 && x2 < 0.0) return; //Avoid drawing if totally behind camera

    int halfScreenWidth = (int) (frameWidth/2.0), halfScreenHeight = (int) (frameHeight/2.0);

    double leftClipU = 0.0, rightClipU = 1.0; //Portion along wall where it's cut off by edge of screen

    double wallLength = dist(x1, y1, x2, y2);

    //Clip wall to screen edges
    if (x1 < 0 ) {
        double slope = (y2-y1)/(x2-x1); //Slope of wall in top-down view
        double yIntercept = y1 - slope*x1;
        leftClipU = sqrt( (-x1)*(-x1) + (yIntercept-y1)*(yIntercept-y1) ) / wallLength;
        x1 = 0.0;
        y1 = yIntercept;
    }

    if (x2 < 0 ) {
        double slope = (y2-y1)/(x2-x1); //Slope of wall in top-down view
        double yIntercept = y1 - slope*x1;
        rightClipU = 1.0 - ( sqrt( (-x2)*(-x2) + (yIntercept-y2)*(yIntercept-y2) ) / wallLength );
        x2 = 0.0;
        y2 = yIntercept;
    }

    if (x1 < 0.01) x1 = 0.01;
    if (x2 < 0.01) x2 = 0.01;

    double FOV = 150.0;
    double depth1 = x1;// / 2.0;
    double depth2 = x2;// / 2.0;

    double point1_plotX = halfScreenWidth + FOV*y1/depth1;
    double point2_plotX = halfScreenWidth + FOV*y2/depth2;

    if (point2_plotX < point1_plotX) return; //Avoid drawing back of wall

    int leftEdge = point1_plotX;
    int rightEdge = point2_plotX;

    // if (leftEdge > spanEnd) return;
    // if (rightEdge > spanStart) return;
    // if ( -=spanFilled=- ) return;

    double SEC_FLOOR_Z = 0.0; double SEC_CEIL_Z = 100.0; double CAM_Z = 50; //Placeholder variables

    double point1_plotY_low = halfScreenHeight + FOV*(SEC_FLOOR_Z - CAM_Z)/depth1;
    double point1_plotY_high = halfScreenHeight + FOV*(SEC_CEIL_Z - CAM_Z)/depth1;
    double point2_plotY_low = halfScreenHeight + FOV*(SEC_FLOOR_Z - CAM_Z)/depth2;
    double point2_plotY_high = halfScreenHeight + FOV*(SEC_CEIL_Z - CAM_Z)/depth2;

    double horizProgress, quadBottom, quadTop, quadHeight;
    int rasterBottom, rasterTop;

    if (leftEdge < 0.0) leftEdge = 0.0;
    if (rightEdge > frameWidth-1) rightEdge = frameWidth - 1;

    double fogFactor = 0.0;

    for (int drawX = leftEdge; drawX <= rightEdge; drawX++) {
        //if (drawX<0) continue;
        //if (drawX>=frameWidth) break;

        //if (drawX >=spanEnd || drawX < spanStart) continue;

        horizProgress = (drawX-point1_plotX) / (point2_plotX-point1_plotX);

        if (horizProgress < 0.0 ) horizProgress = 0.0;
        if (horizProgress > 1.0 ) horizProgress = 1.0;

        quadBottom = lerp_D(point1_plotY_low, point2_plotY_low, horizProgress);
        quadTop = lerp_D(point1_plotY_high, point2_plotY_high, horizProgress);
        quadHeight = quadTop - quadBottom;

        // rasterBottom = (int) Math.max( occlusionBottom[drawX], quadBottom);
        // rasterTop = (int) Math.min( occlusionTop[drawX], quadTop);
        rasterBottom = quadBottom;
        if (rasterBottom < 0 ) rasterBottom = 0;
        rasterTop = quadTop;
        if (rasterTop > frameHeight-1) rasterTop = frameHeight -1;

        //if (rasterTop < rasterBottom) continue; //Unnecessary??

        double u =  ((1.0 - horizProgress)*(leftClipU/x1) + horizProgress*(rightClipU/x2)) 
                    / 
                    ((1.0-horizProgress)*(1.0/x1) + horizProgress*(1.0/ x2));
        if (u<0.0001) u = 0.0001; if (u>0.9999) u = 0.9999; 

        fogFactor = (depth1 + u*(depth2-depth1)) / 300.0;
        if (fogFactor > 1.0 ) fogFactor = 1.0;

        // int texWidth = 128;
        // double texU = u * wallLength /* * tex_x_scale */;
        // //texU += texXOffset;
        // while (texU > 1.0) texU -= 1.0;
        // while (texU < 0.0) texU += 1.0;

        for (int drawY = rasterBottom; drawY < rasterTop; drawY++) {
            double v = (drawY - quadBottom) / quadHeight; // v is portion along wall this draw pixel is
            /*float texY = v * (SEC_CEIL_Z - SEC_FLOOR_Z);
            //texY *= orig.texYScale;
            //texY += orig.texYOffset;
            float texH = 128; //tex.getHeight();
            while (texY > texH) texY -= texH;
            while (texY <= 0) texY += texH;
            texY = texH - texY; //Flip vertically*/

            col.g = lerp_8_D(255, 50, (double) wallIndex / (double) NUM_WALLS );
            col.b = lerp_8_D( (uint8_t) 255,  50, u);
            col.r = lerp_8_D(25, 255, v);
            //col.r = lerp_8_D(50, (uint8_t) 255, v);

            col.r = lerp_8_D( col.r, 0x00 , fogFactor);
            col.g = lerp_8_D( col.g, 0x00 , fogFactor);
            col.b = lerp_8_D( col.b, 0x00 , fogFactor);

            // col.r = (col.r/8)*8;
            // col.g = (col.g/8)*8;
            // col.b = (col.b/8)*8;

            gameSetPixel(pixels, drawX, drawY, col, frameWidth, frameHeight);
        }
    }

}

void gameSetPixel(uint32_t *pixels, int x, int y, SDL_Color col, int frameWidth, int frameHeight) {
    if (x >= frameWidth || x<0 || y >= frameHeight || y<0) return;
    uint32_t pix = ( col.a | (col.g<<8) | (col.b<<16) | (col.r<<24) );
    pixels[ x + y*frameWidth ] = pix;
}
