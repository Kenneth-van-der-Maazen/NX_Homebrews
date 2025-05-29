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

    char keysNames[28][32] = {
        "(A)", "(B)", "(X)", "(Y)",
        "Stick_L", "Stick_R", "L", "R",
        "ZL", "ZR", "Plus", "Minus",
        "Left", "Up", "Right", "Down",
        "Stick_L_Left", "Stick_L_Up", "Stick_L_Right", "Stick_L_Down",
        "Stick_R_Left", "Stick_R_Up", "Stick_R_Right", "Stick_R_Down",
        "Left_SL", "Left_SR", "Right_SL", "Right_SR",
    };

    u32 kDownOld = 0, kHeldOld = 0, kUpOld = 0; // Deze variabelen bevatten informatie over welke knoppen zijn gedetecteerd in vorige frame

    printf("\x1b[1;1HDruk op de + om te verlaten.");
    printf("\x1b[2;1HLinker joystick positie: ");
    printf("\x1b[4;1HRechter joystick positie: ");

    // Main loop
    while(appletMainLoop())
    {
        padUpdate(&pad);

        u64 kDown = padGetButtonsDown(&pad);

        u64 kHeld = padGetButtons(&pad);

        u64 kUp = padGetButtonsUp(&pad);

        if(kDown & HidNpadButton_Plus)  {break;}

        // Print alleen de waarde als de staat van een knop is gewijzigd
        if (kDown != kDownOld || kHeld != kHeldOld || kUp != kUpOld)
        {
            // Console scherm leeg maken
            consoleClear();

            // Deze 2 regels moeten worden hergeschrevem omdat de console leeg is gemaakt
            printf("\x1b[1;1HDruk op de + om te verlaten.");
            printf("\x1b[2;1HLinker joystick positie: ");
            printf("\x1b[4;1HRechter joystick positie: ");

            printf("\x1b[6;1H");

            // Check of sommige knoppen ingedrukt, vastgehouden of losgelaten zijn
            int i;
            for ( i= 0; i < 28; i++)
            {
                if (kDown & BIT(i)) printf("%s ingedrukt\n", keysNames[i]);
                if (kHeld & BIT(i)) printf("%s vastgehouden\n", keysNames[i]);
                if (kUp & BIT(i)) printf("%s los\n", keysNames[i]);
            }
        }

        // Stel knoppen met de oude waarden in voor de volgende frame
        kDownOld = kDown;
        kHeldOld = kHeld;
        kUpOld = kUp;

        // Lees de positie van de sticks
        HidAnalogStickState analog_stick_l = padGetStickPos(&pad, 0);
        HidAnalogStickState analog_stick_r = padGetStickPos(&pad, 1);

        // Print de posities van sticks
        printf("\x1b[3;1H%04d; %04d", analog_stick_l.x, analog_stick_l.y);
        printf("\x1b[5;1H%04d; %04d", analog_stick_r.x, analog_stick_r.y);

        // Update de console, nieuwe frame sturen
        consoleUpdate(NULL);
    }

    consoleExit(NULL);
    return 0;
}