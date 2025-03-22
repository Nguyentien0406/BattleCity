#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>
#include "Wall.h"
#include "Bullet.h"
#include "PlayerTank.h"
#include "EnemyTank.h"

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
    void addExplosion(int x, int y);
    void removeWall(int x, int y);
    SDL_Texture* spriteSheet;

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
    std::vector<std::vector<int>> map;
    std::vector<Wall> walls;
    std::vector<EnemyTank> enemyTanks;
    struct Explosion {
        int x, y;
        Uint32 startTime;
    };
    std::vector<Explosion> explosions;
    void generateBorders();
    void renderBorders();
    void loadMapFromFile(const std::string& filename);
    void updateBullets();
    void updateEnemies();
    void generateEnemies(int numEnemies);
};

#endif
