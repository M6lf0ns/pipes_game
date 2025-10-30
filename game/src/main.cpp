#include "Game.h"
#include <iostream>


int main() {
    try {
        gl3::Game spaceBattle(1280, 720, "Space Battle");
        spaceBattle.run();
        std::cout <<"Spaceship created";
    }
    catch(const std::exception &e) {
        std::cerr << "Unhandled exception: " << e.what() << std::endl;
    }
}