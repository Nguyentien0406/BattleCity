#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_mixer.h>
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
    void close();
    void playShootSound();
    void playExplosionSound();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    bool gameOver;
    Mix_Music* backgroundMusic;
    Mix_Chunk* shootSound;
    Mix_Chunk* explosionSound;
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
