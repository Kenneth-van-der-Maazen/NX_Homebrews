/*  Nintendo Switch homebrew projects 
    by Kenneth van der Maazen (c) 2024
    
    Space Invaders type game created in C++ using the SDL2 Library.
    it's a very basic homebrew concept made without any official instructions or support from Nintendo.
    I created this for educational purposes and to learn more about game development.
    Homebrews are not supported by Nintendo and may not work on all systems.

    Date created: 20-10-2024
    Last updated: 28-10-2024 @ 18:11
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <ctime>

#include <switch.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>


using namespace std;


const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;


int spaceshipX = 640;
const int spaceshipY = 620; // Hoe hoger de waarde hoe lager in het beeld de spaceship wordt gerenderd
const int spaceshipWidth = 50;
const int spaceshipSpeed = 6; // snelheid van het verplaatsen

const int maxBullets = 10;
const int maxEnemies = 10;
SDL_Texture *enemyTexture;
SDL_Texture *gameoverTexture;

/* GLOBALE VARIABELEN */
bool quit = false;
bool gameover = false;
bool inStartScreen = true;
bool isPaused = false;
bool reset = false;

int score = 0;

Mix_Chunk *gameoverSound = nullptr;
Mix_Chunk *shootSound = nullptr;
Mix_Chunk *enemyHitSound = nullptr;

Mix_Music *startScreenMusic = nullptr;

Uint32 gameoverTime = 0;

struct Star {
    int x;
    int y;
    int speed;
};
const int maxStars = 100; // Het aantal sterren op het scherm
vector<Star> stars(maxStars);

struct Bullet{
    int x;
    int y;
    int speed;
    bool active;
};
vector<Bullet> bullets(maxBullets);

struct Enemy{
    int x;
    int y;
    int speed;
    bool active;
    int width;
    int height;
    int health; // Enemy aantal levens punten
};
vector<Enemy> enemies(maxEnemies);

struct EnemyBullet {
    int x;
    int y;
    int speed;
    bool active;
};

void initStars() {
    srand(static_cast<unsigned>(time(NULL))); // Willekeurige sterren

    for (int i = 0; i < maxStars; ++i) {
        stars[i].x = rand() % SCREEN_WIDTH; // Willekeurige x-positie
        stars[i].y = rand() % SCREEN_HEIGHT; // Willekeurige y-positie
        stars[i].speed = rand() % 3 + 1; // Willekeurige snelheid tussen 1 en 3
    }
}

void updateStars() {
    for (int i = 0; i < maxStars; ++i) {
        stars[i].y += stars[i].speed; // Sterren bewegen naar beneden

        if (stars[i].y > SCREEN_HEIGHT) {
            stars[i].y = 0; // Reset de y-positie bovenaan
            stars[i].x = rand() % SCREEN_WIDTH; // Geef nieuwe willekeurige x-posities
        }
    }
}

void renderStars(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // Witte sterren

    for (int i = 0; i < maxStars; ++i) {
        SDL_RenderDrawPoint(renderer, stars[i].x, stars[i].y); // Teken de ster als een punt
    }
}

void renderText(SDL_Renderer *renderer, const char* text, TTF_Font *font, int x, int y) {
    SDL_Color white = { 255, 255, 255 }; // Witte tekst
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text, white);
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

    SDL_Rect messageRect;
    messageRect.x = x;
    messageRect.y = y;
    messageRect.w = surfaceMessage->w;
    messageRect.h = surfaceMessage->h;

    // Render de tekst op het scherm
    SDL_RenderCopy(renderer, message, NULL, &messageRect);

    // Opruimen
    SDL_DestroyTexture(message);
    SDL_FreeSurface(surfaceMessage);
}

void fireBullet() {
    for(int i = 0; i < maxBullets; ++i) {
        if(!bullets[i].active) {
            bullets[i].active = true;
            bullets[i].x = spaceshipX + spaceshipWidth / 2;
            bullets[i].y = spaceshipY;
            bullets[i].speed = 10; // Bullets speed

            // Speel het schietgeluid af
            Mix_PlayChannel(-1, shootSound, 0);


            break;
        }
    }
}

void updateBullets() {
    for(int i = 0; i < maxBullets; ++i) {
        if (bullets[i].active) {
            bullets[i].y -= bullets[i].speed; // Laat de bullet naar boven bewegen
            if (bullets[i].y < 0) { // Als de bullet buiten het scherm is, reset de bullet

                bullets[i].active = false;
            }
        }
    }
}

void renderBullets(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // WITTE BULLETS
    for (int i = 0; i < maxBullets; ++i) {
        if (bullets[i].active) {
            SDL_Rect bulletRect = { bullets[i].x, bullets[i].y, 5, 10}; // Bullet grootte
            SDL_RenderFillRect(renderer, &bulletRect); // Teken de bullet in window
        }
    }
}

void loadEnemies(SDL_Renderer *renderer) {
    enemyTexture = IMG_LoadTexture(renderer, "sprites/enemies.png");

    if(!enemyTexture) {
        cout << "Error loading enemies.png";
        return;
    }

    srand(static_cast<unsigned>(time(NULL)));
    for(int i = 0; i < maxEnemies; ++i) {
        enemies[i].x = rand() % 1280;
        enemies[i].y = rand() % 100 - 150;
        enemies[i].width = 55;
        enemies[i].height = 35;
        enemies[i].active = true;
        enemies[i].speed = rand() % 1 + 1;
        enemies[i].health = rand() % 3 + 1;
    }
}

bool checkCollision(const SDL_Rect& rect1, const SDL_Rect& rect2) {
    return SDL_HasIntersection(&rect1, &rect2);
}

void respawnEnemy(Enemy& enemy) {
    enemy.x = rand() % 1280;
    enemy.y = rand() % 100 - 150;
    enemy.active = true;
    enemy.health = rand() % 3 + 1;
}

void resetGame() {
    gameover = false;
    reset = false;
    gameoverTime = 0;
    score = 0; // Reset score bij een nieuwe game

    spaceshipX = 640; // Reset spaceship naar beginpositie
    // Reset alle enemies
    for (int i = 0; i < maxEnemies; ++i) {
        respawnEnemy(enemies[i]);
    }
    // Reset alle bullets
    for (int i = 0; i < maxBullets; ++i) {
        bullets[i].active = false;
    }
}

bool handleEvents(PadState *pad) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            quit = true;
        }
    }
    
    // Haal de knoppeninvoer op van de controller
    u64 kDown = padGetButtonsDown(pad);
    u64 kHeld = padGetButtons(pad);

    // Als we in het startscherm zijn, wachten op A-knop om het spel te starten
    if (inStartScreen) {
        if (kDown & HidNpadButton_A) {
            inStartScreen = false; // Verlaat startscherm

            // Fade-out van de muziek over 2 seconden (2000 ms)
            Mix_FadeOutMusic(2000);
        }
        return true;
    }

    // Als het spel gepauzeerd is
    if (isPaused) {
        // Als de MINUS-knop wordt ingedrukt terwijl het spel gepauzeerd is, sluit het spel af
        if (kDown & HidNpadButton_Minus) {
            quit = true; // Be-eindig het spel
            //break;
        } 
        // Mogelijkheid om het spel weer te hervatten met PLUS
        if (kDown & HidNpadButton_Plus) {
            isPaused = false; // Verlaat pauze
        }
        return true; // Geen verdere acties uitvoeren als het spel gepauzeerd is
    }

    // Als het game over is, wacht op de A-knop om het spel te resetten
    if (gameover) {
        // Bereken hoeveel tijd verstreken is sinds gameover
        Uint32 timeSinceGameover = SDL_GetTicks() - gameoverTime;

        // Wacht 5 seconden voordat de A-knop het spel kan resetten
        if (timeSinceGameover >= 5000 && (kDown & HidNpadButton_A)) {

            reset = true;
        }
        // Geen verdere acties uitvoeren als het gameover is
        return true;
    }


    // Controleer of een van de knoppen is ingedrukt: (A) (B) [ZR] [R]
    if (!gameover && !inStartScreen && !isPaused) { // Alleen schieten als het spel niet game over is!
        if (kDown & HidNpadButton_A || kDown & HidNpadButton_B || kDown & HidNpadButton_ZR || kDown & HidNpadButton_R) {
            fireBullet();
        } else if (kDown & HidNpadButton_Plus) {
            isPaused = !isPaused; // Wissel tussen pauzeren en hervatten
            return true; // Geen verdere acties vereist
        }

        // Beweging van het ruimteschip naar links en rechts, en binnen het scherm blijven
        if (kHeld & HidNpadButton_Left && spaceshipX > 0) {
            spaceshipX -= spaceshipSpeed; // Verplaatst naar links
        }
        if (kHeld & HidNpadButton_Right && spaceshipX + spaceshipWidth < 1280) {
            spaceshipX += spaceshipSpeed; // Verplaatst naar rechts
        }
    }

    return true;
}


// Main application entrypoint
int main(int argc, char* argv[]) {
    romfsInit();
    chdir("romfs:/");

    // Configureer de controller-invoer
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);
    PadState pad;
    padInitializeDefault(&pad);

    // Initialize SDL
    SDL_Init(SDL_INIT_EVERYTHING);

    // Initialize SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1) {
        printf("SDL_mixer: %s\n", Mix_GetError());
        return -1;
    }


    gameoverSound = Mix_LoadWAV("sfx/game-over.wav");   // Laden van het game over-geluidje
    shootSound = Mix_LoadWAV("sfx/laser-gun.wav");      // Laden van het schietgeluidje
    enemyHitSound = Mix_LoadWAV("sfx/oof-04s.wav");     // Laden van het geluidje wanneer een vijand wordt geraakt/gedoodt

    startScreenMusic = Mix_LoadMUS("sfx/spaceinvaders1.mpeg");
    if (!startScreenMusic) {
        printf("Failed to load music: %s\n", Mix_GetError());
        return -1;
    }

    // Start het afspelen van de muziek in een loop, maar alleen in het startscherm
    if (inStartScreen) {
        Mix_PlayMusic(startScreenMusic, -1); // Speel in een loop af
    }

    // Font initialisatie
    if (TTF_Init() == -1) {
        cout << "Failed to initialize TTF: " << TTF_GetError() << endl;
        return -1;
    }

    // Laad een font
    TTF_Font *font = TTF_OpenFont("fonts/Perfect DOS VGA 437.ttf", 24);

    // Maak een nieuw venster aan
    SDL_Window *window = SDL_CreateWindow("Space Invaders NX", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    // Maak een renderer aan voor op het venster te tekenen
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Laad het ruimteschip in als texture
    SDL_Texture *spaceshipTexture = IMG_LoadTexture(renderer, "sprites/spaceship.png");
    if(!spaceshipTexture) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();

    }

    loadEnemies(renderer);
    initStars();

    // Game loop (Main)
    while(!quit) {
        // Verwerk de controller-invoer
        padUpdate(&pad);
        handleEvents(&pad);

        // Maak het scherm leeg
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Zwarte achtergrond
        SDL_RenderClear(renderer);

        updateStars();
        renderStars(renderer);

        // Teken het ruimteschip
        SDL_Rect spaceshipRect = {spaceshipX, spaceshipY, spaceshipWidth, spaceshipWidth };
        SDL_RenderCopy(renderer, spaceshipTexture, NULL, &spaceshipRect);

        if (inStartScreen) {
            renderText(renderer, "PRESS (A) TO START!", font, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2);
        }

        // Controleer of het spel gepauzeerd is
        if (isPaused) {
            // Render de pauzetekst
            renderText(renderer, "- GAME PAUSED -", font, SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2);
        } else {
            // Voer het spel alleen uit als het niet gepauzeerd is
            if (!inStartScreen && !gameover) {
                updateBullets();
                renderBullets(renderer);

                // Collision detectie tussen de enemies en het ruimteschip
                for(int i = 0; i < maxEnemies; ++i) {
                    if (enemies[i].active) {
                        SDL_Rect enemyRect = { enemies[i].x, enemies[i].y, enemies[i].width, enemies[i].height };
                        SDL_RenderCopy(renderer, enemyTexture, NULL, &enemyRect);
                        enemies[i].y += enemies[i].speed;

                        if (checkCollision(spaceshipRect, enemyRect)) {
                            gameover = true;

                            // Speel het game-over geluidje af als het voor het eerst game over gaat
                            if (gameoverTime == 0) {
                                Mix_PlayChannel(-1, gameoverSound, 0); // Speel het geluidje eenmalig af
                                gameoverTime = SDL_GetTicks(); // Sla de tijd op wanneer gameover begint
                            }
                        }

                        if (enemies[i].y > 720) {
                            respawnEnemy(enemies[i]);
                        }
                    }
                }
            }

            // Check voor botsingen tussen bullets en enemies
            for (int i = 0; i < maxBullets; ++i) {
                if (bullets[i].active) {
                    SDL_Rect bulletRect = { bullets[i].x, bullets[i].y, 2, 10 };

                    for (int j = 0; j < maxEnemies; ++j) {
                        if (enemies[j].active) {
                            SDL_Rect enemyRect = { enemies[j].x, enemies[j].y, enemies[j].width, enemies[j].height };
                            if (checkCollision(bulletRect, enemyRect)) {
                                enemies[j].health--;
                                // Markeer bullet als inactief
                                bullets[i].active = false;

                                // Hier de loop van wanneer enemy 0 of minder health heeft en dus dood gaat
                                if (enemies[j].health <= 0) {
                                    // Verhoog de score
                                    score += 10;

                                    // Speel het vijand-doodschiet-geluid af
                                    Mix_PlayChannel(-1, enemyHitSound, 0);

                                    // Respawn de vijand
                                    respawnEnemy(enemies[j]);
                                }
                            }
                        }
                    }
                }
            }
        
            // Als het game over is, toon het gameover-scherm
            if (gameover) {
                if (gameoverTime == 0) {
                    gameoverTime = SDL_GetTicks(); // Sla de tijd op wanneer gameover begint
                }

                gameoverTexture = IMG_LoadTexture(renderer, "sprites/gameover.png");
                SDL_Rect gameoverRect = { 0, 0, 1280, 720 };
                SDL_RenderCopy(renderer, gameoverTexture, NULL, &gameoverRect);

                // Bereken hoeveel tijd verstreken is sinds de gameover
                Uint32 timeSinceGameover = SDL_GetTicks() - gameoverTime;

                // Laat de gebruiker pas na 5 seconden (5000 ms ) het spel resetten
                if (timeSinceGameover >= 5000 && reset) {
                    resetGame(); // Reset het spel
                }

                SDL_DestroyTexture(gameoverTexture); // Verwijder gameoverTexture na reset
            }

            // Score weergeven in rechterbovenhoek
            string scoreText = "Score: " + to_string(score);
            renderText(renderer, scoreText.c_str(), font, SCREEN_WIDTH - 200, 20);

        }

        // Toon de nieuwe frame
        SDL_RenderPresent(renderer);
    }   

    // Code opruimen en geheugen vrijmaken
    SDL_DestroyTexture(spaceshipTexture);
    SDL_DestroyTexture(gameoverTexture);

    Mix_FreeChunk(gameoverSound);
    Mix_FreeChunk(shootSound);
    Mix_FreeChunk(enemyHitSound);
    Mix_FreeMusic(startScreenMusic);
    
    startScreenMusic = nullptr;
    gameoverSound = nullptr;
    shootSound = nullptr;
    enemyHitSound = nullptr;

    Mix_CloseAudio();
    TTF_CloseFont(font);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
    
    romfsExit();
    return 0;
}