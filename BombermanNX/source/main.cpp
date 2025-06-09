/*  Nintendo Switch homebrew projects 
    by Kenneth van der Maazen (c) 2025

    Bomberman clone for the Nintendo Switch homebrew environment.
    This is a simplified version of the classic game.


    Build: 0.0.1
        Made a working game loop
        Implemented SDL2 libraries
        Added an exit button by pressing +
        Added a background static color to see if the game is running

    
*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>


#include <switch.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>


using namespace std;


const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;


/* GLOBALE VARIABELEN */
bool quit = false;



void handleEvents(PadState *pad) {
    u64 kDown = padGetButtonsDown(pad);

    if (kDown & HidNpadButton_Plus) {
        quit = true;
    }


    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            quit = true;
        }
    }

    // Haal de knoppen invoer van de controller op
    
    // u64 kHeld = padGetButtons(pad);


    
}


/*////////////////////////////////////////////////////////////////////
// Functie om de game loop te starten, main entrypoint!
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
int main(int argc, char* argv[]) {
    romfsInit();
    chdir("romfs:/");

    // Configureer de controller input
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);

    // Initializeer SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);


    SDL_Window *window = SDL_CreateWindow("Bomberman", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Player positie
    int playerX = 100;
    int playerY = 100;
    int frame = 0;
    int animationSpeed = 10; // lager = sneller
    int animCounter = 0;

    // Spritesheet info
    const int SPRITE_WIDTH = 65;
    const int SPRITE_HEIGHT = 110;
    int directionRow = 0; // 0 = achter, 1 = links/rechts, 2 = vooraanzicht

    // Laad de sprite sheet
    SDL_Texture* playerTexture = IMG_LoadTexture(renderer, "romfs:/Assets/Player/bomberman-sprite-sheet.png");

    
    /*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Code hieronder herhaalt zich gedurende de game, de loop stopt pas wanneer de game is beëindigd en is vaak ook een voorwaarde!
    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
    while (!quit) {
        // Verwerk controller input
        padUpdate(&pad);
        handleEvents(&pad);

        // Verwerk knoppen
        u64 kHeld = padGetButtons(&pad);

        bool moving = false;

        if (kHeld & HidNpadButton_Up) {
        playerY -= 2;
        directionRow = 0;
        moving = true;
        }
        if (kHeld & HidNpadButton_Down) {
            playerY += 2;
            directionRow = 2;
            moving = true;
        }
        if (kHeld & HidNpadButton_Left) {
            playerX -= 2;
            directionRow = 1;
            moving = true;
        }
        if (kHeld & HidNpadButton_Right) {
            playerX += 2;
            directionRow = 1;
            moving = true;
        }

        // Animatie update
        if (moving) {
            animCounter++;
            if (animCounter >= animationSpeed) {
                frame = (frame + 1) % 4; // 4 frames per animatie
                animCounter = 0;
            }
        } else {
            frame = 0;
        }

        // Maak het scherm leeg
        SDL_SetRenderDrawColor(renderer, 150, 50, 70, 255);
        SDL_RenderClear(renderer);

        SDL_Rect srcRect = {frame * SPRITE_WIDTH, directionRow * SPRITE_HEIGHT, SPRITE_WIDTH, SPRITE_HEIGHT};
        // SDL_Rect destRect = {playerX, playerY, SPRITE_WIDTH, SPRITE_HEIGHT}; // optioneel schalen
        SDL_Rect destRect;
        destRect.w = SPRITE_WIDTH;
        destRect.h = SPRITE_HEIGHT;
        destRect.x = playerX - SPRITE_WIDTH / 2;   // Horizontaal centreren
        destRect.y = playerY - SPRITE_HEIGHT;      // Voeten onderaan positioneren

        SDL_RenderCopy(renderer, playerTexture, &srcRect, &destRect);

        // Update het scherm
        SDL_RenderPresent(renderer);

    }
    IMG_Quit();
    SDL_DestroyTexture(playerTexture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    romfsExit();
    return 0;
}