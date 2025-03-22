#include "Wall.h"
#include "Game.h"

Wall::Wall(int x, int y, bool breakable, bool camouflaged) : breakable(breakable), camouflaged(camouflaged), hitPoints(2) {
    rect.x = x;
    rect.y = y;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;
}

void Wall::render(SDL_Renderer* renderer, SDL_Texture* spriteSheet) const {
    SDL_Rect srcRect;
    if(breakable) {
        srcRect = {1052, 256, 32, 32};
    } else if(camouflaged) {
        srcRect = {1084, 288, 32, 32};
    } else{
        srcRect = {1052, 288, 32, 32};
    }
    SDL_RenderCopy(renderer, spriteSheet, &srcRect, &rect);
}

void Wall::takeDamage() {
    if (breakable) {
        hitPoints--;
    }
}

bool Wall::isDestroyed() const {
    return breakable && hitPoints == 0;
}
