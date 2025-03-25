#ifndef ENEMYTANK_H
#define ENEMYTANK_H

#include "Wall.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include <vector>
class Bullet;
class PlayerTank;
class Game;

class EnemyTank {
public:
    EnemyTank(int x, int y);
    ~EnemyTank() {}

    void render(SDL_Renderer* renderer, SDL_Texture* spriteSheet);
    void move(const std::vector<Wall>& walls, const PlayerTank& player, const PlayerTank& otherPlayer, const std::vector<EnemyTank>& enemies);
    void shoot();
    void updateBullets(std::vector<Wall>& walls, PlayerTank& player, PlayerTank& otherPlayer, std::vector<EnemyTank>& enemies,Game &game);

    std::vector<Bullet>& getBullets();
    const SDL_Rect getRect() const;
    void takeDamage();
    bool isDestroyed() const;

    int getHealth() const { return health; }
    void setHealth(int health) { this->health= health; }

    int getDirection() const { return direction; }
    void setDirection(int direction) { this->direction= direction; }

    int getChangeDirectionCooldown() const { return changeDirectionCooldown; }
    void setChangeDirectionCooldown(int changeDirectionCooldown) { this->changeDirectionCooldown= changeDirectionCooldown; }

private:
    SDL_Rect rect;
    int speed;
    int direction;
    int health;
    int currentFrame;
    Uint32 lastFrameTime;
    bool isMoving;
    std::vector<Bullet> bullets;
    int shootCooldown;
    int changeDirectionCooldown;
};

#endif
