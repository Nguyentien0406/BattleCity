#include "MainMenu.h"
#include "Game.h"

using namespace std;
 // Hàm khởi tạo, nhận renderer từ SDL
MainMenu::MainMenu(SDL_Renderer* renderer) : renderer(renderer), fontMenu(nullptr), selectedOption(0) {
    // Tải font cho menu
    fontMenu = TTF_OpenFont("Font/Font1.ttf", 30);
    if (!fontMenu) {
        cerr << "Failed to load fonts: " << TTF_GetError() << endl;
        throw runtime_error("Font initialization failed");
    }
    // Tải và xử lý hình ảnh tiêu đề
    SDL_Surface* surface = IMG_Load("C:\\SDL2\\BattleCity\\Ảnh\\assets.png");
    if (!surface) {
        cerr << "Failed to load title image: " << IMG_GetError() << endl;
        throw runtime_error("Image loading failed");
    }
    SDL_Rect srcRect = {28, 40, 188, 68};
    SDL_Surface* cropped = SDL_CreateRGBSurface(0, srcRect.w, srcRect.h, 32, 0, 0, 0, 0);
    SDL_BlitSurface(surface, &srcRect, cropped, NULL);

    titleTexture = SDL_CreateTextureFromSurface(renderer, cropped);
    titleRect.w = srcRect.w * 3;
    titleRect.h = srcRect.h * 3;
    titleRect.x = (SCREEN_WIDTH - titleRect.w) / 2;
    titleRect.y = 80;
    // Giải phóng bộ nhớ
    SDL_FreeSurface(cropped);
    SDL_FreeSurface(surface);
}
 // Hàm hủy để dọn dẹp tài nguyên
MainMenu::~MainMenu() {
    if (fontMenu) TTF_CloseFont(fontMenu);
    if (titleTexture) SDL_DestroyTexture(titleTexture);
}
 // Hiển thị menu và trả về lựa chọn của người chơi
MainMenu::MenuAction MainMenu::ShowMenu(bool saveFile) {
    bool menuRunning = true;
    MenuAction action = EXIT_GAME;
    // Vòng lặp chính của menu
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
 // Vẽ menu lên màn hình
void MainMenu::RenderMenu(bool saveFile) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    if (titleTexture) {
        SDL_RenderCopy(renderer, titleTexture, nullptr, &titleRect);
    }
    // Hiệu ứng nhấp nháy cho lựa chọn hiện tại
    Uint32 ticks = SDL_GetTicks();
    bool isBlinking = (ticks / 500) % 2;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color red = {255, 0, 0, 255};
    SDL_Color blinkColor = isBlinking ? red : white;
    // Vẽ nút START
    SDL_Color startColor = (selectedOption == 0) ? blinkColor : white;
    SDL_Surface* startSurface = TTF_RenderText_Solid(fontMenu, "START", startColor);
    SDL_Texture* startTexture = SDL_CreateTextureFromSurface(renderer, startSurface);
    SDL_Rect startRect = {(SCREEN_WIDTH - startSurface->w)/2, 350, startSurface->w, startSurface->h};
    SDL_RenderCopy(renderer, startTexture, nullptr, &startRect);
    // Vẽ nút CONTINUE nếu có file save
    if (saveFile) {
        SDL_Color continueColor = (selectedOption == 1) ? blinkColor : white;
        SDL_Surface* continueSurface = TTF_RenderText_Solid(fontMenu, "CONTINUE", continueColor);
        SDL_Texture* continueTexture = SDL_CreateTextureFromSurface(renderer, continueSurface);
        SDL_Rect continueRect = {(SCREEN_WIDTH - continueSurface->w)/2, 400, continueSurface->w, continueSurface->h};
        SDL_RenderCopy(renderer, continueTexture, nullptr, &continueRect);
        SDL_FreeSurface(continueSurface);
        SDL_DestroyTexture(continueTexture);
    }
    SDL_FreeSurface(startSurface);
    // Giải phóng bộ nhớ
    SDL_DestroyTexture(startTexture);
    SDL_RenderPresent(renderer);
}
 // Xử lý đầu vào từ người dùng
void MainMenu::HandleInput(bool& menuRunning, bool saveFile) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        // Thoát game nếu nhấn nút đóng cửa sổ hoặc ESC
        if (event.type == SDL_QUIT ||
           (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) {
            menuRunning = false;
            exit(0);
        }
        // Xử lý phím điều hướng
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:
                case SDLK_DOWN:
                    if (saveFile) selectedOption = 1 - selectedOption;
                    break;
                case SDLK_RETURN:
                    menuRunning = false;
                    break;
            }
        }
    }
}
