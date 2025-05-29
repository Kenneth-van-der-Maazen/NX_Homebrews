//                                          18-02-2025
// Seal Hunter NX
// By Kenneth van der Maazen
//
// Seal Hunter NX is a port of the original game to the Nintendo Switch.
// Software was made for educational purposes.
// It's published under fair use.
// free for everyone to alter/copy or distribute.
// The original game was made in 2010 for PC 
// by Tomas Olander
// > http://www.sealhunter.com/
//
//======================================================================
// LOGS
//      V.0.0.2 -   Main menu has been added
//      V.0.0.1 -   Player movement and background added to game state. 
//
//======================================================================
//
//             y-as
//              |
//              |
//         -----+------ x-as
//              |
//              |
//
//=======================================================================
//
//     1280,0      ▼ 1280     1280,720
//           x---------------x
//           |               | 
//           |               | ◄ 720
//           |               |
//       0,0 x---------------x 0,720
//
//=======================================================================
//
//      {0, 500, 1280, 220}
//
//      0    -> Begint vanaf de linkerkant
//      500  -> begint vanaf de bovenkant
//      1280 -> eind van de linkerkant
//      220  -> eind van de bovenkant
//
//=======================================================================
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <switch.h>

using namespace std;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;


struct Position {
    int x, y;
};


////////////////// GLOBALE VARIABLES //////////////////
bool running = true;
bool inMenu = true;
bool quit = false;
bool inStartScreen = true;
bool isPaused = false;
bool isReloading = false;
bool inNewGameMenu = false;
bool twoPlayerMode = false;


Uint32 reloadStartTime = 0;
int bulletCount = 0;
int menuSelection = 0;

const char* mainMenuOptions[] = {"New Game", "Options", "Stats", "Help", "Quit"};
const int mainMenuOptionsCount = 5;
const char* newGameOptions[] = {"1 Player", "2 Players", "Back"};
const int newGameOptionsCount = 3;
const int menuOptionsCount = 5;

// Geluids effecten
Mix_Chunk* pistolShootSound = nullptr;
Mix_Chunk* pistolReloadSound = nullptr;
Mix_Music* backgroundMusic = nullptr;

// Fonts
TTF_Font* font = nullptr;
TTF_Font* menuText = nullptr;
TTF_Font* dosFont = NULL;

// Textures
SDL_Texture* backdropTexture = nullptr;
SDL_Texture* iceTexture = nullptr;
SDL_Texture* menuTexture = nullptr;
SDL_Texture* menuBackgroundTexture = nullptr;
SDL_Texture* menuTitleTexture = nullptr;

struct Player {
    int x, y;
    int speed;
    SDL_Texture* texture;
};

Player player1;
Player player2;

// Player 1 
// struct Player1 {
//     int speed;                      // Snelheid van speler
//     int currentFrame;               // Huidige frame van de speler
//     int currentWeaponIndex;         // Huidig wapen in gebruik
//     bool isMoving;                  // Is de speler aan het lopen? True/False
//     Position pos;                   // Positie van de speler
//     SDL_Texture* idleTexture;       // Afbleeding voor stilstaande speler
//     SDL_Texture* walkTextures[2];   // Afbeelding frames voor lopen
//     SDL_Texture* crosshairTexture;  // Blauwe crosshair
//     Uint32 lastFrameTime;           // Tijd van de laatste frame
//     Uint32 lastShotTime;            // Tijd van de laatste schot
// };


//Player1 player;


// Laad textures efficienter functie
SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* filepath) {
    SDL_Surface* surface = IMG_Load(filepath);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

////////////////// MAIN MENU FUNCTIES //////////////////
void renderMenu(SDL_Renderer* renderer) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderClear(renderer);

    if (menuBackgroundTexture) {
        SDL_Rect titleRect = {SCREEN_WIDTH - 700, 50, 680, 100};
        SDL_RenderCopy(renderer, menuTitleTexture, NULL, &titleRect);
        SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderCopy(renderer, menuBackgroundTexture, NULL, &bgRect);
    }

    SDL_Color color = {255, 255, 255, 255};
    SDL_Color selectedColor = {255, 0, 0, 255};

    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 200);
    SDL_Rect menuBox = {80, 220, 300, 300};
    SDL_RenderFillRect(renderer, &menuBox);

    const char** options = inNewGameMenu ? newGameOptions : mainMenuOptions;
    int optionsCount = inNewGameMenu ? newGameOptionsCount : mainMenuOptionsCount;

    for (int i = 0; i < optionsCount; i++) {
        SDL_Surface* textSurface = TTF_RenderText_Solid(font, options[i], (i == menuSelection) ? selectedColor : color);
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        SDL_Rect textRect = {100, 250 + (i * 50), textSurface->w, textSurface->h};
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_FreeSurface(textSurface);
        SDL_DestroyTexture(textTexture);
    }
    SDL_RenderPresent(renderer);
}

void handleMenuInput(PadState* pad) {
    u64 kDown = padGetButtonsDown(pad);
    if (inNewGameMenu) {
        if (kDown & HidNpadButton_AnyDown) menuSelection = (menuSelection + 1) % newGameOptionsCount;
        if (kDown & HidNpadButton_AnyUp) menuSelection = (menuSelection - 1 + newGameOptionsCount) % newGameOptionsCount;
        if (kDown & HidNpadButton_A) {
            if (menuSelection == 2) { inNewGameMenu = false; menuSelection = 0; }
            else { twoPlayerMode = (menuSelection == 1); inMenu = false; }
        }
    } else {
        if (kDown & HidNpadButton_AnyDown) menuSelection = (menuSelection + 1) % mainMenuOptionsCount;
        if (kDown & HidNpadButton_AnyUp) menuSelection = (menuSelection - 1 + mainMenuOptionsCount) % mainMenuOptionsCount;
        if (kDown & HidNpadButton_A) {
            if (menuSelection == 0) { inNewGameMenu = true; menuSelection = 0; }
            else if (menuSelection == 4) { running = false; }
        }
    }
}


////////////////// PLAYER FUNCTIES //////////////////

// Laad Player1-afbeeldingen
// bool loadPlayerTextures(SDL_Renderer* renderer) {
//     player.idleTexture = loadTexture(renderer, "romfs:/images/Player1/Stopped/anim.png");
//     player.walkTextures[0] = loadTexture(renderer, "romfs:/images/Player1/Walking/anim_1.png");
//     player.walkTextures[1] = loadTexture(renderer, "romfs:/images/Player1/Walking/anim_2.png");
//     player.crosshairTexture = loadTexture(renderer, "romfs:/images/Misc/blue_crosshair_1.png");

//     if (!player.idleTexture || !player.walkTextures[0] || !player.walkTextures[1]) {
//         return false;
//     }

//     // Startpositie van player
//     player.pos.x = 1100;                // Rechts van het scherm.
//     player.pos.y = 500;                 // Onderaan het scherm.
//     player.speed = 2;
//     player.currentFrame = 0;
//     player.lastFrameTime = SDL_GetTicks();
//     player.isMoving = false;
//     player.lastShotTime = 0;
    
//     return true;
// }

void initPlayers(SDL_Renderer* renderer) {
    player1.x = 300;
    player1.y = 500;
    player1.speed = 5;
    player1.texture = loadTexture(renderer, "romfs:/images/Player1/anim.png");

    player2.x = 900;
    player2.y = 500;
    player2.speed = 5;
    player2.texture = loadTexture(renderer, "romfs:/images/Player2/anim.png");
}

// Functie om de speler te renderern
void renderPlayer(SDL_Renderer* renderer) {
    // SDL_Texture* currentTexture = player.isMoving ? player.walkTextures[player.currentFrame] : player.idleTexture;

    // if (currentTexture == nullptr) {
    //     printf("Error: Speler texture is null!\n");
    //     return;
    // }

    // Player tekenen op het scherm
    SDL_Rect destRect = {player1.x, player1.y, 25, 39};  // Speler heeft grootte 36 x 50px
    SDL_RenderCopy(renderer, player1.texture, NULL, &destRect);

    SDL_Rect destRect = {player2.x, player2.y, 25, 39};  // Speler heeft grootte 36 x 50px
    SDL_RenderCopy(renderer, player2.texture, NULL, &destRect);

    // Render het huidige wapen
    //Weapon currentWeapon = player.weapons[player.currentWeaponIndex];
    // SDL_Rect weaponRect = {
    //     player.pos.x + currentWeapon.offsetX, 
    //     player.pos.y + currentWeapon.offsetY, 
    //     currentWeapon.width,
    //     currentWeapon.height
    // };
    // SDL_RenderCopy(renderer, currentWeapon.texture, NULL, &weaponRect);

    // Crosshair tekenen aan de linkerkant
    SDL_Rect crosshairRect = {player1.x - 70, player1.y + 17, 15, 15};    // 70px naar links en 15px naar beneden
    //SDL_RenderCopy(renderer, player1.crosshairTexture, NULL, &crosshairRect);
}

// Functie om speler te kunnen verplaatsen op het scherm
void handlePlayerMovement(PadState* pad) {
    u64 kHeld = padGetButtons(pad);
    //player1.isMoving = false;

    if (kHeld & HidNpadButton_AnyLeft) {
        player1.x -= player1.speed;
        //player1.isMoving = true;
    }
    if (kHeld & HidNpadButton_AnyRight) {
        player1.x += player1.speed;
        //player1.isMoving = true;
    }
    if (kHeld & HidNpadButton_AnyUp) {
        player1.y -= player1.speed;
        //player1.isMoving = true;
    }
    if (kHeld & HidNpadButton_AnyDown) {
        player1.y += player1.speed;
        //player1.isMoving = true;
    }

    // Houd de speler binnen het scherm
    if (player1.x < 0) player1.x = 0;
    if (player1.x > 1280 - 36) player1.x = 1280 - 36;
    if (player1.y < 0) player1.y = 0;
    if (player1.y > 720 - 64) player1.y = 720 - 64;

    // Wissel animatieframe als speler beweegt
    if (player1.isMoving) {
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime > player1.lastFrameTime + 35) {
            player.currentFrame = (player.currentFrame + 1) % 2;
            player.lastFrameTime = currentTime;
        }
    }
}


// Functie om events te verwerken met controller
void handleEvents(PadState* pad) {
    u64 kDown = padGetButtonsDown(pad);

    if (kDown & HidNpadButton_Plus) {
        running = false;
    }
    if (kDown & HidNpadButton_A) {
        //shootWeapon();
        //fireBullet();
    }
}

///////////////  MAIN APPLET ENTRYPOINT  ///////////////
int main(int argc, char* argv[]) 
{
    romfsInit();
    chdir("romfs:/");

    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);

    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("Seal Hunter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    font = TTF_OpenFont("romfs:/fonts/dosVGA.ttf", 24);
    menuTitleTexture = loadTexture(renderer, "romfs:/images/Misc/shtitle.png");
    menuBackgroundTexture = loadTexture(renderer, "romfs:/images/Misc/titlescreen.png");
    
    backdropTexture = IMG_LoadTexture(renderer, "romfs:/images/Misc/backdrop.png");
    iceTexture = IMG_LoadTexture(renderer, "romfs:/images/Misc/ice.png");
    initPlayers(renderer);

    //loadPlayerTextures(renderer);

    const int frameDelay = 1000 / 60;

    ///////////////  MAIN GAMEPLAY LOOP  ///////////////
    while (appletMainLoop() && running) 
    {
        Uint32 frameStart = SDL_GetTicks();

        padUpdate(&pad);
        handleEvents(&pad);
        handlePlayerMovement(&pad);

        SDL_SetRenderDrawColor(renderer, 155, 155, 155, 255);
        SDL_RenderClear(renderer);

        if (inMenu) {
            handleMenuInput(&pad);
            renderMenu(renderer);
        } else {

            if (twoPlayerMode) {
                padConfigureInput(1, HidNpadStyleSet_NpadStandard);
                padConfigureInput(2, HidNpadStyleSet_NpadStandard);
            }
            
            if (backdropTexture) {
                SDL_Rect destRect = { 0, 0, 1280, 720 };
                SDL_RenderCopy(renderer, backdropTexture, NULL, &destRect);
            }

            if (iceTexture) {
                SDL_Rect iceRect = {0, 80, 1280, 650};  // Adjust position & size
                SDL_RenderCopy(renderer, iceTexture, NULL, &iceRect);
            }
            SDL_RenderClear(renderer);
            renderPlayer(renderer);
            if (twoPlayerMode) renderPlayer(renderer, player2);
            SDL_RenderPresent(renderer);
        }

        
    

        Uint32 frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
    // if (player.idleTexture) SDL_DestroyTexture(player.idleTexture);             // <- controle toegevoegd om crashes te voorkomen bij vrijmaken geheugen
    // if (player.walkTextures[0]) SDL_DestroyTexture(player.walkTextures[0]);
    // if (player.walkTextures[1]) SDL_DestroyTexture(player.walkTextures[1]);
    // if (player.crosshairTexture) SDL_DestroyTexture(player.crosshairTexture);
    // if (backdropTexture) {SDL_DestroyTexture(backdropTexture);}
    // if (iceTexture) {SDL_DestroyTexture(iceTexture);}

    SDL_DestroyTexture(menuBackgroundTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);

    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
    romfsExit();
    
    return 0;
}