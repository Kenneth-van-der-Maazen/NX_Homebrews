#pragma once
#define __SETTINGS_HPP__

#include <iostream>
#include <stdio.h>

class Game
{
public:
    Game();
    ~Game();
    void handleEvents();
    void update_welcome();
    void render_welcome();
};