#include "Wall.h"
#include "Game.h"

Wall::Wall(int x, int y, bool breakable, bool camouflaged) : breakable(breakable), camouflaged(camouflaged), hitPoints(2), damageDirection(-1), halfDamaged(false) {
    rect.x = x;
    rect.y = y;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;
}

void Wall::render(SDL_Renderer* renderer, SDL_Texture* spriteSheet) const {
    SDL_Rect srcRect;
    if(breakable) {
        if (halfDamaged) {
            switch (damageDirection) {
                case 0:
                    srcRect = {1180, 256, 32, 16};
                    break;
                case 1:
                    srcRect = {1116, 272, 32, 16};
                    break;
                case 2:
                    srcRect = {1148, 256, 16, 32};
                    break;
                case 3:
                    srcRect = {1100, 256, 16, 32};
                    break;
                default:
                    srcRect = {1052, 288, 32, 32};
                    break;
            }
        } else {
            srcRect = {1052, 256, 32, 32};
        }
    } else if(camouflaged) {
        srcRect = {1084, 288, 32, 32};
        SDL_RenderCopy(renderer, spriteSheet, &srcRect, &rect);
    } else{
        srcRect = {1052, 288, 32, 32};
        SDL_RenderCopy(renderer, spriteSheet, &srcRect, &rect);
    }
    SDL_RenderCopy(renderer, spriteSheet, &srcRect, &rect);
}

void Wall::takeDamage(int direction) {
    if (breakable) {
        if (!halfDamaged) {
            halfDamaged = true;
            damageDirection = direction;
            hitPoints--;
            switch (direction) {
                case 0:
                    rect.h /= 2;
                    break;
                case 1:
                    rect.y += TILE_SIZE / 2;
                    rect.h /= 2;
                    break;
                case 2:
                    rect.w /= 2;
                    break;
                case 3:
                    rect.x += TILE_SIZE / 2;
                    rect.w /= 2;
                    break;
            }
        } else {
            hitPoints--;
        }
    }
}

bool Wall::isDestroyed() const {
    return breakable && hitPoints == 0;
}
