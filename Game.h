#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include "Wall.h"
#include "Bullet.h"
#include "PlayerTank.h"
#include "EnemyTank.h"

enum TileType {
    EMPTY,
    WALL
};

class Game {
public:
    Game();
    ~Game();

    bool init();
    void setGameOver();
    void render();
    void run();
    void handleEvents();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    bool gameOver;
    PlayerTank player;
    std::vector<SDL_Rect> borders;
    std::vector<std::vector<TileType>> map;
    std::vector<Wall> walls;
    std::vector<EnemyTank> enemyTanks;

    void generateBorders();
    void renderBorders();
    void generateMap();
    void renderWalls();
    void updateBullets();
    void updateEnemies();
    void generateEnemies(int numEnemies);
};

#endif
