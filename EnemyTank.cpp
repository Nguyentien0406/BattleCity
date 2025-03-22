#include "EnemyTank.h"
#include "Game.h"
using namespace std;

EnemyTank::EnemyTank(int x, int y) : speed(10), direction(rand() % 4), health(3), moveCooldown(10), shootCooldown(0), changeDirectionCooldown(0) {
    rect.x = x;
    rect.y = y;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;
}

void EnemyTank::render(SDL_Renderer* renderer, SDL_Texture* spriteSheet) const{
    SDL_Rect srcRect;
    switch (direction) {
        case 0:
            srcRect = {521, 530, 57, 56};
            break;
        case 1:
            srcRect = {778, 526, 57, 55};
            break;
        case 2:
            srcRect = {651, 526, 59, 56};
            break;
        case 3:
            srcRect = {906, 526, 55, 56};
            break;
    }
    SDL_RenderCopy(renderer, spriteSheet, &srcRect, &rect);
}

void EnemyTank::move(const vector<Wall>& walls, const PlayerTank& player, const vector<EnemyTank>& enemies) {
    if (moveCooldown > 0) {
        moveCooldown--;
        return;
    }

    moveCooldown = 10;

    if (changeDirectionCooldown <= 0) {
        int newDirection;
        do {
            newDirection = rand() % 4;
        } while (newDirection == direction);
        direction = newDirection;
        changeDirectionCooldown = 50 + rand() % 50;
    } else {
        changeDirectionCooldown--;
    }

    int dx = 0, dy = 0;
    switch (direction) {
        case 0: dy = -1; break;
        case 1: dy = 1; break;
        case 2: dx = -1; break;
        case 3: dx = 1; break;
    }

    int newX = rect.x + dx * speed;
    int newY = rect.y + dy * speed;
    SDL_Rect newRect = { newX, newY, rect.w, rect.h };

    for (const Wall& wall : walls) {
        if (SDL_HasIntersection(&newRect, &wall.getRect())) {
            direction = rand() % 4;
            moveCooldown = 30;
            return;
        }
    }
    SDL_Rect playerRect = player.getRect();
    if (SDL_HasIntersection(&newRect, &playerRect)) {
        direction = rand() % 4;
        return;
    }

    for (const EnemyTank& enemy : enemies) {
    if (&enemy != this) {
        SDL_Rect enemyRect = enemy.getRect();
        if (SDL_HasIntersection(&newRect, &enemyRect)) {
            direction = rand() % 4;
            return;
        }
    }
}

    if (newX < TILE_SIZE || newX > SCREEN_WIDTH - TILE_SIZE - TILE_SIZE ||
        newY < TILE_SIZE || newY > SCREEN_HEIGHT - TILE_SIZE - TILE_SIZE) {
        direction = rand() % 4;
        moveCooldown = 30;
        return;
    }

    rect.x = newX;
    rect.y = newY;
}



void EnemyTank::shoot() {
    if (shootCooldown > 0) {
        shootCooldown--;
        return;
    }

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
            bulletX -= BULLET_SIZE + offset;
            bulletY += rect.h / 2 - BULLET_SIZE / 2;
            break;
        case 3:
            bulletX += rect.w - offset;
            bulletY += rect.h / 2 - BULLET_SIZE / 2;
            break;
    }

    bullets.emplace_back(bulletX, bulletY, direction, false);
    shootCooldown = 50 + (rand() % 50);
}

void EnemyTank::updateBullets(vector<Wall>& walls, PlayerTank& player, vector<EnemyTank>& enemies, Game &game) {
    for (Bullet& bullet : bullets) {
        bullet.update(walls, player, enemies, game);
    }

    bullets.erase(remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b) { return b.isOffScreen() || b.hasCollided(); }),
        bullets.end());
}

const SDL_Rect EnemyTank::getRect() const {
    return rect;
}

vector<Bullet>& EnemyTank::getBullets() {
    return bullets;
}
void EnemyTank::takeDamage() {
    health--;
}
bool EnemyTank::isDestroyed() const {
    return health <= 0;
}
