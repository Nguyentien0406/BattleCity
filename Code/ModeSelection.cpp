#include "ModeSelection.h"
#include <iostream>
#include <SDL_image.h>
#include "Game.h"

using namespace std;
 // Khởi tạo với renderer
ModeSelection::ModeSelection(SDL_Renderer* renderer) : renderer(renderer), titleTexture(nullptr), selectedOption(0) {
    font= TTF_OpenFont("Font/Font1.ttf", 30);
    if(!font) {
        cerr << "Failed to load font: " << TTF_GetError() << endl;
        throw runtime_error("Font initialization failed");
    }
    SDL_Surface* surface = IMG_Load("C:\\SDL2\\BattleCity\\Ảnh\\assets.png");
    if (!surface) {
        cerr << "Failed to load title image: " << IMG_GetError() << endl;
        throw runtime_error("Image loading failed");
    }

    SDL_Rect srcRect= {28, 40, 188, 68};
    SDL_Surface* cropped= SDL_CreateRGBSurface(0, srcRect.w, srcRect.h, 32, 0, 0, 0, 0);
    SDL_BlitSurface(surface, &srcRect, cropped, nullptr);

    titleTexture= SDL_CreateTextureFromSurface(renderer, cropped);
    titleRect.w= srcRect.w* 3;
    titleRect.h= srcRect.h* 3;
    titleRect.x= (SCREEN_WIDTH- titleRect.w)/ 2;
    titleRect.y= 80;

    SDL_FreeSurface(cropped);
    SDL_FreeSurface(surface);
}
 // Hủy để dọn dẹp tài nguyên
ModeSelection::~ModeSelection() {
    if(font) TTF_CloseFont(font);
    if(titleTexture) SDL_DestroyTexture(titleTexture);
}
 // Hiển thị menu chọn chế độ
ModeSelection::Mode ModeSelection::ShowSelection() {
    bool selectionRunning = true;
    Mode action = BACK;
    while(selectionRunning) {
        HandleInput(selectionRunning);
        if(!selectionRunning) {
            action = static_cast<Mode>(selectedOption);
        }
        RenderSelection();
        SDL_Delay(16);
    }
    return action;
}
 // Vẽ menu lên màn hình
void ModeSelection::RenderSelection() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    if (titleTexture) {
        SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);
    }
    Uint32 ticks = SDL_GetTicks();
    bool isBlinking = (ticks / 500) % 2;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color red = {255, 0, 0, 255};

    SDL_Color color1 = (selectedOption == 0) ? (isBlinking ? red : white) : white;
    SDL_Color color2 = (selectedOption == 1) ? (isBlinking ? red : white) : white;

    SDL_Surface* option1Surface = TTF_RenderText_Solid(font, "1 PLAYER", color1);
    SDL_Texture* option1Texture = SDL_CreateTextureFromSurface(renderer, option1Surface);
    SDL_Rect option1Rect = {(SCREEN_WIDTH - option1Surface->w)/2, 350, option1Surface->w, option1Surface->h};
    SDL_Surface* option2Surface = TTF_RenderText_Solid(font, "2 PLAYERS", color2);
    SDL_Texture* option2Texture = SDL_CreateTextureFromSurface(renderer, option2Surface);
    SDL_Rect option2Rect = {(SCREEN_WIDTH - option2Surface->w)/2, 400, option2Surface->w, option2Surface->h};
    SDL_RenderCopy(renderer, option1Texture, nullptr, &option1Rect);
    SDL_RenderCopy(renderer, option2Texture, nullptr, &option2Rect);
    SDL_FreeSurface(option1Surface);
    SDL_DestroyTexture(option1Texture);
    SDL_FreeSurface(option2Surface);
    SDL_DestroyTexture(option2Texture);

    SDL_RenderPresent(renderer);
}
 // Xử lý đầu vào
void ModeSelection::HandleInput(bool& selectionRunning) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type== SDL_QUIT) {
            selectedOption= 2;
            selectionRunning= false;
        }
        else if (event.type== SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    selectedOption= 2;
                    selectionRunning= false;
                    break;
                case SDLK_UP:
                    selectedOption= (selectedOption - 1 + 2) % 2;
                    break;
                case SDLK_DOWN:
                    selectedOption= (selectedOption + 1) % 2;
                    break;
                case SDLK_RETURN:
                    selectionRunning= false;
                    break;
            }
        }
    }
}
