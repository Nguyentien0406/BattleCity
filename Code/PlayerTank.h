#ifndef PLAYERTANK_H
#define PLAYERTANK_H

#include "Wall.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include <vector>
class Bullet;
class EnemyTank;
class Game;
class BossTank;

class PlayerTank {
public:
    PlayerTank(int x, int y);
    ~PlayerTank() {}

    void render(SDL_Renderer* renderer, SDL_Texture* spriteSheet, bool isOtherPlayer);
    void move(int dx, int dy, const std::vector<class Wall>& walls, const std::vector<class EnemyTank>& enemies, const std::vector<class BossTank>& bosses, const PlayerTank& otherPlayer);
    void shoot(Game &game);

    std::vector<Bullet>& getBullets();
    const SDL_Rect getRect() const;
    void takeDamage();
    bool isDestroyed() const;
    // Getter/Setter
    int getHealth() const { return health; }
    void setHealth(int health) { this->health = health; }

    int getSpeed() const { return speed; }
    void setSpeed(int s) { speed = s; }

    int getDirection() const { return direction; }
    void setDirection(int d) { direction = d; }

    int getCurrentFrame() const { return currentFrame; }
    void setCurrentFrame(int f) { currentFrame = f; }

    bool getIsMoving() const {return isMoving;}
    void setIsMoving(bool moving) { isMoving = moving; }

    int getBoostedMoves() const { return boostedMoves; }
    void setBoostedMoves(int moves) { boostedMoves = moves; }

    bool isAlive() const { return alive; }
    void setAlive(bool status) { alive = status; }

private:
    SDL_Rect rect;
    int speed;
    int baseSpeed;
    int direction;
    int health;
    int currentFrame;
    Uint32 lastFrameTime;
    bool isMoving;
    int boostedMoves;
    std::vector<Bullet> bullets;
    bool alive;
    Uint32 shootCooldown;
    Uint32 lastShotTime;
};

#endif
