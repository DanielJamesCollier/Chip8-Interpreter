#include "Chip8.hpp"
#include "SDL_Module.hpp"

#include <chrono>
#include <thread>
#include <iostream>

// todo
// add command line argument support
// fix the timestep
// add beeping noise
// add CRT sim shader

int main(int argc, char* argv[]) {
    
    std::cout <<  argv[0] << std::endl;

    Chip8 chip8("INVADERS");
    
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
