Seal Hunter NX - The Seal Hunters
Nintendo Switch Homebrew project
by Kenneth van der Maazen (c) 2024

Pre-Build setup:
    Root folder created:
        .vscode
            \ c_cpp_properties.json
            \ launch.json
            \ settings.json
        romfs
            \ Assets
        source
            \ main.cpp

    .vscode folder setup: 
        SDL2 library linked.
        Nintendo Switch library linked.
        Compiler path set to "C:/devkitPro/devkitA64/bin/aarch64-none-elf-g++"

    Include C++ libraries:
        string.h
        stdio.h
        stdlib.h
        iostream
        switch.h            
        SDL2/SDL.h          
        SDL2/SDL_image.h    

        Makefile created
        icon.jpg created (256 x 256 pixels)

        Nintendo Switch screen size defined.
        Function for event handling created.
        Controller input handling created.
        Main application enrtypoint created.
        Game loop created.

=================================================================================================================================================================================
To-Do:
* Add 2-4 player functionality (Local Co-op / Online Co-op)
* Add collision detection
* Add enemies
* Add scorekeeping
* Add levels

* Wanneer speler terug gaat naar Main menu moet de game resetten.
* Tijdens een pauze menu moet het huidige level waarop gespeeld wordt zichtbaar zijn.


* Lijn 612 - titlescherm

(0,0) ------------------------------> x (horizontale as)
    |
    |
    |
    |
    |
    v
y (verticale as)

-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
- Display start screen
inStartScreen:

    * New Game
        \ 1 Player
        \ 2 Players (online/adhoc) ?
        < Back

    * Options
        \ P1 Control
        \ P2 Control
        \ Game
        \ System
        < Main menu

    * Stats
        \ General
        \ Kills
        \ Deaths
        \ Highscore
        < Main menu

    * Help
        \ General
        \ Enemies
        \ Weapons
        \ About
        < Main menu

    * Quit
        > Exits the game

-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Basic Seal Hujnter Controls (default):
Description         Key         Function
Move up             UP          Moves character up.
Move left           LEFT        Moves character left.
Move down           DOWN        Moves character down.
Move right          RIGHT       Moves character right.
Jump                (A)         Character jumps. Can smash a crawling brown seal by falling on him.
Fire (shoot)        (B)         Gun fires.
Reload weapon       (Y)         Reloads the current weapon.
Quickbuy            (X)         Buys the next available weapon.
Pause Game          [+]         Pauses the game.
Open buymenu        [-]         Opens a menu where you can buy different weapons.
Switch weapons      [L]/[R]     Toggles between all weapons, press [L] to switch to the previous weapon, [R] to switch to the next weapon.
Show stats          [ZR]        Show various informations: team, money, loadout, kill count, level progress.
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Lv. 1 Brown Seal
Lv. 1 Boss Fat Seal Boss

Lv. 2 Brown Seal, British Activist
    Pause (if > 1 enemies alive; do not kill last enemy)
Lv. 2 Boss Penguin Boss

Lv. 3 Brown Seal, British Activist, Penguin
    Special Pause (5-10 seconds after last enemy has been spawned)
Lv. 3 Boss Polar Bear Boss

Lv. 4 Brown Seal, British Activist, Penguin, Fat Seal
Lv. 4 Boss Turtle Boss

Lv. 5 Brown Seal, British Activist, Penguin, Fat Seal, Polar Bear
    Pause (if > 1 enemies alive; do not kill last enemy)
Lv. 5 Boss Walrus
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Enemy count tables (classic):
        Seals   Activists   Penguins    Fat Seals   Bears   Turtles Walrus
Lv. 1   32      0           0           1           0       0       0
Lv. 2   16      9           9           0           0       0       0
Lv. 3   17      13          12          0           1       0       0
Lv. 4   0       0           0           0           0       1       0
Lv. 5   17      13          14          5           4       0       1
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Enemy per level tables
Brown Seal (Lv. 1-5)
HP: 26+(Level*21)   Lv.1: 47 HP  Lv.2: 68 HP    Lv.3: 89 HP    Lv.4: 110 HP    Lv.5: 131 HP
Bounty: 50$
Headshot Bonus: 10$
Headshot Multiplier: x2
Moving Speed: 25-55 px/sec
Special: Moves in a straight line. May crawl while critically wounden. Jump on a crawling seal to kill it.

Fat Seal (Lv. 1-5)
HP: (Level*250)   Lv.1 Boss: 1250 HP  Lv.4: 1000 HP    Lv.5: 1250 HP
Bounty: 200$ (400$ if boss)
Grenade Bonus: 5-15$
Eyeshot Bonus: 0$
Eyeshot Multiplier: x1.25
Moving Speed: 35 px/sec
Special: Moves in a straight line. Regenerates HP when standing still. Can overrun the player.

British Activist (Lv. 2-5)
HP: 30+(Level*35)   Lv.2: 100 HP    Lv.3: 135 HP    Lv.4: 170 HP    Lv.5: 205 HP
Bounty: 100$
Headshot Bonus: 15$
Headshot Multiplier: x2
Moving Speed: 55-100 px/sec
Special: Runs in zigzags. Carries a Seal Cub. Player can let activist pass the screen if Seal Cub is killed.

Seal Cub (Lv. 2-5)
HP: 10+(Level*5)    Lv.2: 20 HP     Lv.3: 25 HP     Lv.4: 30 HP     Lv.5: 35 HP
Bounty: 25$
Moving Speed: 22-37 px/sec
Special: Moves in a straight line. Can be mashed by a Polar Bear.

Penguin (Lv. 2-5)
HP: (Level*25)   Lv.2 Boss: 50 HP    Lv.3: 75 HP    Lv.4: 100 HP    Lv.5: 125 HP
Bounty: 100$
Headshot Bonus: 10$
Headshot Multiplier: x2
Waling Speed: 32-55 px/sec
Sliding Speed: 87-157 px/sec
Special: Moves (walks) in zigzags. Starts to slide on his stomach after a while.

Polar Bear (Lv. 3-5)
HP: 1700 HP (Boss: 2000 HP)
Bounty: 500$
Headshot Bonus: 0$
Headshot Multiplier: x2
Moving Speed: 40-370 px/sec
Special: Moves in zigzags. Gets enraged when hit & runs faster. Can smash seal cub, when falling and sliding on the ground.

Turtle (Lv. 4)
HP: N/A
Bounty: 500$
Headshot Bonus: 10$
Headshot Multiplier: N/A
Moving Speed: 35 px/sec
Special: Moves in a straight line. Can hide in turtle shell if player shoots at him.

Walrus (Lv. 5)
HP: N/A
Bounty: 1000$
Headshot Bonus: 0$
Headshot Multiplier: N/A
Moving Speed: N/A
Special: Moves in a straight line. Goes a little forward and stops for some seconds. Can eat players.
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Weapon Overview
            Cost    Unlock Lv.  Accuracy        Ammo    Reload Time     Rate of Fire    Damage / Bullet     Damage / Sec
Pistol      Free    1           Perfect         12      2.0s            4/sec           18                  41
Magnum      500$    2           Perfect         6       2.2s            2.5/sec         60                  150
Land Mine   750$    17          Perfect         3       N/A             4/sec           220 (60px splash)   880
MP5         1000$   3           +/- 2 units     30      1.5s            10/sec          23                  230
Grenades    1500$   7           N/A             1       1.0s            1/sec           250 (95px splash)   250
Shotgun     2500$   5           +/- 8.5 units   8       2.5s            1.25/sec        40                  400
Dual Macs   3500$   9           +/- 8 units     60      2.6s            15/sec          37                  555
M79         5000$   11          N/A             5       3.0s            2.5/sec         400 (100px splash)  1000
Colt M16    6000$   13          +/- 2 units     30      1.6s            10/sec          60                  600
AWP         7500$   15  	    Perfect         5       2.2s            0.75/sec        855                 641
Carl Gustav 9000$   19          Perfect         1       0.6s            1.5/sec         400 (60px splash)   600
Punt Gun    10000$  20          +/- 22 units    1       1.3s            0.77/sec        125                 1442
Minigun     12500$  22          +/- 5 units     150     2.0s            15/sec          65                  975

Cost            - weapon's price ingame
Unlock Lv.      - level when you get this weapon unlocked
Accuracy        - weapon's accuracy or spray.
+/- X units     - width of the bullets spray. Bullets will randomly spray from -X to +X from the player middle.
Ammo            - bullet or grenade count of the weapon.
Reload Time     - weapon's reload time in seconds.
Rate of Fire    - weapon's fire rate in bullets per second.
Damage / Bullet - how much damage per single bullet or grenade.
X px splash     - Area of Effect damage on X pixels around the bullet epicenter.
Damage / Sec    - how much damage the weapon does per second.
-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
Seal Hunter Ranks & Unlocks
Level   XP Required     Title Unlock                    Weapon Unlock
1       0               Disgruntled Fisherman           --
2       300             Aggravated Fisherman            Magnum
3       1,000           Spraying Machine                MP5
4       2,500           Hunting Apprentice              --
5       5,000           Wild at Heart                   Shotgun
6       7,500           Omega-3 Salesman                --
7       10,000          Boomer                          Grenades
8       15,000          Fishermen's Rights Activist     --
9       20,000          Akimbo Assasin                  MAC-10
10      25,000          Harbringer of Fish              --
11      30,000          Master Exploder                 M79
12      35,000          Force of Nature                 --
13      40,000          Special Forces Member           Colt M16
14      45,000          Twisted Eskimo                  --
15      50,000          Sniper                          AWP
16      55,000          Sam Fisherman                   --
17      60,000          Miner                           Land Mine
18      65,000          Fifth Plague                    --
19      70,000          Snowman of Death                Carl Gustav
20      75,000          Punter                          --
21      80,000          Honorable Fisherman             Punt Gun
22      90,000          Heavy                           --
23      100,000         Norwegian Patriot               Minigun
24      150,000         Industry Protector              --
25      250,000         Hunting Master                  --

=================================================================================================================================================================================

1. Projectplanning en ontwerp:
    * Doelstellingen definieren: bepaal de kernfunctionaliteiten van het spel, zoals gameplay mechanica, aantal levels en multiplayer opties.
    * Technologiestack kiezen: Selecteer gechikste bibliotheken en tools voor dit project. In dit geval is het een Nintendo Switch Homebrew project, dus we zullen gebruik maken van de C++ bibliotheek met SDL2 en SFML.

2. Ontwikkelomgeving opzetten:
    * IDE installeren: kies een Interdrated Development Environment (IDE) zoals Visual Studio.
    * Bibliotheken intergreren: Download en configureer de gekozen bibliotheken binnen de ontwikkelomgeving, SDL2 & SFML

3. Basisstructuur implementeren:
    * Hoofdgame-loop creeren: Ontwikkel een loop die de game blijft uitvoeren totdat de speler besluit te stoppen.
    * Event handling: Implementeer mechanismen om gebruikersinvoer te verwerken, zoals toetsenbord en muisacties.
    * Rendering: Zorg voor het tekenen van objecten op het scherm, zoals de speler, vijanden en achtergrond.

4. Gameplay mechanica ontwikkelen:
    * Spelerobject: Maak een klasse voor de speler met eigenschappen zoals positie, snelheid en gezondheid.
    * Vijanden: Ontwerp verschillende vijandtypes met unieke gedragingen en eigenschappen.
    * Wapens en projectielen: Implementeer diverse wapens en de bijbehorende projectielen die de speler kan gebruiken.

5. Spelregels en logica:
    * Score en puntensysteem: Ontwikkel een systeem om de score van de speler bij te houden en weer te geven.
    * Levelprogressie: Bepaal hoe de speler door verschillende levels of golven van vijanden beweegt.
    * Gameover en wincondities: Definieer de voorwaarden waaronder het spel voorbij of gewonnen is.

6. Audio en visuele elementen:
    * Grafische assets: Ontwerp of verzamel sprites voor personages, vijanden en achtergronden.
    * Geluidseffecten en muziek: Voeg audio toe voor acties zoals schieten, explosies en achtergrond muziek.

7. Testen en debuggen:
    * Unit tests: Schrijf tests voor individuele componenten om ervoor te zorgen dat ze correct functionerem.
    * Integratietests: Test de interactie tussen verschillende componenten.
    * Gebruikerstests: Laat anderen je spel spelen om feedback te verzamelen en eventuele problemen te identificeren.

8. Optimalisatie en verfijning:
    * Prestatieverbeteringen: Optimaliseer de code om een soepele gameplay-ervaring te garanderen.
    * Bugfixes: Los eventuele gevonden bugs op tijdens het testproces.
    * Gameplay-balans: Pas moelijkheidsgraden en andere parameters aan voor een evenwichtige spelervaring.

9. Distributie en onderhoud:
    * Versiebeheer: Gebruik tools zoals Git om de code te beheren en bij te houden.
    * Documentaties: Schrijf handleidingen en documentatie voor gebruikers en andere ontwikkelaars.
    * Updates en ondersteuning: Blijf het spel bijwerken op basis van feedback en zorg voor ondersteuning bij problemen.




