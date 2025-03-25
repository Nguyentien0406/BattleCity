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
#include <cstdio>
#include <string>
#include "Wall.h"
#include "Bullet.h"
#include "PlayerTank.h"
#include "EnemyTank.h"

class Game {
public:
    Game();
    ~Game();

    bool init();
    void setGameOver(PlayerTank& player, PlayerTank& otherPlayer);
    void setGameWin();
    void renderGameOverScreen();
    void renderWinScreen();
    void render();
    void run();
    void handleEvents(PlayerTank& player, PlayerTank& otherPlayer);
    void close();
    void playShootSound();
    void playExplosionSound();
    void addExplosion(int x, int y);
    void updateExplosions();
    void removeWall(int x, int y);
    void loadNextMap();
    SDL_Texture* spriteSheet;

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    bool gameOver;
    bool gameWin;
    int currentMap;
    Mix_Music* backgroundMusic;
    Mix_Chunk* shootSound;
    Mix_Chunk* explosionSound;
    PlayerTank player;
    PlayerTank otherPlayer;
    std::vector<SDL_Rect> borders;
    std::vector<std::vector<int>> map;
    std::vector<Wall> walls;
    std::vector<EnemyTank> enemyTanks;
    struct Explosion{
        int x, y;
        Uint32 startTime;
        int currentFrame;
        int currentSize;
    };
    std::vector<Explosion> explosions;
    void saveGame(const std::string& filename);
    void loadGame(const std::string& filename);
    void generateBorders();
    void renderBorders();
    void loadMapFromFile(const std::string& filename);
    void updateBullets();
    void updateEnemies();
    void generateEnemies(int numEnemies);
};

#endif
