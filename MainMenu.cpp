#include "MainMenu.h"
#include <SDL_image.h>
#include <iostream>

using namespace std;

MainMenu::MainMenu(SDL_Renderer* renderer) : renderer(renderer) {
    TTF_Init();
    fontTitle = TTF_OpenFont("Font/Font1.ttf", 64);
    fontStart = TTF_OpenFont("Font/Font1.ttf", 32);

    if (!fontTitle || !fontStart) {
        cerr << "Lỗi tải font: " << TTF_GetError() << "\n";
    }

    SDL_Color white = {255, 255, 255, 255};


    SDL_Surface* titleSurface = TTF_RenderText_Solid(fontTitle, "BattleCity", white);
    titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);


    SDL_Surface* startSurface = TTF_RenderText_Solid(fontStart, "Start", white);
    startTexture = SDL_CreateTextureFromSurface(renderer, startSurface);


    titleRect.w = titleSurface->w;
    titleRect.h = titleSurface->h;
    titleRect.x = (800 - titleRect.w) / 2;
    titleRect.y = 150;

    startRect.w = startSurface->w;
    startRect.h = startSurface->h;
    startRect.x = (800 - startRect.w) / 2;
    startRect.y = 300;

    SDL_FreeSurface(titleSurface);
    SDL_FreeSurface(startSurface);
}

MainMenu::~MainMenu() {
    SDL_DestroyTexture(titleTexture);
    SDL_DestroyTexture(startTexture);
    TTF_CloseFont(fontTitle);
    TTF_CloseFont(fontStart);
    TTF_Quit();
}

void MainMenu::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);
    SDL_RenderCopy(renderer, startTexture, nullptr, &startRect);

    SDL_RenderPresent(renderer);
}

void MainMenu::handleEvents(bool &running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
            running = false;
        }
    }
}

bool MainMenu::show() {
    bool running = true;
    while (running) {
        handleEvents(running);
        render();
    }
    return true;
}
