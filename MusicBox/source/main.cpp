#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <switch.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

int main(int argc, char *argv[])
{
    consoleInit(NULL);

    // Configure our supported input layout: a single player with standard controller styles
    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    // Initialize the default gamepad (which reads handheld mode inputs as well as the first connected controller)
    PadState pad;
    padInitializeDefault(&pad);

    Result rc = romfsInit();
    if (R_FAILED(rc))
        printf("romfsInit: %08X\n", rc);

    else
        printf("\x1b[8;8H[ My Music Box ]\n");


    // Other initialization goes here. As a demonstration, we print Music Box.
    //printf("\x1b[8;8H[ My Music Box ]\n");
    SDL_Init(SDL_INIT_AUDIO);
    Mix_Init(MIX_INIT_MP3);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);
    //Mix_Music *audio = Mix_LoadMUS("romfs:/data/Music Box (Lullaby).mp3");
    Mix_Music *audio = Mix_LoadMUS("romfs:/data/Music Box (Lullaby).mp3");
    Mix_Music *track1 = Mix_LoadMUS("romfs:/data/Music Box (Explicit).mp3");
    Mix_Music *track2 = Mix_LoadMUS("romfs:/data/Zetsubou No Shima.mp3");
    Mix_Music *track3 = Mix_LoadMUS("romfs:/data/Pack-A-Punch Jingle.mp3");

    // Main loop
    while (appletMainLoop())
    {
        padUpdate(&pad);
        u64 kDown = padGetButtonsDown(&pad);
    
        if (kDown & HidNpadButton_Plus) break; // break in order to return to hbmenu

        if (kDown & HidNpadButton_A)
        {
            Mix_PlayMusic(audio, 1); //Play the audio file
            printf("\nMusic Box");
            printf("\nby EMINEM\n");
        }

        if (kDown & HidNpadButton_B)
        {
            Mix_PlayMusic(track1, 1); //Play the audio file
            printf("\nMusic Box (EXPLICIT VERSION)");
            printf("\nby EMINEM\n");
        }

        if (kDown & HidNpadButton_Y)
        {
            Mix_PlayMusic(track2, 1); //Play the audio file
            printf("\nZetsubou No Shima");
            printf("\nSamantha's Lullaby\n");
        }

        if (kDown & HidNpadButton_X)
        {
            Mix_PlayMusic(track3, 1); //Play the audio file
            printf("\nPack-A-Punch Jingle");
            printf("\nCall of Duty: Black Ops II\n");
        }

        // Update the console, sending a new frame to the display
        consoleUpdate(NULL);
    }

    Mix_FreeMusic(audio);
    Mix_FreeMusic(track1);
    SDL_Quit();
    romfsExit();
    consoleExit(NULL);
    return 0;
}

//https://www.c-sharpcorner.com/forums/custom-console-menu-in-cpp
//https://jdonaldmccarthy.wordpress.com/tag/c/