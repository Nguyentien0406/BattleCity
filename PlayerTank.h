#ifndef PLAYERTANK_H
#define PLAYERTANK_H

#include "Wall.h"
#include <SDL.h>
#include <vector>
class Bullet;
class EnemyTank;
class Game;

class PlayerTank {
public:
    PlayerTank(int x, int y);
    ~PlayerTank() {}

    void render(SDL_Renderer* renderer);
    void move(int dx, int dy, const std::vector<class Wall>& walls, const std::vector<class EnemyTank>& enemies);
    void shoot(Game &game);

    std::vector<Bullet>& getBullets();
    const SDL_Rect getRect() const;
    void takeDamage(Game &game);
    bool isDestroyed() const;

private:
    SDL_Rect rect;
    int speed;
    int direction;
    int health;
    int lives;
    std::vector<Bullet> bullets;
};

#endif
