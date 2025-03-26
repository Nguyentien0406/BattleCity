#ifndef WALL_H
#define WALL_H

#include "Game.h"
#include <SDL.h>
#include <vector>

const int SCREEN_WIDTH= 800;
const int SCREEN_HEIGHT= 600;
const int TILE_SIZE= 40;
const int BULLET_SIZE= 7;
const int BULLET_SPEED= 10;

class Wall {
public:
    Wall(int x, int y, bool breakable, bool camouflaged);
    ~Wall() {}

    void render(SDL_Renderer* renderer, SDL_Texture* spriteSheet) const;
    const SDL_Rect& getRect() const { return rect; }
    bool isBreakable() const { return breakable; }
    bool isCamouflaged() const { return camouflaged; }
    void takeDamage(int direction);
    bool isDestroyed() const;
    void setSize(int w, int h) { rect.w= w; rect.h= h; }

    int getHitPoints() const { return hitPoints; }
    void setHitPoints(int hp) { hitPoints= hp; }

    bool isHalfDamaged() const { return halfDamaged; }
    void setHalfDamaged(bool damaged) { halfDamaged= damaged; }

    int getDamageDirection() const { return damageDirection; }
    void setDamageDirection(int direction) { damageDirection= direction; }

private:
    SDL_Rect rect;
    bool breakable;
    bool camouflaged;
    int hitPoints;
    int damageDirection;
    bool halfDamaged;
};

#endif


