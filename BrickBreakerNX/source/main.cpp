/*  Nintendo Switch homebrew projects 
    by Kenneth van der Maazen (c) 2024
    
    Brick Breaker type game created in C++ using the SDL2 Library.
    it's a very basic homebrew concept made without any official instructions or support from Nintendo.
    I created this for educational purposes and to learn more about game development.
    Homebrews are not supported by Nintendo and may not work on all systems.

    Date created: 29-10-2024
    Last updated: 29-10-2024 @ 
*/

#include <iostream>


#include <switch.h>
#include <SDL2/SDL.h>

using namespace std;




const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;


/* GLOBALE VARIABELEN */
bool quit = false;

SDL_Event event;

int ballx = 10;
int bally = 10;
int ballvelx = 1;   // Ball snelheid x-axis
int ballvely = 1;   // Ball snelheid y-axis

int bkw = 1280;
int bkh = 720;
int bkwmin = 0;
int bkhmin = 0;
int batx = bkw / 2;
int baty = bkh - 30;

int brickw = 80;
int brickh = 34;

SDL_Surface *brick;
SDL_Texture *bricktexture;

const int numRows = 3;
const int numCols = 8;

SDL_Rect brickrect[numRows][numCols];

SDL_Rect ballrect;



void InitializeBricks() {
    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            brickrect[i][j] = {50 + j* (brickw + 10), 50 + i * (brickh + 10), brickw, brickh };
        }
    }
}

// Functie om invoer te verwerken en de controller toetsaanslagen af te handelen
void EventHandler(PadState *pad) {
    SDL_PollEvent(&event);
    if (event.type == SDL_QUIT) {
        quit = true;
    }


    // Haal de knoppeninvoer op van controller
    u64 kDown = padGetButtonsDown(pad);
    u64 kHeld = padGetButtons(pad);

    // Verplaats de bat naar links en rechts afhankelijk van invoer
    if (kHeld & HidNpadButton_Left && batx > 0) {
        //batx -= 5; // Verplaatst naar links
        batx = batx - 2;
    }
    if (kHeld & HidNpadButton_Right && batx < bkw - 60) {
        //batx += 5; // Verplaatst naar rechts
        batx = batx + 2;
    }
}

void moveBall() {
    ballx = ballx + ballvelx;
    bally = bally + ballvely;
}

void ball_collision() {
    if (ballx < bkwmin || ballx > bkw - 30) {
        ballvelx = -ballvelx;
    }
    if (bally < bkhmin || bally > bkh - 30) {
        ballvely = -ballvely;
    }

    int ballscaling = 20;

    if (bally + ballscaling >= baty && bally + ballscaling <= baty + 30 && ballx + ballscaling >= batx && ballx + ballscaling <= batx + 60) {
        ballvely = -ballvely;
    }
}

bool ball_brick_collision_detect(SDL_Rect rect1, SDL_Rect rect2) {
    if (rect1.x > rect2.x + rect2.w) {
        return false;
    }
    if (rect1.x + rect1.w < rect2.x) {
        return false;
    }
    if (rect1.y > rect2.y + rect2.h) {
        return false;
    }
    if (rect1.y + rect1.h < rect2.y) {
        return false;
    }
    return true;
}

void ball_brick_collision() {
    bool a;

    for (int i = 0; i < numRows; i++) {
        for (int j = 0; j < numCols; j++) {
            a = ball_brick_collision_detect(brickrect[i][j],ballrect);

            if (a == true) {

                brickrect[i][j].x = 3000;
                ballvely = -ballvely;

            }
            a = false;
        }
    }
}

// Main applicatie enrtypoint
int main (int argc, char* argv[]) 
{
    romfsInit();
    chdir("romfs:/");    

    // Configureer de controller invoer
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);


    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window* window = SDL_CreateWindow("Brick Breaker", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Rect bkrect = {0,0,SCREEN_WIDTH,SCREEN_HEIGHT};
    SDL_Rect batrect = {batx, baty, 60, 30};
    

    SDL_Surface *ball = SDL_LoadBMP("gfx/ball.bmp");
    SDL_Surface *bk = SDL_LoadBMP("gfx/bk.bmp");
    SDL_Surface *bat = SDL_LoadBMP("gfx/bat.bmp");
    brick = SDL_LoadBMP("gfx/brick.bmp");
    InitializeBricks();
    SDL_Texture *balltexture = SDL_CreateTextureFromSurface(renderer,  ball);
    SDL_Texture *bktexture = SDL_CreateTextureFromSurface(renderer,  bk);
    SDL_Texture *battexture = SDL_CreateTextureFromSurface(renderer,  bat);
    bricktexture = SDL_CreateTextureFromSurface(renderer,  brick);


    SDL_RenderCopy(renderer, bktexture, NULL, &bkrect);


    // Game Loop (Main)
    while (!quit)
    {
        // Verwerk de controller invoer
        padUpdate(&pad);
        EventHandler(&pad);


        ballrect = {ballx, bally, 20, 30};
        SDL_Rect batrect = {batx, baty, 60, 30};
        
        moveBall();
        ball_collision();
        //InitializeBricks();

        ball_brick_collision();

        //SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bktexture, NULL, &bkrect);
        SDL_RenderCopy(renderer, balltexture, NULL, &ballrect);
        SDL_RenderCopy(renderer, battexture, NULL, &batrect);

        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                SDL_RenderCopy(renderer, bricktexture, NULL, &brickrect[i][j]);
            }
        }

        SDL_RenderPresent(renderer);

        SDL_RenderClear(renderer);
    }
    
    // Code opruimen
    SDL_DestroyTexture(balltexture);
    SDL_DestroyTexture(bktexture);
    SDL_DestroyTexture(battexture);
    SDL_FreeSurface(ball);
    SDL_FreeSurface(bk);
    SDL_FreeSurface(bat);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    romfsExit();
    return 0;
}