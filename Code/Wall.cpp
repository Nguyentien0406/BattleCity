#include "Wall.h"
#include "Game.h"
 // Khởi tạo tường
Wall::Wall(int x, int y, bool breakable, bool camouflaged) : breakable(breakable), camouflaged(camouflaged), hitPoints(2), damageDirection(-1), halfDamaged(false) {
    rect.x= x;
    rect.y= y;
    rect.w= TILE_SIZE;
    rect.h= TILE_SIZE;
}
 // Vẽ tường
void Wall::render(SDL_Renderer* renderer, SDL_Texture* spriteSheet) const {
    SDL_Rect srcRect;
    if(breakable) {
        if(halfDamaged) {
            switch(damageDirection) {
                case 0:
                    srcRect= {136, 414, 8, 4};
                    break;
                case 1:
                    srcRect= {145, 418, 8, 4};
                    break;
                case 2:
                    srcRect= {154, 414, 4, 8};
                    break;
                case 3:
                    srcRect= {131, 414, 4, 8};
                    break;
                default:
                    srcRect= {118, 414, 8, 8};
                    break;
            }
        } else{
            srcRect= {118, 414, 8, 8};
        }
    } else if(camouflaged) {
        srcRect= {136, 423, 8, 8};
        SDL_RenderCopy(renderer, spriteSheet, &srcRect, &rect);
    } else{
        srcRect= {109, 414, 8, 8};
        SDL_RenderCopy(renderer, spriteSheet, &srcRect, &rect);
    }
    SDL_RenderCopy(renderer, spriteSheet, &srcRect, &rect);
}
 // Nhận sát thương từ 1 hướng
void Wall::takeDamage(int direction) {
    if(breakable) {
        if(!halfDamaged) {
            halfDamaged= true;
            damageDirection= direction;
            hitPoints--;
            switch(direction) {
                case 0:
                    rect.h/= 2;
                    break;
                case 1:
                    rect.y+= TILE_SIZE/ 2;
                    rect.h/= 2;
                    break;
                case 2:
                    rect.w/= 2;
                    break;
                case 3:
                    rect.x+= TILE_SIZE/ 2;
                    rect.w/= 2;
                    break;
            }
        } else{
            hitPoints--;
        }
    }
}
 // Kiểm tra bị phá hủy
bool Wall::isDestroyed() const {
    return breakable && hitPoints== 0;
}
