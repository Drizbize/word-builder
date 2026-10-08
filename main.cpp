//#define DEBUG
#define RL_TEXT_IMPLEMENTATION

#include "includes/Game.hpp"

int main() {
    Game game;

    while (!game.window.ShouldClose()) {
        game.doLogic();
        game.drawAll();
    }

    return 0;
}
