#include "chip8.h"
#include "includes/platform.h"
#include <chrono>
#include <thread>
#include <iostream>

int main(int argc, char **argv) {
    if(argc != 2) {
        std::cerr << "Usage: " << argv[0] << " ROM\n";
        return 1;
    }

    const int videoScale = 10;
    Platform platform("CHIP-8", Chip8::VIDEO_WIDTH*videoScale, Chip8::VIDEO_HEIGHT*videoScale, Chip8::VIDEO_WIDTH, Chip8::VIDEO_HEIGHT);

    Chip8 chip8;
    chip8.LoadROM(argv[1]);

    int pitch = sizeof(chip8.video[0]) * Chip8::VIDEO_WIDTH;
    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    double cycleDelay = 1000.0 / 500.0; 

    bool quit = false;
    while(!quit) {
        quit = platform.ProcessInput(chip8.keypad.data());
        auto currentTime = std::chrono::high_resolution_clock::now();
        double dt = std::chrono::duration<double, std::milli>(currentTime-lastCycleTime).count();
        if(dt > cycleDelay) {
            chip8.Cycle();
            if(chip8.drawFlag) {
                platform.Update(chip8.video.data(), pitch);
                chip8.drawFlag = false;
            }
            lastCycleTime = currentTime;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return 0;
}

