#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <switch.h>


int main(int argc, char* argv[])
{
    consoleInit(NULL);

    padConfigureInput(1, HidNpadStyleSet_NpadStandard);

    PadState pad;
    padInitializeDefault(&pad);

    hidInitializeTouchScreen();

    s32 prev_touchcount = 0;

    printf("\x1b[1;1HDruk op de + om de applicatie te verlaten.");
    printf("\x1b[2;1HTouch-sreen inputs en de daarbij horende positie: ");


    // Main loop
    while(appletMainLoop())
    {
        padUpdate(&pad);

        u64 kDown = padGetButtonsDown(&pad);

        if(kDown & HidNpadButton_Plus)  {break;}

        HidTouchScreenState state={0};
        if (hidGetTouchScreenStates(&state, 1)) {
            if (state.count != prev_touchcount)
            {
                prev_touchcount = state.count;

                consoleClear();

                printf("\x1b[1;1HDruk op de + om de applicatie te verlaten.");
                printf("\x1b[2;1HTouch-sreen inputs en de daarbij horende positie: ");
            }

            printf("\x1b[3;1H");

            for(s32 i=0; i<state.count; i++)
            {
                // print de touch-screen coordinaten
                printf("[%d] x=%03d, y=%03d, diameter_x=%03d, diameter_y=%03d, graden_hoek=%03d\n", i, state.touches[i].x, state.touches[i].y, state.touches[i].diameter_x, state.touches[i].diameter_y, state.touches[i].rotation_angle);
            }
        }

        
        consoleUpdate(NULL);
    }

    consoleExit(NULL);
    return 0;
}