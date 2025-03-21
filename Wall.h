#ifndef WALL_H
#define WALL_H

#include "Game.h"
#include <SDL.h>
#include <vector>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TILE_SIZE = 40;
const int BORDER_THICKNESS = 10;
const int BULLET_SIZE = 10;
const int BULLET_SPEED = 10;

class Wall {
public:
    Wall(int x, int y);
    ~Wall() {}

    void render(SDL_Renderer* renderer) const;
    const SDL_Rect& getRect() const { return rect; }

private:
    SDL_Rect rect;
};

#endif


