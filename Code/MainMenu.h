#ifndef MAINMENU_H
#define MAINMENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>

class MainMenu {
public:
    enum MenuAction { START_GAME, CONTINUE_GAME, EXIT_GAME };   // Các hành động có thể chọn trong menu
    MainMenu(SDL_Renderer* renderer);
    ~MainMenu();
    MenuAction ShowMenu(bool saveFile);

private:
    SDL_Renderer* renderer;
    SDL_Texture* titleTexture;
    SDL_Rect titleRect;
    TTF_Font* fontMenu;
    int selectedOption;
    void RenderMenu(bool saveFile);
    void HandleInput(bool& menuRunning, bool saveFile);
};

#endif
