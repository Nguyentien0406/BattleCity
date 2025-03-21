#include "Game.h"
#include "PlayerTank.h"

using namespace std;

PlayerTank::PlayerTank(int x, int y) : speed(20), direction(0), health(5) {
    rect.x = x;
    rect.y = y;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;
}

void PlayerTank::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &rect);

    for (const Bullet& bullet : bullets) {
        bullet.render(renderer);
    }
}

void PlayerTank::move(int dx, int dy, const vector<Wall>& walls, const vector<EnemyTank>& enemies) {
    int newX = rect.x + dx * speed;
    int newY = rect.y + dy * speed;
    SDL_Rect newRect = { newX, newY, rect.w, rect.h };

    for (const Wall& wall : walls) {
        if (SDL_HasIntersection(&newRect, &wall.getRect())) {
            return;
        }
    }

    for (const EnemyTank& enemy : enemies) {
            SDL_Rect enemyRect = enemy.getRect();
            if (SDL_HasIntersection(&newRect, &enemyRect)) {
                direction = rand() % 4;
                return;
            }
    }

    rect.x = newX;
    rect.y = newY;

    if (dx == 1) direction = 3;  // Right
    else if (dx == -1) direction = 2;  // Left
    else if (dy == 1) direction = 1;  // Down
    else if (dy == -1) direction = 0;  // Up

    if (rect.x < BORDER_THICKNESS) rect.x = BORDER_THICKNESS;
    if (rect.x > SCREEN_WIDTH - TILE_SIZE - BORDER_THICKNESS) rect.x = SCREEN_WIDTH - TILE_SIZE - BORDER_THICKNESS;
    if (rect.y < BORDER_THICKNESS) rect.y = BORDER_THICKNESS;
    if (rect.y > SCREEN_HEIGHT - TILE_SIZE - BORDER_THICKNESS) rect.y = SCREEN_HEIGHT - TILE_SIZE - BORDER_THICKNESS;
}

void PlayerTank::shoot() {
    int bulletX = rect.x;
    int bulletY = rect.y;
    int offset = 5;

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

    bullets.emplace_back(bulletX, bulletY, direction, true);
}
const SDL_Rect PlayerTank::getRect() const {
    return rect;
}

vector<Bullet>& PlayerTank::getBullets() {
    return bullets;
}

void PlayerTank::takeDamage(Game &game) {
    health--;
    if (health <= 0) {
        game.setGameOver();
    }
}

bool PlayerTank::isDestroyed() const {
    return health <= 0;
}
