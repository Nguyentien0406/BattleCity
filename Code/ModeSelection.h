#ifndef MODE_SELECTION_H
#define MODE_SELECTION_H

#include <SDL.h>
#include <SDL_ttf.h>

class ModeSelection{
public:
    enum Mode { ONE_PLAYER, TWO_PLAYERS, BACK };
    ModeSelection(SDL_Renderer* renderer);
    ~ModeSelection();
    Mode ShowSelection();

private:
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Texture* titleTexture;
    SDL_Rect titleRect;
    int selectedOption;
    void RenderSelection();
    void HandleInput(bool& selectionRunning);
};

#endif
