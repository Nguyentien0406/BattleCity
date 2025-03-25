#include "MainMenu.h"
#include "Game.h"

using namespace std;

MainMenu::MainMenu(SDL_Renderer* renderer) : renderer(renderer),fontTitle(nullptr), fontMenu(nullptr), selectedOption(0) {
    fontTitle = TTF_OpenFont("Font/Font1.ttf", 64);
    fontMenu = TTF_OpenFont("Font/Font1.ttf", 30);
    if (!fontTitle || !fontMenu) {
        cerr << "Failed to load fonts: " << TTF_GetError() << endl;
        throw runtime_error("Font initialization failed");
    }
}

MainMenu::~MainMenu() {
    if (fontTitle) TTF_CloseFont(fontTitle);
    if (fontMenu) TTF_CloseFont(fontMenu);
}

MainMenu::MenuAction MainMenu::ShowMenu(bool saveFile) {
    bool menuRunning = true;
    MenuAction action = EXIT_GAME;
    while (menuRunning) {
        HandleInput(menuRunning, saveFile);
        if (!menuRunning) {
            action = (selectedOption == 0) ? START_GAME : CONTINUE_GAME;
        }
        RenderMenu(saveFile);
        SDL_Delay(16);
    }
    return action;
}

void MainMenu::RenderMenu(bool saveFile) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* titleSurface = TTF_RenderText_Solid(fontTitle, "BATTLE CITY", white);
    SDL_Texture* titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);
    SDL_Rect titleRect = {(SCREEN_WIDTH - titleSurface->w)/2, 150, titleSurface->w, titleSurface->h};
    SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);
    SDL_Color red = {255, 0, 0, 255};
    SDL_Color startColor = (selectedOption == 0) ? red : white;
    SDL_Surface* startSurface = TTF_RenderText_Solid(fontMenu, "START", startColor);
    SDL_Texture* startTexture = SDL_CreateTextureFromSurface(renderer, startSurface);
    SDL_Rect startRect = {(SCREEN_WIDTH - startSurface->w)/2, 330, startSurface->w, startSurface->h};
    SDL_RenderCopy(renderer, startTexture, nullptr, &startRect);
    if (saveFile) {
        SDL_Color continueColor = (selectedOption == 1) ? red : white;
        SDL_Surface* continueSurface = TTF_RenderText_Solid(fontMenu, "CONTINUE", continueColor);
        SDL_Texture* continueTexture = SDL_CreateTextureFromSurface(renderer, continueSurface);
        SDL_Rect continueRect = {(SCREEN_WIDTH - continueSurface->w)/2, 400, continueSurface->w, continueSurface->h};
        SDL_RenderCopy(renderer, continueTexture, nullptr, &continueRect);
        SDL_FreeSurface(continueSurface);
        SDL_DestroyTexture(continueTexture);
    }
    SDL_FreeSurface(titleSurface);
    SDL_FreeSurface(startSurface);
    SDL_DestroyTexture(titleTexture);
    SDL_DestroyTexture(startTexture);

    SDL_RenderPresent(renderer);
}

void MainMenu::HandleInput(bool& menuRunning, bool hasSaveFile) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT ||
           (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
            menuRunning = false;
            exit(0);
        }
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                case SDLK_DOWN:
                    if (hasSaveFile) selectedOption = 1 - selectedOption;
                    break;
                case SDLK_RETURN:
                    menuRunning = false;
                    break;
            }
        }
    }
}
