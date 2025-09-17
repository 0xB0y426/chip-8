#ifndef CHIP8_H
#define CHIP8_H

#include <cstdint>
#include <array>
#include <random>
#include <string>

class Chip8 {
public:
    static const unsigned int VIDEO_WIDTH = 64;
    static const unsigned int VIDEO_HEIGHT = 32;

    Chip8();
    void LoadROM(const std::string &filename);
    void Cycle();

    std::array<uint8_t, 16> keypad{};
    std::array<uint32_t, VIDEO_WIDTH * VIDEO_HEIGHT> video{};
    bool drawFlag = false;

private:
    std::array<uint8_t, 4096> memory{};
    std::array<uint8_t, 16> V{};
    uint16_t I = 0;
    uint16_t pc = 0x200;
    uint16_t opcode = 0;
    uint8_t delayTimer = 0;
    uint8_t soundTimer = 0;
    std::array<uint16_t, 16> stack{};
    uint8_t sp = 0;
    std::mt19937 rng;

    void Table0();
    void Table8();
    void TableE();
    void TableF();

    void OP_NULL();

    void OP_00E0(); void OP_00EE();
    void OP_1nnn(); void OP_2nnn();
    void OP_3xkk(); void OP_4xkk();
    void OP_5xy0(); void OP_6xkk();
    void OP_7xkk(); void OP_8xy0();
    void OP_8xy1(); void OP_8xy2();
    void OP_8xy3(); void OP_8xy4();
    void OP_8xy5(); void OP_8xy6();
    void OP_8xy7(); void OP_8xyE();
    void OP_9xy0(); void OP_Annn();
    void OP_Bnnn(); void OP_Cxkk();
    void OP_Dxyn(); void OP_Ex9E();
    void OP_ExA1(); void OP_Fx07();
    void OP_Fx0A(); void OP_Fx15();
    void OP_Fx18(); void OP_Fx1E();
    void OP_Fx29(); void OP_Fx33();
    void OP_Fx55(); void OP_Fx65();

    typedef void (Chip8::*Chip8Func)();
    Chip8Func table[0xF + 1]{};
    Chip8Func table0[0xE + 1]{};
    Chip8Func table8[0xE + 1]{};
    Chip8Func tableE[0xE + 1]{};
    Chip8Func tableF[0x65 + 1]{};

    void InitializeTables();
    void LoadFontset();
};

#endif

