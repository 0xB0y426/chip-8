#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL2/SDL.h>
#include <string>

class Platform {
public:
    Platform(const std::string &title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~Platform();
    void Update(void const *buffer, int pitch);
    bool ProcessInput(uint8_t *keys);

private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    SDL_Texture *texture = nullptr;
};

#endif

