#include "Game.h"
#include "PlayerTank.h"
#include <iostream>

using namespace std;
// Khởi tạo
PlayerTank::PlayerTank(int x, int y) : speed(1), baseSpeed(1), direction(0), health(3), currentFrame(0), lastFrameTime(SDL_GetTicks()),
 isMoving(false), boostedMoves(0), alive(true), shootCooldown(500), lastShotTime(SDL_GetTicks()) {
    rect.x= x;
    rect.y= y;
    rect.w= TILE_SIZE;
    rect.h= TILE_SIZE;
}
// Vẽ tank
void PlayerTank::render(SDL_Renderer* renderer, SDL_Texture* spriteSheet, bool isOtherPlayer) {
    if (!alive) return;
    SDL_Rect srcRect;
    Uint32 currentTime= SDL_GetTicks();
    // Cập nhật frame animation nếu đang di chuyển
    if(isMoving && currentTime- lastFrameTime > 10) {
        currentFrame= 1- currentFrame;
        lastFrameTime= currentTime;
    }
    // Chọn sprite phù hợp dựa trên hướng và loại người chơi
    if(!isOtherPlayer) {
        switch(direction) {
            case 0:
                srcRect= currentFrame== 0 ? SDL_Rect{37, 228, 13, 13} : SDL_Rect{37, 245, 13, 13};
                break;
            case 1:
                srcRect= currentFrame== 0 ? SDL_Rect{20, 228, 13, 13} : SDL_Rect{20, 245, 13, 13};
                break;
            case 2:
                srcRect= currentFrame== 0 ? SDL_Rect{52, 228, 13, 13} : SDL_Rect{52, 245, 13, 13};
                break;
            case 3:
                srcRect= currentFrame== 0 ? SDL_Rect{3, 228, 13, 13} : SDL_Rect{3, 245, 13, 13};
                break;
        }
    } else {
        switch(direction) {
            case 0:
                srcRect= currentFrame== 0 ? SDL_Rect{37, 300, 13, 13} : SDL_Rect{37, 317, 13, 13};
                break;
            case 1:
                srcRect= currentFrame== 0 ? SDL_Rect{20, 300, 13, 13} : SDL_Rect{20, 317, 13, 13};
                break;
            case 2:
                srcRect= currentFrame== 0 ? SDL_Rect{52, 300, 13, 13} : SDL_Rect{52, 317, 13, 13};
                break;
            case 3:
                srcRect= currentFrame== 0 ? SDL_Rect{3, 300, 13, 13} : SDL_Rect{3, 317, 13, 13};
                break;
        }
    }
    SDL_RenderCopy(renderer, spriteSheet, &srcRect, &rect);
}
 // Di chuyển tank
void PlayerTank::move(int dx, int dy, const vector<Wall>& walls, const vector<EnemyTank>& enemies, const vector<BossTank>& bosses, const PlayerTank& otherPlayer) {
    if (!alive) return;
    // Tăng tốc nếu đang trong trạng thái boosted
    speed= (boostedMoves> 0) ? baseSpeed* 10 : baseSpeed;
    // Tính toán vị trí mới
    int newX= rect.x+ dx* speed;
    int newY= rect.y+ dy* speed;
    SDL_Rect newRect= { newX, newY, rect.w, rect.h };
    // Kiểm tra va chạm với tường
    for(const Wall& wall : walls) {
        if(!wall.isCamouflaged() && SDL_HasIntersection(&newRect, &wall.getRect())) {
            return;
        }
    }
    // Kiểm tra va chạm với xe tăng địch
    for(const EnemyTank& enemy : enemies) {
            SDL_Rect enemyRect= enemy.getRect();
            if(SDL_HasIntersection(&newRect, &enemyRect)) {
                return;
            }
    }
    // Kiểm tra va chạm với boss
    for(const BossTank& boss : bosses) {
            SDL_Rect bossRect= boss.getRect();
            if(SDL_HasIntersection(&newRect, &bossRect)) {
                return;
            }
    }
    // Kiểm tra va chạm với người chơi khác (nếu có)
    if(otherPlayer.isAlive()) {
        SDL_Rect otherPlayerRect= otherPlayer.getRect();
        if (SDL_HasIntersection(&newRect, &otherPlayerRect)) {
            return;
        }
    }

    rect.x= newX;
    rect.y= newY;

    if(dx== 1) direction= 3;
    else if(dx== -1) direction= 2;
    else if(dy== 1) direction= 1;
    else if(dy== -1) direction= 0;
    // Giảm thời gian boosted nếu có
    if(boostedMoves> 0) boostedMoves--;
    // Giới hạn không cho đi ra khỏi màn hình
    if(rect.x< TILE_SIZE) rect.x= TILE_SIZE;
    if(rect.x> SCREEN_WIDTH- TILE_SIZE- TILE_SIZE) rect.x= SCREEN_WIDTH- TILE_SIZE- TILE_SIZE;
    if(rect.y< TILE_SIZE) rect.y= TILE_SIZE;
    if(rect.y> SCREEN_HEIGHT- TILE_SIZE- TILE_SIZE) rect.y= SCREEN_HEIGHT- TILE_SIZE- TILE_SIZE;
}
 // Bắn đạn
void PlayerTank::shoot(Game &game) {
    if (!alive) return;
    // Kiểm tra thời gian chờ giữa các lần bắn
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastShotTime < shootCooldown) {
        return;
    }
    lastShotTime = currentTime;
    // Tính toán vị trí bắn đạn dựa trên hướng
    int bulletX= rect.x;
    int bulletY= rect.y;
    int offset= 10;

    switch(direction) {
        case 0:
            bulletX+= rect.w/ 2 - BULLET_SIZE/ 2;
            bulletY-= BULLET_SIZE- offset;
            break;
        case 1:
            bulletX+= rect.w/ 2 - BULLET_SIZE/ 2;
            bulletY+= rect.h- offset;
            break;
        case 2:
            bulletX-= BULLET_SIZE- offset;
            bulletY+= rect.h/ 2 - BULLET_SIZE/ 2;
            break;
        case 3:
            bulletX+= rect.w- offset;
            bulletY+= rect.h/ 2 - BULLET_SIZE/ 2;
            break;
    }

    bullets.emplace_back(bulletX, bulletY, direction, true, false);
    game.playShootSound();
}
// Getter/Setter
vector<Bullet>& PlayerTank::getBullets() {
    return bullets;
}

const SDL_Rect PlayerTank::getRect() const {
    return rect;
}
 // Nhận sát thương
void PlayerTank::takeDamage() {
    if (!alive) return;
    health--;
}
 // Kiểm tra tank bị phá hủy
bool PlayerTank::isDestroyed() const {
    return health <= 0;
}
