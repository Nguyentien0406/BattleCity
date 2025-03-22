#ifndef BULLET_H
#define BULLET_H

#include "Game.h"
#include "Wall.h"
#include "PlayerTank.h"
#include "EnemyTank.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include <vector>

class Bullet {
public:
    Bullet(int x, int y, int direction, bool fromPlayer);
    ~Bullet() {}

    void render(SDL_Renderer* renderer, SDL_Texture* spriteSheet) const;
    void update(std::vector<Wall>& walls, PlayerTank& player, std::vector<EnemyTank>& enemies, Game &game);
    bool isOffScreen() const;
    bool hasCollided() const;
    SDL_Rect getRect() const { return rect; }

private:
    SDL_Rect rect;
    int speed;
    int direction;
    bool offScreen;
    bool collided;
    bool isFromPlayer;
};

#endif

