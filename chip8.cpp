#include "chip8.h"
#include <fstream>
#include <cstring>
#include <stdexcept>
#include <chrono>

namespace {
const uint8_t fontset[80] = {
 0xF0,0x90,0x90,0x90,0xF0, 0x20,0x60,0x20,0x20,0x70,
 0xF0,0x10,0xF0,0x80,0xF0, 0xF0,0x10,0xF0,0x10,0xF0,
 0x90,0x90,0xF0,0x10,0x10, 0xF0,0x80,0xF0,0x10,0xF0,
 0xF0,0x80,0xF0,0x90,0xF0, 0xF0,0x10,0x20,0x40,0x40,
 0xF0,0x90,0xF0,0x90,0xF0, 0xF0,0x90,0xF0,0x10,0xF0,
 0xF0,0x90,0xF0,0x90,0x90, 0xE0,0x90,0xE0,0x90,0xE0,
 0xF0,0x80,0x80,0x80,0xF0, 0xE0,0x90,0x90,0x90,0xE0,
 0xF0,0x80,0xF0,0x80,0xF0, 0xF0,0x80,0xF0,0x80,0x80
};
}

Chip8::Chip8() : rng(std::random_device{}()) {
    pc = 0x200;
    LoadFontset();
    InitializeTables();
}

void Chip8::LoadFontset() {
    for (int i=0;i<80;i++) memory[i] = fontset[i];
}

void Chip8::LoadROM(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if(!file) throw std::runtime_error("ERROR: Load the ROM");
    std::streampos size = file.tellg();
    std::vector<char> buffer(size);
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), size);
    for (size_t i=0;i<buffer.size();++i) memory[0x200+i] = buffer[i];
}

void Chip8::Cycle() {
    opcode = (memory[pc] << 8u) | memory[pc+1];
    pc += 2;
    (this->*table[(opcode & 0xF000u) >> 12u])();
    if (delayTimer > 0) --delayTimer;
    if (soundTimer > 0) --soundTimer;
}

void Chip8::InitializeTables() {
    for(auto &f : table) f = &Chip8::OP_NULL;
    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::OP_1nnn;
    table[0x6] = &Chip8::OP_6xkk;
    table[0x7] = &Chip8::OP_7xkk;
    table[0xA] = &Chip8::OP_Annn;
    table[0xD] = &Chip8::OP_Dxyn;
}

void Chip8::OP_NULL() {}
void Chip8::Table0() {
    switch(opcode & 0x00FFu) {
        case 0xE0: OP_00E0(); break;
        case 0xEE: OP_00EE(); break;
    }
}
void Chip8::OP_00E0() { video.fill(0); drawFlag = true; }
void Chip8::OP_00EE() { --sp; pc = stack[sp]; }
void Chip8::OP_1nnn() { pc = opcode & 0x0FFFu; }
void Chip8::OP_6xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    V[Vx] = opcode & 0x00FFu;
}
void Chip8::OP_7xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    V[Vx] += opcode & 0x00FFu;
}
void Chip8::OP_Annn() { I = opcode & 0x0FFFu; }

void Chip8::OP_Dxyn() {
    uint8_t Vx = V[(opcode & 0x0F00u) >> 8u] % VIDEO_WIDTH;
    uint8_t Vy = V[(opcode & 0x00F0u) >> 4u] % VIDEO_HEIGHT;
    uint8_t height = opcode & 0x000Fu;
    V[0xF] = 0;
    for(int row=0;row<height;++row) {
        uint8_t spriteByte = memory[I+row];
        for(int col=0;col<8;++col) {
            uint8_t spritePixel = spriteByte & (0x80u >> col);
            uint32_t &screenPixel = video[(Vy+row)*VIDEO_WIDTH+(Vx+col)];
            if(spritePixel) {
                if(screenPixel == 0xFFFFFFFF) V[0xF]=1;
                screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
    drawFlag = true;
}

