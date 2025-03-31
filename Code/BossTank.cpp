#include "BossTank.h"
#include "Game.h"
using namespace std;
 // Constructor
BossTank::BossTank(int x, int y) : speed(1), direction(rand() % 4), health(20), currentFrame(0), lastFrameTime(SDL_GetTicks()),
 isMoving(true), shootCooldown(2000), lastShotTime(SDL_GetTicks()), changeDirectionCooldown(0) {
    rect.x= x;
    rect.y= y;
    rect.w= TILE_SIZE* 2;
    rect.h= TILE_SIZE* 2;
}
 // Vẽ boss
void BossTank::render(SDL_Renderer* renderer, SDL_Texture* spriteSheet) {
    SDL_Rect srcRect;
    Uint32 currentTime= SDL_GetTicks();
    if(currentTime- lastFrameTime> 10&& isMoving) {
        currentFrame= 1 - currentFrame;
        lastFrameTime= currentTime;
    }
    switch(direction) {
        case 0:
            srcRect= currentFrame== 0 ? SDL_Rect{110, 228, 13, 15} : SDL_Rect{110, 245, 13, 15};
            break;
        case 1:
            srcRect= currentFrame== 0 ? SDL_Rect{95, 228, 13, 15} : SDL_Rect{95, 245, 13, 15};
            break;
        case 2:
            srcRect= currentFrame== 0 ? SDL_Rect{125, 228, 15, 13} : SDL_Rect{125, 245, 15, 13};
            break;
        case 3:
            srcRect= currentFrame== 0 ? SDL_Rect{78, 228, 15, 13} : SDL_Rect{78, 245, 15, 13};
            break;
    }
    SDL_RenderCopy(renderer, spriteSheet, &srcRect, &rect);
}
 // Di chuyển AI
void BossTank::move(const vector<Wall>& walls, const PlayerTank& player, const PlayerTank& otherPlayer, const vector<EnemyTank>& enemies, const vector<BossTank>& bosses) {
    if(changeDirectionCooldown<= 0) {
        int newDirection;
        do{
            newDirection= rand()% 4;
        } while(newDirection== direction);
        direction= newDirection;
        changeDirectionCooldown= 300+ rand()% 100;
    } else{
        changeDirectionCooldown--;
    }

    int dx= 0, dy= 0;
    switch(direction) {
        case 0: dy= -1; break;
        case 1: dy= 1; break;
        case 2: dx= -1; break;
        case 3: dx= 1; break;
    }

    int newX= rect.x+ dx* speed;
    int newY= rect.y+ dy* speed;
    SDL_Rect newRect= {newX, newY, rect.w, rect.h};

    for(const Wall& wall : walls) {
        if(SDL_HasIntersection(&newRect, &wall.getRect())) {
            isMoving= false;
            direction= rand() % 4;
            return;
        }
    }

    if(player.isAlive()) {
        SDL_Rect playerRect= player.getRect();
        if(SDL_HasIntersection(&newRect, &playerRect)) {
            isMoving= false;
            direction= rand() % 4;
            return;
        }
    }

    if(otherPlayer.isAlive()) {
        SDL_Rect otherPlayerRect= otherPlayer.getRect();
        if(SDL_HasIntersection(&newRect, &otherPlayerRect)) {
            isMoving= false;
            direction= rand() % 4;
            return;
        }
    }

    for(const EnemyTank& enemy : enemies) {
        SDL_Rect enemyRect= enemy.getRect();
        if(SDL_HasIntersection(&newRect, &enemyRect)) {
            isMoving= false;
            direction= rand() % 4;
            return;
        }
    }

    for(const BossTank& boss : bosses) {
        if(&boss!= this) {
            SDL_Rect bossRect= boss.getRect();
            if(SDL_HasIntersection(&newRect, &bossRect)) {
                isMoving= false;
                direction= rand() % 4;
                return;
            }
        }
    }

    if(newX< TILE_SIZE || newX+ rect.w> SCREEN_WIDTH- TILE_SIZE ||
        newY< TILE_SIZE || newY+ rect.h> SCREEN_HEIGHT- TILE_SIZE) {
        isMoving= false;
        direction= rand() % 4;
        return;
    }
    isMoving= true;

    rect.x= newX;
    rect.y= newY;
}
 // Bắn đạn
void BossTank::shoot() {
    Uint32 currentTime = SDL_GetTicks();
    if (currentTime - lastShotTime < shootCooldown) {
        return;
    }
    lastShotTime = currentTime;
    int bulletX= rect.x;
    int bulletY= rect.y;

    switch(direction) {
        case 0:
            bulletX+= rect.w/ 2 - BULLET_SIZE/ 2 -2;
            bulletY-= BULLET_SIZE;
            break;
        case 1:
            bulletX+= rect.w/ 2 - BULLET_SIZE/ 2 -2;
            bulletY+= rect.h;
            break;
        case 2:
            bulletX-= BULLET_SIZE;
            bulletY+= rect.h/ 2 - BULLET_SIZE/ 2 -2;
            break;
        case 3:
            bulletX+= rect.w;
            bulletY+= rect.h/ 2 - BULLET_SIZE/ 2 -2;
            break;
    }

    bullets.emplace_back(bulletX, bulletY, direction, false, true);
}
 // Cập nhật đạn
void BossTank::updateBullets(vector<Wall>& walls, PlayerTank& player, PlayerTank& otherPlayer, vector<EnemyTank>& enemies, vector<BossTank>& bosses, Game &game) {
    for (Bullet& bullet : bullets) {
        bullet.update(walls, player, otherPlayer, enemies, bosses, game);
    }

    bullets.erase(remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b) { return b.hasCollided(); }),
        bullets.end());
}
 // Lấy danh sách đạn
vector<Bullet>& BossTank::getBullets() {
    return bullets;
}
 // Lấy hitbox
const SDL_Rect BossTank::getRect() const {
    return rect;
}
 // Nhận sát thương
void BossTank::takeDamage() {
    health--;
}
 // Kiểm tra bị phá hủy
bool BossTank::isDestroyed() const {
    return health <= 0;
}
