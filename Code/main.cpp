#include "Game.h"
#include <SDL_mixer.h>

using namespace std;

int main(int argc, char* argv[]) {
    Game game;  // Tạo đối tượng game
    if(!game.init()) {  // Khởi tạo game (cửa sổ, renderer, v.v.)
        return 1;  // Thoát nếu lỗi
    }
    game.run();  // Bắt đầu vòng lặp chính của game
    return 0;
}
