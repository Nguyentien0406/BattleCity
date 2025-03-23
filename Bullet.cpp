#include "Bullet.h"
#include "Game.h"

using namespace std;

Bullet::Bullet(int x, int y, int dir, bool fromPlayer) : speed(BULLET_SPEED), direction(dir), collided(false), isFromPlayer(fromPlayer) {
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
                wall.takeDamage(direction);
                if (wall.isDestroyed()) {
                    game.removeWall(wall.getRect().x, wall.getRect().y);
                }
                collided = true;
                return;
            } else if(wall.isCamouflaged()) {
                continue;
            } else{
                collided = true;
                return;
            }
        }
    }

    SDL_Rect playerRect = player.getRect();
    if (!isFromPlayer && SDL_HasIntersection(&rect, &playerRect)) {
        collided = true;
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
            return;
            }
        }
    }

    if (rect.x <= TILE_SIZE || rect.x + BULLET_SIZE >= SCREEN_WIDTH - TILE_SIZE ||
        rect.y <= TILE_SIZE || rect.y + BULLET_SIZE >= SCREEN_HEIGHT - TILE_SIZE) {
        collided = true;
    }
}

bool Bullet::hasCollided() const {
    return collided;
}
