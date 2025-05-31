/*  Nintendo Switch homebrew projects 
    by Kenneth van der Maazen (c) 2024

    Seal Hunter is a free 2D shoot 'em up game, this version is written and programmed by me from scratch with the only goal for it to be playable on the Nintendo Switch.
    This game might trigger some people due to the content of seal hunting, but I want to make it clear that this game is not meant to promote or glorify seal hunting in any way.
    The game is purely fictional and for entertainment and educational purposes only, feel free to download, use, alter and/or modify my source code as you see fit.

    The concept of this game is to prevent the seals and other creatures from overpopulating the ocean and eating all the fish from it!
    Using a wide range of weapons, players must prevent harmful sea creatures, ranging from seals to polar bears, from reaching back to the ocean to ensure a good catch.
    The base game is divided into 5 levels, each with its own unique challenges and obstacles. 
    later on I will be adding more functionalities like splitscreen Co-op and/or online Multiplayer, game modes such as endless survival mode or levels with different stages.


    Date created:   07-11-2024
    Last updated:   08-11-2024 @ 14:29

    Build:  0.0.1
        Character static png displays on screen.
        Background set to black.
        Character animation and movement implemented.

    Build:  0.0.2
        Character now holds starting pistol.
        Crosshair added with offset: 130 pixels to left from character.

    Build:  0.0.3
        Include vector library.
        Added backdrop png of an iceberg.
        Added background png of the ice layer's reflection.
        Changed some values where the character holds the gun wrong on the y-axis, crosshair adjusted and backgrounds playable area is now set with borders.

    Build:  0.0.4
        Include SDL_ttf library for displaying texts.
        Added HUD image on top of the screen.
        Repositioned player sprite to the right side of the screen, in the middle of the ice vertically.

    Build:  0.0.5
        Starting pistol now able to fire 12 bullets.
        Added a sound effect when pistol is fired.
        Added sound effect when pistol is reloading.
        Muzzle flash when pistol is fired is randomly selected from 2 different .png images.
        Bullet counter added to crosshair.
        Manual reloading is now implemented but not fully functional. *Issue: when all bullets are depleted, the pistol can't reload!

    Build: 0.0.6
        Reloading the pistol is now fixed, manual and automatic are both working!
        Reloading now shows any% on top of the crosshair for the full duration of a reload.

    Build: 0.0.7
        Added new start menu screen.
        Added a paused game screen.

    Build: 0.0.8
        Added a background image to the start menu screen.
        Background music added, - need to implement some settings for disabling/enabling music tracks.
        Pause menu added options to resume game, quit game, or go to settings.
        Changed the name in Makefile from Seal Hunter NX -> Seal Hunters

    Build: 0.0.9
        Start menu now correctly sized and displayed withing the borders of the screen.
        Option menu was succesfull created with the option to mute the background song.
        Options menu button is now fixed within the start menu where it now only responds to the (A) button like the rest.
        Selecting the quit button from the pause menu now asks if you want to quit to the main menu or exit the game.

    Build: 0.1.0 🎉 PRE-ALPHA Build
        Created HUD elements.
        HUD now contains weapon images and pricing info, - still not buyable and there are some cropping isseus.
        Created a transparent dark overlay for the menu items and titlescreen.

    Build: 0.1.1
        Added empty shell ejections from gun when fired, - still needs more tweeking.

*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include <switch.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

using namespace std;

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const int PLAYER_SPEED = 4;     // Snelheid van de speler
const int ANIMATION_DELAY = 10; // Aantal frames tussen animatiewisselingen

const int maxBullets_Pistol = 12;
const int reloadTime_Pistol = 2000; // Reload tijd in milliseconden (2 seconden)

/* GLOBALE VARIABELEN */
bool quit = false;
bool inStartScreen = true;
bool isPaused = false;

bool pistol = true;
bool showMuzzleFlash = false;
bool isReloading = false;
bool musicEnabled = true;
bool confirmingQuit = false;

Uint32 reloadStartTime = 0;     // Start tijd voor het herladen

int score = 0;
int muzzleFlashTimer = 0;
int bulletCount = 0;        // Aantal schoten afgevuurd


Mix_Chunk *pistolShootSound = nullptr;
Mix_Chunk *pistolReloadSound = nullptr;

Mix_Music* backgroundMusic = nullptr;

TTF_Font* font = NULL;  // Font voor tekstweergave
TTF_Font* dosTitleFont = NULL;
TTF_Font* dosFont = NULL;

SDL_Texture* muzzleFlashTextures[2];    // Array voor muzzle flash texture (Small)

// Menu opties
enum MenuOption { NEW_GAME, OPTIONS, STATS, HELP, QUIT };
enum OptionsMenuOption { TOGGLE_MUSIC, BACK_TO_MAIN };
enum PauseOption { CONTINUE, SETTINGS, QUIT_TO_MAIN };
enum QuitConfirmOption { MAIN_MENU, EXIT_GAME };

MenuOption selectedOption = NEW_GAME;
OptionsMenuOption selectedOptionsMenuOption = TOGGLE_MUSIC;
PauseOption selectedPauseOption = CONTINUE;
QuitConfirmOption quitConfirmOption = MAIN_MENU;

// Speler
struct Player {
    int x, y;               // Positie van de speler
    int width, height;      // Grootte van de speler
    bool moving;            // Beweegt de speler op dit moment?
    int frame;              // Huidige frame van de animatie
    int frameCounter;       // Teller voor animatiewisseling
    SDL_Texture* idleTexture;       // Statische sprite van de speler
    SDL_Texture* walkTexture1;      // Eerste frame van de loopanimatie
    SDL_Texture* walkTexture2;      // Tweede frame van de loopanimatie
    SDL_Texture* weaponTexture;     // Wapen van de speler
    SDL_Texture* crosshairTexture;  // Crosshair icoon links van de speler
};

struct Enemy {
    int x, y;
    int width, height;
    int speed;
    bool active;
    SDL_Texture* texture;
};

struct Bullet {
    int x, y;
    int speed;
    bool active;
};
vector<Bullet> bullets(maxBullets_Pistol);

struct Shell {
    int x, y;
    float speedX, speedY;
    int bouncesLeft;
    bool active;
    SDL_Texture* texture;
};
vector<Shell> shells;
SDL_Texture* shellTextures[4];  // Array voor shell textures

Player player;
Enemy brown_seal;

SDL_Texture* backdropTexture = NULL;    // Achtergrond afbeelding
SDL_Texture* iceTexture = NULL;         // Ijslaag afbeelding
SDL_Texture* hudTexture = NULL;         // HUD afbeelding
SDL_Texture* startMenuBackgroundTexture = NULL; // Texture voor de achtergrond van het start menu

// Functie om tekst te renderen
void renderText(SDL_Renderer* renderer, const char* text, TTF_Font *font, int x, int y, SDL_Color color = {255, 255, 255}) {
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect messageRect;
    messageRect.x = x;
    messageRect.y = y;
    messageRect.w = surfaceMessage->w;
    messageRect.h = surfaceMessage->h;

    // Render de teskt op het scherm
    SDL_RenderCopy(renderer, message, NULL, &messageRect);

    // Opruimen
    SDL_DestroyTexture(message);
    SDL_FreeSurface(surfaceMessage);
}

// Functie om de speler te initialiseren
void initPlayer(SDL_Renderer* renderer) {
    player.width = 24;          // Stel de breedte van de speler in
    player.height = 40;         // Stel de hoogte van de speler in
    player.x = SCREEN_WIDTH - player.width - 80;    // Plaats de speler aan de rechterkant, 20 pixels van de rand.
    player.y = (SCREEN_HEIGHT / 2) + 80;            // Plaats de speler verticaal in het midden van het ijs.
    
    player.moving = false;      // De speler beweegt niet op het moment van initialisatie
    player.frame = 0;           // Start met de eerste frame van de animatie
    player.frameCounter = 0;    // Reset de framecounter

    // Laad de statische sprite
    SDL_Surface* tempSurface = IMG_Load("romfs:/Assets/Player1/Stopped/anim.png");
    player.idleTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    // Laad het eerste loopframe
    tempSurface = IMG_Load("romfs:/Assets/Player1/Walking/anim_1.png");
    player.walkTexture1 = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    // Laad het tweede loopframe
    tempSurface = IMG_Load("romfs:/Assets/Player1/Walking/anim_2.png");
    player.walkTexture2 = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    // Laad het pistol wapen    - if (pistol = true) dan img load -> else statement van maken
    tempSurface = IMG_Load("romfs:/Assets/Weapons/Hands/Blue/USP/usp.png");
    if (!tempSurface) {
        cerr << "Failed to load weapon texture: " << IMG_GetError() << endl;
        quit = true;
        return;
    }
    player.weaponTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    // Laad de crosshair-afbeelding
    tempSurface = IMG_Load("romfs:/Assets/Misc/blue_crosshair_1.png");
    player.crosshairTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
}

void initBrownSeal(SDL_Renderer* renderer) {
    brown_seal.x = -1290;  // Plaats de brown seal aan de linkerkant van het scherm
    brown_seal.y = SCREEN_HEIGHT - 150;      // Plaats de brown seal aan de onderkant van het scherm
    brown_seal.width = 50;  // Stel de breedte van de brown seal
    brown_seal.height = 50; // Stel de hoogte van de brown seal
    brown_seal.speed = 2;
    brown_seal.active = true;

    SDL_Surface* tempSurface = IMG_Load("romfs:/Assets/Enemies/brown_seal.png");

    brown_seal.texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
}

void renderBrownSeal(SDL_Renderer* renderer) {
    if (brown_seal.active) {
        SDL_Rect enemyRect = { brown_seal.x, brown_seal.y, brown_seal.width, brown_seal.height };
        SDL_RenderCopy(renderer, brown_seal.texture, NULL, &enemyRect);
    }
}

void updateBrownSeal() {
    if (brown_seal.active) {
        brown_seal.x += brown_seal.speed; // Beweegt naar rechts

        if (brown_seal.x + brown_seal.width < 0) {
            brown_seal.x = SCREEN_WIDTH; // Reset naar rechts
        }
    }
}

// Functie om muzzle flash textures te initialiseren
void initMuzzleFlash(SDL_Renderer* renderer) {
    SDL_Surface* tempSurface1 = IMG_Load("romfs:/Assets/Muzzle/Small/anim_1.png");
    SDL_Surface* tempSurface2 = IMG_Load("romfs:/Assets/Muzzle/Small/anim_2.png");
    muzzleFlashTextures[0] = SDL_CreateTextureFromSurface(renderer, tempSurface1);
    muzzleFlashTextures[1] = SDL_CreateTextureFromSurface(renderer, tempSurface2);
    SDL_FreeSurface(tempSurface1);
    SDL_FreeSurface(tempSurface2);
}

void renderMuzzleFlash(SDL_Renderer* renderer) {
    if (showMuzzleFlash && muzzleFlashTimer > 0) {
        int randomIndex = rand() % 2;   // random keuze tussen 0 en 1
        SDL_Rect muzzleRect = { player.x - 40, player.y + 9, 35, 20 };
        SDL_RenderCopy(renderer, muzzleFlashTextures[randomIndex], NULL, &muzzleRect);
        muzzleFlashTimer--;
        if (muzzleFlashTimer <= 0) {
            showMuzzleFlash = false;
        }
    }
}


void initShellTextures(SDL_Renderer* renderer) {
    const char* shellPaths[4] = {
        "romfs:/Assets/Misc/SmallShell/anim_1.png",
        "romfs:/Assets/Misc/SmallShell/anim_2.png",
        "romfs:/Assets/Misc/SmallShell/anim_3.png",
        "romfs:/Assets/Misc/SmallShell/anim_4.png"
    };

    for (int i = 0; i < 4; i++) {
        SDL_Surface* tempSurface = IMG_Load(shellPaths[i]);
        if (!tempSurface) {
            cerr << "Failed to load shell texture: " << i + 1 << ": " << IMG_GetError() << endl;
            quit = true;
            return;
        }
        shellTextures[i] = SDL_CreateTextureFromSurface(renderer, tempSurface);
        SDL_FreeSurface(tempSurface);
    }
}

// Functie om de achtergrond en ijslaag te initialiseren
void initBackdropAndIce(SDL_Renderer* renderer) {
    SDL_Surface* tempSurface = IMG_Load("romfs:/Assets/Misc/backdrop.png"); // Laad de achtergrond
    backdropTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    tempSurface = IMG_Load("romfs:/Assets/Misc/ice.png");   // Laad de ijslaag
    iceTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
}


// Functie om de HUD-afbeelding te laden
void initHUD(SDL_Renderer* renderer) {
    SDL_Surface* tempSurface = IMG_Load("romfs:/Assets/Misc/blueHUD.png");
    if (!tempSurface) {
        cerr << "ERROR_ " << IMG_GetError() << endl;
        quit = true;
        return;
    }
    hudTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    
}

SDL_Texture* weaponTextures[10];    // Array voor 10 wapen afbeeldingen
int weaponPoints[10] = { 500, 1000, 1500, 2500, 3500, 5000, 6000, 7500, 10000, 12500 };

void initWeapons(SDL_Renderer* renderer) {
    // Laad de wapenafbeeldingen
    for (int i = 0; i < 10; i++) {
        string path = "romfs:/Assets/Textures/UI/" + to_string(i+1) + ".png";
        SDL_Surface* tempSurface = IMG_Load(path.c_str());
        if (!tempSurface) {
            cerr << "ERROR: Failed to load weapon texture " << i+1 << ": " << IMG_GetError() << endl;
            quit = true;
            return;
        }
        weaponTextures[i] = SDL_CreateTextureFromSurface(renderer, tempSurface);
        SDL_FreeSurface(tempSurface);
    }
}

void renderHUD(SDL_Renderer* renderer) {
    SDL_Rect hudRect = { 0, 0, SCREEN_WIDTH, 122 };  // Plaats bovenaan met de hoogte van de HUD afbeelding
    SDL_RenderCopy(renderer, hudTexture, NULL, &hudRect);

    int iconSize = 55;      // Grootte van elke wapenafbeelding
    int spacing = 52;       // Ruimte tussen de afbeeldingen
    int startX = 34;        // Begin x-positie
    int yPosition = 39;     // Y-positie voor de wapenafbeeldingen in de HUD

    // Render de wapenafbeeldingen en de kosten per wapen
    for (int i = 0; i < 10; i++) {
        // Render de wapenafbeelding
        SDL_Rect iconRect;  // Declareer iconRect binnen de loop, maar buiten de initialisatie van de waarde, geen idee waarom..
        iconRect.x = startX + i * (iconSize + spacing);
        iconRect.y = yPosition;
        iconRect.w = iconSize;
        iconRect.h = iconSize;

        // Render de wapenafbeeldingen
        SDL_RenderCopy(renderer, weaponTextures[i], NULL, &iconRect);

        // Render de puntenwaarde onder de afbeelding
        string pointsText = to_string(weaponPoints[i]) + "$";
        renderText(renderer, pointsText.c_str(), font, iconRect.x, iconRect.y + iconSize + 5);
    }
}


// Functie om de achtergrond en ijslaag te renderen
void renderBackdropAndIce(SDL_Renderer* renderer) {
    // Render de achtergrond
    SDL_Rect backdropRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderCopy(renderer, backdropTexture, NULL, &backdropRect);

    // Render de ijslaag bovenop de achtergrond
    SDL_Rect iceRect = { 0, SCREEN_HEIGHT - 590, SCREEN_WIDTH, SCREEN_HEIGHT - 100 };   // Plaats de ijslaag onderaan het scherm
    SDL_RenderCopy(renderer, iceTexture, NULL, &iceRect);
}

// Functie om de speler te renderen
void renderPlayer(SDL_Renderer* renderer) {
    SDL_Rect destRect = { player.x, player.y, player.width, player.height };
    SDL_Texture* currentTexture = player.idleTexture;

    if (player.moving) {
        // Wissel tussen de twee animatieframes
        if (player.frame == 0) {
            currentTexture = player.walkTexture1;
        } else {
            currentTexture = player.walkTexture2;
        }
    }

    // Render de speler
    SDL_RenderCopy(renderer, currentTexture, NULL, &destRect);

    // Render het wapen links van de speler
    SDL_Rect weaponRect = { player.x - 10, player.y + 17, 34, 10 }; // Pas de offset en grootte aan van het wapen
    SDL_RenderCopy(renderer, player.weaponTexture, NULL, &weaponRect);

    // Render de crosshair links van de speler
    SDL_Rect crosshairRect = { player.x - 100, player.y + 15, 16, 16 }; // Pas de offset en grootte aan van de crosshair
    SDL_RenderCopy(renderer, player.crosshairTexture, NULL, &crosshairRect);
}

void startReloading() {
    isReloading = true;
    reloadStartTime = SDL_GetTicks();
    Mix_PlayChannel(-1, pistolReloadSound, 0);
}

void shootBullet() {
    if (isReloading || bulletCount >= maxBullets_Pistol) {
        if (!isReloading && bulletCount >= maxBullets_Pistol) {
            startReloading();   // Automatisch herladen wanneer kogels op zijn
        }
        return;
    }

    for (int i = 0; i < maxBullets_Pistol; ++i) {
        if (!bullets[i].active) {
            bullets[i].active = true;
            bullets[i].x = player.x - player.width;
            bullets[i].y = player.y + player.height / 2;
            bullets[i].speed = 200;

            Mix_PlayChannel(-1, pistolShootSound, 0);
            showMuzzleFlash = true;
            muzzleFlashTimer = 4;   // Display muzzle flash voor aantal frames

            bulletCount++;

            Shell shell;
            shell.x = player.x + 10;  // Startpositie naast de speler
            shell.y = player.y + player.height / 2;
            shell.speedX = (rand() % 5 + 2) * (rand() % 2 == 0 ? 1 : 1);    // Willekeurige horizontale snelheid
            shell.speedY = -(rand() % 5 + 3);   // Willekeurige verticale snelheid
            shell.bouncesLeft = 1;  // Aantal keer dat shell kan stuiteren
            shell.active = true;
            shell.texture = shellTextures[rand() % 4];  // Random texture keuze uit 4 opties
            shells.push_back(shell); // Voeg de shell toe aan de lijst

            break;
        }
    }
}



void updateReload() {
    if (isReloading && SDL_GetTicks() - reloadStartTime >= reloadTime_Pistol) {
        isReloading = false;
        bulletCount = 0;    // Reset bullet count na het herladen
    } else if (!isReloading && bulletCount >= maxBullets_Pistol) {
        startReloading();
    }
}

void updateBullets() {
    for (int i = 0; i < maxBullets_Pistol; ++i) {
        if (bullets[i].active) {
            bullets[i].x -= bullets[i].speed;   // Laat de bullet horizontaal bewegen
            if (bullets[i].x < 0) {
                bullets[i].active = false;
            }
        }
    }
}

void renderBullets(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Kleur voor kogels (wit)
    for (int i = 0; i < maxBullets_Pistol; ++i) {
        if (bullets[i].active) {
            SDL_Rect bulletRect = { bullets[i].x, bullets[i].y, 120, 2}; // bullet grootte
            SDL_RenderFillRect(renderer, &bulletRect);  // Teken de bullet in de window
        }
    }
}

void renderShells(SDL_Renderer* renderer) {
    for (const auto& shell : shells) {
        if (!shell.active) continue;

        SDL_Rect shellRect = { shell.x, shell.y, 3, 2 };  // Pas grootte van shells aan
        SDL_RenderCopy(renderer, shell.texture, NULL, &shellRect);
    }
}

void updateShells() {
    for (auto& shell : shells) {
        if (!shell.active) continue;

        shell.x += shell.speedX;
        shell.y += shell.speedY;

        
        shell.speedY += 0.5f;   // Zwaartekracht effect

        // Detecteer stuiteren op de grond
        if (shell.y >= SCREEN_HEIGHT - 90) {    // Onderkant van het scherm
            shell.y = SCREEN_HEIGHT - 90;   // Zet de shell op de grond
            if (shell.bouncesLeft > 0) {
                shell.speedY = -shell.speedY * 0.5f;   // Verander de verticale snelheid
                shell.speedX *= 0.7f;   // Verminder horizontale snelheid
                shell.bouncesLeft--;
            } else {
                //shell.active = false;   // Deactiveer shell als de bounces op zijn
                shell.speedX = 0;
                shell.speedY = 0;
            }
        }
    }
}

void renderBulletCount(SDL_Renderer* renderer) {
    if (isReloading) {
        // Bereken herlaadpercentage op basis van de verstreken tijd
        Uint32 elapsedTime = SDL_GetTicks() - reloadStartTime;
        int reloadPercentage = (elapsedTime * 100) / reloadTime_Pistol;

        // Beperk de waarde tussen 0 en 100%
        if (reloadPercentage > 100) reloadPercentage = 100;

        // Convert percentage naar tekst
        string reloadText = to_string(reloadPercentage) + "%";
        SDL_Color color = { 255, 0, 0 }; // Rood voor herlaadpercentage

        // Maak de surface en texture voor het percentage
        SDL_Surface* surface = TTF_RenderText_Solid(font, reloadText.c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        // Plaats de tekst boven de crosshair
        SDL_Rect textRect = { player.x - 100, player.y - 5, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, NULL, &textRect);

        // Opruimen
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    } else {
        // Toon normale bullet count als er niet wordt herladen
        string bulletText = to_string(maxBullets_Pistol - bulletCount);

        // Kies de juiste kleur op basis van het aantal resterende kogels
        SDL_Color color;
        if (maxBullets_Pistol - bulletCount <= 1) {
            color = { 255, 165, 0 }; // Oranje als er 3 of minder kogels over zijn
        } else {
            color = { 0, 0, 0 };    // Zwart
        }

        SDL_Surface* surface = TTF_RenderText_Solid(font, bulletText.c_str(), color);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_Rect textRect = { player.x - 100, player.y -5, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, NULL, &textRect);

        SDL_DestroyTexture(texture);
        SDL_FreeSurface(surface);
    }
}


// Functie om de speler te verplaatsen op basis van invoer
void handlePlayerMovement(PadState* pad) {
    u64 kHeld = padGetButtons(pad);
    player.moving = false;

    if (kHeld & HidNpadButton_Up) {
        player.y -= PLAYER_SPEED;
        player.moving = true;
    }
    if (kHeld & HidNpadButton_Down) {
        player.y += PLAYER_SPEED;
        player.moving = true;
    }
    if (kHeld & HidNpadButton_Left) {
        player.x -= PLAYER_SPEED;
        player.moving = true;
    }
    if (kHeld & HidNpadButton_Right) {
        player.x += PLAYER_SPEED;
        player.moving = true;
    }

    // Zorg ervoor dat de speler binnen het scherm blijft
    if (player.x < 0) player.x = 0;
    if (player.y < SCREEN_HEIGHT - 430) player.y = SCREEN_HEIGHT - 430; // Blijf op de ijslaag
    if (player.x > SCREEN_WIDTH - player.width) player.x = SCREEN_WIDTH - player.width;
    if (player.y > SCREEN_HEIGHT - 90) player.y = SCREEN_HEIGHT - 90;

    // Update de animatie alleen als de speler beweegt
    if (player.moving) {
        player.frameCounter++;
        if (player.frameCounter >= ANIMATION_DELAY) {
            player.frame = (player.frame + 1) % 2;  // Wissel tussen 0 en 1
            player.frameCounter = 0;
        }
    } else {
        player.frame = 0; // Reset naar de eerste frame
    }
}


// Functie om het startmenu weer te geven
void renderStartMenu(SDL_Renderer* renderer) {
    // Render de achtergrond afbeelding
    if (startMenuBackgroundTexture) {
        SDL_Rect bgRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
        SDL_RenderCopy(renderer, startMenuBackgroundTexture, NULL, &bgRect);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128); // Zwart met alpha = 128 (50% transparantie)
    SDL_Rect titleOverlayRect = { SCREEN_WIDTH - 650, 10, 600, 80 };
    SDL_Rect menuOverlayRect = { 10, SCREEN_HEIGHT - 400, 300, 400 };
    SDL_RenderFillRect(renderer, &menuOverlayRect);
    SDL_RenderFillRect(renderer, &titleOverlayRect);

    SDL_Color white = { 255, 255, 255 };
    SDL_Color yellow = { 255, 255, 0 };
    SDL_Color black = { 0, 0, 0 };

    const char* menuOptions[] = { "New Game", "Options", "Stats", "Help", "Quit" };

    int startX = 50;
    int startY = SCREEN_HEIGHT - 380; // Begin positie van het menu in de overlay

    // Stel een grotere font grootte in voor het menu
    TTF_Font* largeFont = TTF_OpenFont("romfs:/fonts/Perfect DOS VGA 437.ttf", 64); // Grotere lettergrootte!

    // Titel renderen
    const char* titleText = "Seal Hunters";

    renderText(renderer, titleText, largeFont, SCREEN_WIDTH - 500, 20, white);  // -300 = The Se..


    // Render de menu opties onder elkaar
    for (int i = 0; i < 5; i++) {
        SDL_Color color = (i == selectedOption) ? yellow : white;   // Gele kleur voor de geselecteerde optie, anders zwart
        renderText(renderer, menuOptions[i], dosTitleFont, startX, startY + i * 50, color);
    }

    // Tekst met naam en datum onderaan het hoofdmenu
    const char* authorText = "Kenneth van der Maazen (C) 2024.";
    renderText(renderer, authorText, dosFont, SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT - 30, black);

    // Opruimen van de code
    TTF_CloseFont(largeFont);
}

// Functie voor het pauze menu
void renderPauseMenu(SDL_Renderer* renderer) {
    const char* pauseOptions[] = { "Continue", "Settings", "Quit" };
    SDL_Color white = { 255, 255, 255 };
    SDL_Color yellow = { 255, 255, 0 };

    // Titel van het pauzemenu
    renderText(renderer, "- GAME PAUSED -", dosTitleFont, SCREEN_WIDTH / 2 - 100, 100);

    // Opties van het pauzemenu
    for (int i = 0; i < 3; i++) {
        SDL_Color color = (i == selectedPauseOption) ? yellow : white;
        renderText(renderer, pauseOptions[i], dosFont, SCREEN_WIDTH / 2 - 50, 200 + i * 40, color);
    }
}

void handlePauseMenuInput(PadState* pad) {
    u64 kDown = padGetButtonsDown(pad);

    if (confirmingQuit) {
        // Bevestigingsscherm-bediening
        if (kDown & HidNpadButton_Up || kDown & HidNpadButton_Down) {
            quitConfirmOption = (quitConfirmOption == MAIN_MENU) ? EXIT_GAME : MAIN_MENU;
        } else if (kDown & HidNpadButton_A) {
            if (quitConfirmOption == MAIN_MENU) {
                inStartScreen = true;  // Keer terug naar het hoofdmenu
                isPaused = false;
                confirmingQuit = false;
            } else if (quitConfirmOption == EXIT_GAME) {
                quit = true;  // Sluit het spel volledig af
            }
        } else if (kDown & HidNpadButton_B) {
            confirmingQuit = false;  // Annuleer het bevestigingsscherm
        }
    } else {
        // Normaal pauzemenu-bediening
        if (kDown & HidNpadButton_Up) {
            selectedPauseOption = (selectedPauseOption == CONTINUE) ? QUIT_TO_MAIN : static_cast<PauseOption>(selectedPauseOption - 1);
        } else if (kDown & HidNpadButton_Down) {
            selectedPauseOption = (selectedPauseOption == QUIT_TO_MAIN) ? CONTINUE : static_cast<PauseOption>(selectedPauseOption + 1);
        } else if (kDown & HidNpadButton_A) {
            switch (selectedPauseOption) {
                case CONTINUE:
                    isPaused = false;
                    break;
                case SETTINGS:
                    // Open de instellingen
                    break;
                case QUIT_TO_MAIN:
                    confirmingQuit = true;  // Start het bevestigingsscherm
                    break;
            }
        }
    }
}

// Functie om het menu te navigeren met Nintendo Switch controls
void handleStartMenuInput(PadState* pad) {
    u64 kDown = padGetButtonsDown(pad);

    if (kDown & HidNpadButton_Up) {
        if (selectedOption == NEW_GAME) {
            selectedOption = QUIT;  // Spring naar het einde van menu
        } else {
            selectedOption = static_cast<MenuOption>(selectedOption -1);  // Ga naar vorige optie
        }
    } else if (kDown & HidNpadButton_Down) {
        if (selectedOption == QUIT) {
            selectedOption = NEW_GAME;  // Spring naar het begin van menu
        } else {
            selectedOption = static_cast<MenuOption>(selectedOption + 1);  // Ga naar volgende optie
        }
    } else if (kDown & HidNpadButton_A) {
        // Voer actie uit op basis van de geselecteerde optie
        switch (selectedOption) {
            case NEW_GAME:
                inStartScreen = false;  // Start het spel
                break;
            case OPTIONS:
                // Voeg logica toe voor OPTIONS
                inStartScreen = false;
                break;
            case STATS:
                // Voeg logica toe voor STATS
                break;
            case HELP:
                // Voeg logica toe voor HELP
                break;
            case QUIT:
                quit = true; // Verlaat de game
                break;
        }
    } 
}


void renderOptionsMenu(SDL_Renderer* renderer) {
    const char* OptionsMenuOptions[] = {
        musicEnabled ? "Music: ON" : "Music: OFF",
        "Back to Main Menu"
    };
    SDL_Color white = { 255, 255, 255 };
    SDL_Color yellow = { 255, 255, 0 };

    // Titel van het options menu
    renderText(renderer, "- OPTIONS -", dosTitleFont, SCREEN_WIDTH / 2 - 100, 100);

    // Opties van het options menu
    for (int i = 0; i < 2; i++) {
        SDL_Color color = (i == selectedOptionsMenuOption) ? yellow : white;
        renderText(renderer, OptionsMenuOptions[i], dosFont, SCREEN_WIDTH / 2 - 50, 200 + i * 40, color);
    }
}

void handleOptionsMenuInput(PadState* pad) {
    u64 kDown = padGetButtonsDown(pad);

    if (kDown & HidNpadButton_Up) {
        selectedOptionsMenuOption = (selectedOptionsMenuOption == TOGGLE_MUSIC) ? BACK_TO_MAIN : TOGGLE_MUSIC;
    } else if (kDown & HidNpadButton_Down) {
        selectedOptionsMenuOption = (selectedOptionsMenuOption == BACK_TO_MAIN) ? TOGGLE_MUSIC : BACK_TO_MAIN;
    } else if (kDown & HidNpadButton_A) {
        switch (selectedOptionsMenuOption) {
            case TOGGLE_MUSIC:
                musicEnabled = !musicEnabled;
                if (musicEnabled) {
                    Mix_ResumeMusic();  // Muziek hervatten
                } else {
                    Mix_PauseMusic();  // Muziek pauzeren
                }
                break;
            case BACK_TO_MAIN:
                inStartScreen = true;   // ga terug naar het start menu
                break;
        }
    }

}

void renderQuitConfirmation(SDL_Renderer* renderer) {
    const char* quitOptions[] = { "Quit to Main Menu", "Exit the Game" };
    SDL_Color white = { 255, 255, 255 };
    SDL_Color yellow = { 255, 255, 0 };

    int padding = 140;
    int yPosition = SCREEN_HEIGHT - 30; // Vlak boven de onderkant van het scherm

    renderText(renderer, "Are you sure?", dosTitleFont, SCREEN_WIDTH / 2 - 100, 100);
    renderText(renderer, "(A) confirm", dosFont, SCREEN_WIDTH / 2 - 120, yPosition);
    renderText(renderer, "(B) cancel", dosFont, SCREEN_WIDTH / 2 + padding, yPosition);

    for (int i = 0; i < 2; i++) {
        SDL_Color color = (i == quitConfirmOption) ? yellow : white;
        renderText(renderer, quitOptions[i], dosFont, SCREEN_WIDTH / 2 - 50, 200 + i * 40, color);
    }
}

// Functie om evenementen te verwerken
void handleEvents(PadState* pad) {
    u64 kDown = padGetButtonsDown(pad); // Knop die net ingedrukt is.

    if (inStartScreen) {
        handleStartMenuInput(pad);  // Handle start menu controls
    } else if (isPaused) {
        handlePauseMenuInput(pad);
    } else if (selectedOption == OPTIONS) {
        handleOptionsMenuInput(pad);  // Bediening voor options menu
    } else {
        if (kDown & HidNpadButton_B) {
            shootBullet();
        } else if (kDown & HidNpadButton_Y && !isReloading) {
            startReloading();
        } else if (kDown & HidNpadButton_Plus) {
            isPaused = !isPaused;
        }
        handlePlayerMovement(pad);
    }
}

/*////////////////////////////////////////////////////////////////////
// Functie om de game loop te starten, main entrypoint!
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
int main(int argc, char* argv[]) {
    romfsInit();
    chdir("romfs:/");

    // Configureer de controller invoer
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);

    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    
    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        printf("SDL_mixer: %s\n", Mix_GetError());
        return -1;
    }

    //backgroundMusic = Mix_LoadMUS("romfs:/Sound/Level1Background.mp3");

    pistolShootSound = Mix_LoadWAV("romfs:/Sound/Weapons/Pistol/PistolFire.wav");
    pistolReloadSound = Mix_LoadWAV("romfs:/Sound/Weapons/Pistol/PistolReload.wav");
    backgroundMusic = Mix_LoadMUS("romfs:/Sound/Level1Background.mp3");

    // Speel de achtergrond muziek in een oneindige loop af
    Mix_PlayMusic(backgroundMusic, -1);

    font = TTF_OpenFont("romfs:/fonts/vera.ttf", 12);
    dosFont = TTF_OpenFont("romfs:/fonts/Perfect DOS VGA 437.ttf", 26);
    dosTitleFont = TTF_OpenFont("romfs:/fonts/Perfect DOS VGA 437.ttf", 48);



    SDL_Window* window = SDL_CreateWindow("Seal Hunter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Surface* tempSurface = IMG_Load("romfs:/Assets/UI/title_menu_bg.png");
    if (!tempSurface) {
        cerr << "Failed to load start menu background: " << IMG_GetError() << endl;
        quit = true;
    } else {
        startMenuBackgroundTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
        SDL_FreeSurface(tempSurface);
    }



    // Initialiseer de speler
    initPlayer(renderer);
    initBrownSeal(renderer);
    initMuzzleFlash(renderer);
    initBackdropAndIce(renderer);
    initHUD(renderer);
    initWeapons(renderer);
    initShellTextures(renderer);

    srand(static_cast<unsigned>(time(0)));  // Seed random for muzzle flash

    if (quit) {
        // Als er een fout is opgetreden tijdens de initialisatie van de speler
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        romfsExit();
        return -1;
    }

    
    /*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Code hieronder herhaalt zich gedurende de game, de loop stopt pas wanneer de game is beëindigd en is vaak ook een voorwaarde!
    \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*/
    while (!quit) {
        // Verwerk de controller invoer
        padUpdate(&pad);
        handleEvents(&pad);
        
        // Wis het scherm
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Zwarte achtergrond
        SDL_RenderClear(renderer);

        if (inStartScreen) {
            renderStartMenu(renderer);
        } else if (selectedOption == OPTIONS) {
            renderOptionsMenu(renderer); // Render het options menu als het geselecteerd is
        } else if (isPaused) {
            if (confirmingQuit) {
                renderQuitConfirmation(renderer);
            } else {
                renderPauseMenu(renderer);
            }
        } else {
            updateBullets();
            updateReload(); // Check of reload complete is
            updateShells();
            updateBrownSeal();

            renderBackdropAndIce(renderer); // Render de achtergrond en ijslaag
            renderHUD(renderer);
            renderPlayer(renderer);         // Render de speler
            renderBrownSeal(renderer);
            renderBullets(renderer);
            renderMuzzleFlash(renderer);
            renderShells(renderer);
            renderBulletCount(renderer);
        }
        // Update het scherm
        SDL_RenderPresent(renderer);
    }

    // Opruimen van de shell textures
    for (int i = 0; i < 4; i++) {
        SDL_DestroyTexture(shellTextures[i]);
    }

    // Code Opruimen en ruimte vrij maken!
    Mix_HaltMusic();
    Mix_FreeMusic(backgroundMusic);
    backgroundMusic = nullptr;

    Mix_FreeChunk(pistolShootSound);
    Mix_FreeChunk(pistolReloadSound);

    SDL_DestroyTexture(startMenuBackgroundTexture);
    SDL_DestroyTexture(muzzleFlashTextures[0]);
    SDL_DestroyTexture(muzzleFlashTextures[1]);
    SDL_DestroyTexture(player.idleTexture);
    SDL_DestroyTexture(player.walkTexture1);
    SDL_DestroyTexture(player.walkTexture2);
    SDL_DestroyTexture(player.weaponTexture);
    SDL_DestroyTexture(player.crosshairTexture);
    SDL_DestroyTexture(backdropTexture);
    SDL_DestroyTexture(iceTexture);
    SDL_DestroyTexture(hudTexture);
    SDL_DestroyTexture(brown_seal.texture);

    Mix_CloseAudio();
    TTF_CloseFont(font);
    TTF_CloseFont(dosFont);
    TTF_CloseFont(dosTitleFont);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
    romfsExit();
    return 0;
}
