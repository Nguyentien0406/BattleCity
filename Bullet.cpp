#include "Bullet.h"
#include "Game.h"

using namespace std;

Bullet::Bullet(int x, int y, int dir, bool fromPlayer) : speed(BULLET_SPEED), direction(dir), offScreen(false), collided(false), isFromPlayer(fromPlayer) {
    rect.x = x;
    rect.y = y;
    rect.w = BULLET_SIZE;
    rect.h = BULLET_SIZE;
}

void Bullet::render(SDL_Renderer* renderer) const {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void Bullet::update(const vector<Wall>& walls, PlayerTank& player, vector<EnemyTank>& enemies, Game &game) {
    switch (direction) {
        case 0: rect.y -= speed; break; // Up
        case 1: rect.y += speed; break; // Down
        case 2: rect.x -= speed; break; // Left
        case 3: rect.x += speed; break; // Right
    }

    for (const Wall& wall : walls) {
        if (SDL_HasIntersection(&rect, &wall.getRect())) {
            collided = true;
            offScreen = true;
            return;
        }
    }

    SDL_Rect playerRect = player.getRect();
    if (!isFromPlayer && SDL_HasIntersection(&rect, &playerRect)) {
        collided = true;
        offScreen = true;
        player.takeDamage(game);
        return;
    }

    for (auto it = enemies.begin(); it != enemies.end(); ++it) {
        SDL_Rect enemyRect = it->getRect();
        if (SDL_HasIntersection(&rect, &enemyRect)) {
            if (isFromPlayer) {
                it->takeDamage();
                if (it->isDestroyed()) {
                    game.playExplosionSound();
                    enemies.erase(it);
                }
            }
            collided = true;
            offScreen = true;
            return;
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
