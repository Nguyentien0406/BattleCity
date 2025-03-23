#include "Game.h"
#include "PlayerTank.h"
#include <iostream>

using namespace std;

PlayerTank::PlayerTank(int x, int y) : speed(5), baseSpeed(5), direction(0), health(5), currentFrame(0), lastFrameTime(SDL_GetTicks()), isMoving(false), boostedMoves(0) {
    rect.x = x;
    rect.y = y;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;
}

void PlayerTank::render(SDL_Renderer* renderer, SDL_Texture* spriteSheet) {
    SDL_Rect srcRect;
    Uint32 currentTime = SDL_GetTicks();
    if (isMoving && currentTime - lastFrameTime > 10) {
        currentFrame = 1 - currentFrame;
        lastFrameTime = currentTime;
    }
    switch (direction) {
        case 0:
            srcRect = currentFrame == 0 ? SDL_Rect{522, 7, 55, 55} : SDL_Rect{586, 7, 55, 55};
            break;
        case 1:
            srcRect = currentFrame == 0 ? SDL_Rect{778, 3, 56, 55} : SDL_Rect{842, 3, 56, 55};
            break;
        case 2:
            srcRect = currentFrame == 0 ? SDL_Rect{652, 2, 59, 56} : SDL_Rect{716, 2, 59, 56};
            break;
        case 3:
            srcRect = currentFrame == 0 ? SDL_Rect{905, 3, 54, 55} : SDL_Rect{970, 3, 54, 55};
            break;
    }
    SDL_RenderCopy(renderer, spriteSheet, &srcRect, &rect);
}

void PlayerTank::move(int dx, int dy, const vector<Wall>& walls, const vector<EnemyTank>& enemies) {
    speed = (boostedMoves > 0) ? baseSpeed * 8 : baseSpeed;
    int newX = rect.x + dx * speed;
    int newY = rect.y + dy * speed;
    SDL_Rect newRect = { newX, newY, rect.w, rect.h };

    for (const Wall& wall : walls) {
        if (!wall.isCamouflaged() && SDL_HasIntersection(&newRect, &wall.getRect())) {
            return;
        }
    }

    for (const EnemyTank& enemy : enemies) {
            SDL_Rect enemyRect = enemy.getRect();
            if (SDL_HasIntersection(&newRect, &enemyRect)) {
                return;
            }
    }

    rect.x = newX;
    rect.y = newY;

    if (dx == 1) direction = 3;
    else if (dx == -1) direction = 2;
    else if (dy == 1) direction = 1;
    else if (dy == -1) direction = 0;

    if (boostedMoves > 0) boostedMoves--;

    if (rect.x < TILE_SIZE) rect.x = TILE_SIZE;
    if (rect.x > SCREEN_WIDTH - TILE_SIZE - TILE_SIZE) rect.x = SCREEN_WIDTH - TILE_SIZE - TILE_SIZE;
    if (rect.y < TILE_SIZE) rect.y = TILE_SIZE;
    if (rect.y > SCREEN_HEIGHT - TILE_SIZE - TILE_SIZE) rect.y = SCREEN_HEIGHT - TILE_SIZE - TILE_SIZE;
}

void PlayerTank::shoot(Game &game) {
    int bulletX = rect.x;
    int bulletY = rect.y;
    int offset = 10;

    switch (direction) {
        case 0:
            bulletX += rect.w / 2 - BULLET_SIZE / 2;
            bulletY -= BULLET_SIZE + offset;
            break;
        case 1:
            bulletX += rect.w / 2 - BULLET_SIZE / 2;
            bulletY += rect.h - offset;
            break;
        case 2:
            bulletX -= BULLET_SIZE - offset;
            bulletY += rect.h / 2 - BULLET_SIZE / 2;
            break;
        case 3:
            bulletX += rect.w - offset;
            bulletY += rect.h / 2 - BULLET_SIZE / 2;
            break;
    }

    bullets.emplace_back(bulletX, bulletY, direction, true);
    game.playShootSound();
}

vector<Bullet>& PlayerTank::getBullets() {
    return bullets;
}

const SDL_Rect PlayerTank::getRect() const {
    return rect;
}

void PlayerTank::takeDamage(Game &game) {
    health--;
    if (health <= 0) {
        game.addExplosion(rect.x, rect.y);
        game.playExplosionSound();
        SDL_Delay(400);
        game.setGameOver();
    }
}

void PlayerTank::setIsMoving(bool moving) {
    isMoving = moving;
}
