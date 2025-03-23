#ifndef MAINMENU_H
#define MAINMENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class MainMenu {
public:
    MainMenu(SDL_Renderer* renderer);
    ~MainMenu();
    bool show();

private:
    SDL_Renderer* renderer;
    TTF_Font* fontTitle;
    TTF_Font* fontStart;
    SDL_Texture* titleTexture;
    SDL_Texture* startTexture;
    SDL_Rect titleRect;
    SDL_Rect startRect;

    void render();
    void handleEvents(bool &running);
};

#endif
