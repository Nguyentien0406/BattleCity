#include "Bullet.h"
#include "Game.h"

using namespace std;

Bullet::Bullet(int x, int y, int dir, bool fromPlayer) : speed(BULLET_SPEED), direction(dir), offScreen(false), collided(false), isFromPlayer(fromPlayer) {
    rect.x = x;
    rect.y = y;
    rect.w = BULLET_SIZE;
    rect.h = BULLET_SIZE;
}

void Bullet::render(SDL_Renderer* renderer, SDL_Texture* spriteSheet) const{
    SDL_Rect srcRect;
    switch (direction) {
        case 0:
            srcRect = {1318, 406, 16, 20};
            break;
        case 1:
            srcRect = {1382, 406, 16, 20};
            break;
        case 2:
            srcRect = {1348, 406, 18, 16};
            break;
        case 3:
            srcRect = {1410, 406, 18, 16};
            break;
    }
    SDL_RenderCopy(renderer, spriteSheet, &srcRect, &rect);
}

void Bullet::update(vector<Wall>& walls, PlayerTank& player, vector<EnemyTank>& enemies, Game &game) {
    switch (direction) {
        case 0: rect.y -= speed; break;
        case 1: rect.y += speed; break;
        case 2: rect.x -= speed; break;
        case 3: rect.x += speed; break;
    }

    for (Wall& wall : walls) {
        if (SDL_HasIntersection(&rect, &wall.getRect())) {
            if (wall.isBreakable()) {
                wall.takeDamage();
                if (wall.isDestroyed()) {
                    game.removeWall(wall.getRect().x, wall.getRect().y);
                }
                collided = true;
                offScreen = true;
                return;
            } else if(wall.isCamouflaged()) {
                continue;
            } else{
                collided = true;
                offScreen = true;
                return;
            }
        }
    }

    SDL_Rect playerRect = player.getRect();
    if (!isFromPlayer && SDL_HasIntersection(&rect, &playerRect)) {
        collided = true;
        offScreen = true;
        player.takeDamage(game);
        return;
    }
    if (isFromPlayer) {
        for (auto it = enemies.begin(); it != enemies.end(); ++it) {
            SDL_Rect enemyRect = it->getRect();
            if (SDL_HasIntersection(&rect, &enemyRect)) {
                it->takeDamage();
                if (it->isDestroyed()) {
                    game.addExplosion(it->getRect().x, it->getRect().y);
                    game.playExplosionSound();
                    enemies.erase(it);
                }
            collided = true;
            offScreen = true;
            return;
            }
        }
    }

    if (rect.x < 0 || rect.x > SCREEN_WIDTH || rect.y < 0 || rect.y > SCREEN_HEIGHT) {
        offScreen = true;
    }
}

bool Bullet::isOffScreen() const {
    return offScreen;
}

bool Bullet::hasCollided() const {
    return collided;
}
