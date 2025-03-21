#include "Game.h"
#include <SDL.h>
#include <SDL_mixer.h>

using namespace std;

Game::Game() : window(nullptr), renderer(nullptr), running(true), gameOver(false), backgroundMusic(nullptr), shootSound(nullptr), explosionSound(nullptr), player(40, 40) {
    srand(time(nullptr));
    generateBorders();
    generateMap();
    generateEnemies(4);
}
Game::~Game() {
    close();
}

bool Game::init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << endl;
        return false;
    }

    window = SDL_CreateWindow("Battle City", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        cerr << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    backgroundMusic = Mix_LoadMUS("Music\\Mesmerizing Galaxy Loop.mp3");
    if (!backgroundMusic) {
        cerr << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    } else {
        Mix_VolumeMusic(32);
        Mix_PlayMusic(backgroundMusic, -1);
    }
    Mix_PlayMusic(backgroundMusic, -1);

    shootSound = Mix_LoadWAV("Music\\shoot.wav");
    if (!shootSound) {
        cerr << "Failed to load shooting sound! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    explosionSound = Mix_LoadWAV("Music\\explosion.wav");
    if (!explosionSound) {
        cerr << "Failed to load explosion sound! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    return true;
}

void Game::setGameOver() {
    gameOver = true;
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    renderWalls();
    renderBorders();
    player.render(renderer);

    for (const EnemyTank& enemy : enemyTanks) {
        enemy.render(renderer);
    }

    if (enemyTanks.empty()) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect winMessage = {SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 25, 200, 50};
        SDL_RenderFillRect(renderer, &winMessage);
    }

    SDL_RenderPresent(renderer);
}

void Game::run() {
    while (running) {
        handleEvents();
        updateBullets();
        updateEnemies();
        render();
        if (enemyTanks.empty()) {
            cout << "🎉 You Win! All enemies defeated!" << endl;
            SDL_Delay(10);
            running = false;
        }
        if (gameOver) {
            cout << "💀 Game Over! You were destroyed!" << endl;
            SDL_Delay(10);
            running = false;
        }
        SDL_Delay(10);
    }
}

void Game::close() {
    Mix_HaltMusic();
    Mix_FreeMusic(backgroundMusic);
    Mix_CloseAudio();
    Mix_FreeChunk(shootSound);
    Mix_FreeChunk(explosionSound);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::playShootSound() {
    if (shootSound) {
        Mix_VolumeChunk(shootSound, 32);
        Mix_PlayChannel(-1, shootSound, 0);
    }
}

void Game::playExplosionSound() {
    if (explosionSound) {
        Mix_VolumeChunk(explosionSound, 32);
        Mix_PlayChannel(-1, explosionSound, 0);
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) running = false;
        else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_UP:    player.move(0, -1, walls, enemyTanks); break;
                case SDLK_DOWN:  player.move(0, 1, walls, enemyTanks); break;
                case SDLK_LEFT:  player.move(-1, 0, walls, enemyTanks); break;
                case SDLK_RIGHT: player.move(1, 0, walls, enemyTanks); break;
                case SDLK_SPACE: player.shoot(*this); break;
            }
        }
    }
}
void Game::generateBorders() {
    borders.clear();

    SDL_Rect top_border = { 0, 0, SCREEN_WIDTH, BORDER_THICKNESS };
    SDL_Rect bottom_border = { 0, SCREEN_HEIGHT - BORDER_THICKNESS, SCREEN_WIDTH, BORDER_THICKNESS };
    SDL_Rect left_border = { 0, 0, BORDER_THICKNESS, SCREEN_HEIGHT };
    SDL_Rect right_border = { SCREEN_WIDTH - BORDER_THICKNESS, 0, BORDER_THICKNESS, SCREEN_HEIGHT };

    borders.push_back(top_border);
    borders.push_back(bottom_border);
    borders.push_back(left_border);
    borders.push_back(right_border);
}

void Game::renderBorders() {
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    for (const auto& border : borders) {
        SDL_RenderFillRect(renderer, &border);
    }
}

void Game::generateMap() {
    int mapWidth = SCREEN_WIDTH / TILE_SIZE;
    int mapHeight = SCREEN_HEIGHT / TILE_SIZE;

    map.resize(mapHeight, vector<TileType>(mapWidth, EMPTY));

    map[5][5] = WALL;
    map[5][6] = WALL;
    map[6][5] = WALL;
    map[10][11] = WALL;
    map[10][12] = WALL;
    map[11][12] = WALL;
    map[10][13] = WALL;
    map[4][14] = WALL;
    map[4][15] = WALL;
    map[3][14] = WALL;
    map[3][15] = WALL;

    walls.clear();
    for (int y = 0; y < mapHeight; ++y) {
        for (int x = 0; x < mapWidth; ++x) {
            if (map[y][x] == WALL) {
                walls.emplace_back(x * TILE_SIZE, y * TILE_SIZE);
            }
        }
    }
}

void Game::renderWalls() {
    for (const Wall& wall : walls) {
        wall.render(renderer);
    }
}

void Game::updateBullets() {
    vector<Bullet>& playerBullets = player.getBullets();

    for (Bullet& bullet : playerBullets) {
        bullet.update(walls, player, enemyTanks, *this);
    }

    playerBullets.erase(remove_if(playerBullets.begin(), playerBullets.end(),
        [](const Bullet& b) { return b.isOffScreen() || b.hasCollided(); }),
        playerBullets.end());

    for (EnemyTank& enemy : enemyTanks) {
        enemy.updateBullets(walls, player, enemyTanks, *this);
    }
}

void Game::updateEnemies() {
    for (EnemyTank& enemy : enemyTanks) {
        enemy.move(walls, player, enemyTanks);
        enemy.shoot();
    }
}

void Game::generateEnemies(int numEnemies) {
    for (int i = 0; i < numEnemies; ++i) {
        bool validPosition = false;
        int x, y;
        int attempts = 0;

        while (!validPosition && attempts < 50) {
            x = (rand() % ((SCREEN_WIDTH - 2 * BORDER_THICKNESS) / TILE_SIZE)) * TILE_SIZE + BORDER_THICKNESS;
            y = (rand() % ((SCREEN_HEIGHT - 2 * BORDER_THICKNESS) / TILE_SIZE)) * TILE_SIZE + BORDER_THICKNESS;
            SDL_Rect enemyRect = {x, y, TILE_SIZE, TILE_SIZE};
            validPosition = true;

            for (const Wall& wall : walls) {
                if (SDL_HasIntersection(&enemyRect, &wall.getRect())) {
                    validPosition = false;
                    break;
                }
            }

            SDL_Rect playerRect = player.getRect();
            if (SDL_HasIntersection(&enemyRect, &playerRect)) {
                validPosition = false;
            }

            for (const EnemyTank& enemy : enemyTanks) {
                SDL_Rect enemyHitbox = enemy.getRect();
                if (SDL_HasIntersection(&enemyRect, &enemyHitbox)) {
                    validPosition = false;
                    break;
                }
            }

            if (x < BORDER_THICKNESS || x > SCREEN_WIDTH - TILE_SIZE - BORDER_THICKNESS ||
                y < BORDER_THICKNESS || y > SCREEN_HEIGHT - TILE_SIZE - BORDER_THICKNESS) {
                validPosition = false;
            }
            attempts++;
        }

        if (validPosition) {
            enemyTanks.emplace_back(x, y);
        }
    }
}

