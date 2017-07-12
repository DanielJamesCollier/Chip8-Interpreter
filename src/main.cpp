#include "Chip8.hpp"
#include "SDL_Module.hpp"

#include <chrono>
#include <thread>
#include <iostream>

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false); 
    std::string rom("INVADERS");
    
    // allow the user to specify a ROM as a command line argument    
    if (argc == 2) {
        rom = argv[1]; 
    } else {
        std::cout << "\nTo select a ROM pass it as a command line argument as follows\nChip8-Interpreter PONG\nelse the default is Space Invaders\n";
    } 
    
    Chip8 chip8(rom);
    
    using namespace std::chrono_literals;

    SDL_Module sdl(64, 32, 10);
    
    while(sdl.event_loop(chip8.keys)) {
        chip8.tick();    
       
        if (chip8.draw_flag) {
            sdl.draw(chip8.getPixels());
            chip8.draw_flag = false;
        }
        std::this_thread::sleep_for(400us);

    } 
    
    return 0;
}
