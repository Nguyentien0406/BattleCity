#include "Game.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>

using namespace std;

Game::Game() : window(nullptr), renderer(nullptr), running(true), gameOver(false), backgroundMusic(nullptr), shootSound(nullptr), explosionSound(nullptr), player(360, 320) {
    srand(time(nullptr));
    spriteSheet= nullptr;
    generateBorders();
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

    spriteSheet = IMG_LoadTexture(renderer, "C:\\SDL2\\BattleCity\\assets.png");
    if (!spriteSheet) {
        cerr << "Failed to load textures!" << endl;
        return false;
    }

    loadMapFromFile("Map/Map1.txt");
    generateEnemies(4);

    return true;
}

void Game::setGameOver() {
    gameOver = true;
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    renderBorders();

    for (const Wall& wall : walls) {
        if (!wall.isCamouflaged()) {
            wall.render(renderer, spriteSheet);
        }
    }

    player.render(renderer, spriteSheet);

    for (EnemyTank& enemy : enemyTanks) {
        enemy.render(renderer, spriteSheet);
    }

    for (const Bullet& bullet : player.getBullets()) {
        bullet.render(renderer, spriteSheet);
    }

    for (EnemyTank& enemy : enemyTanks) {
        for (const Bullet& bullet : enemy.getBullets()) {
            bullet.render(renderer, spriteSheet);
        }
    }

    for (const Explosion& explosion : explosions) {
        SDL_Rect srcRect = {1120, 519, 56, 56};
        SDL_Rect destRect = {explosion.x, explosion.y, TILE_SIZE, TILE_SIZE};
        SDL_RenderCopy(renderer, spriteSheet, &srcRect, &destRect);
    }
    explosions.erase(remove_if(explosions.begin(), explosions.end(),[](const Explosion& e) { return SDL_GetTicks() - e.startTime > 500; }),explosions.end());

    for (const Wall& wall : walls) {
        if(wall.isCamouflaged()) {
            wall.render(renderer, spriteSheet);
        }
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

void Game::addExplosion(int x, int y) {
    explosions.push_back({x, y, SDL_GetTicks()});
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

    SDL_Rect top_border = { 0, 0, SCREEN_WIDTH, TILE_SIZE };
    SDL_Rect bottom_border = { 0, SCREEN_HEIGHT - TILE_SIZE, SCREEN_WIDTH, TILE_SIZE };
    SDL_Rect left_border = { 0, 0, TILE_SIZE, SCREEN_HEIGHT };
    SDL_Rect right_border = { SCREEN_WIDTH - TILE_SIZE, 0, TILE_SIZE, SCREEN_HEIGHT };

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

void Game::loadMapFromFile(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Không thể mở file bản đồ: " << filename << endl;
        return;
    }
    map.clear();
    walls.clear();
    int y = 0;
    string line;
    while (getline(file, line)) {
        vector<int> row;
        istringstream iss(line);
        int x = 0, tile;
        while (iss >> tile) {
            row.push_back(tile);
            if (tile == 1) {
                walls.emplace_back(x * TILE_SIZE, y * TILE_SIZE, false, false);
            } else if (tile == 2) {
                walls.emplace_back(x * TILE_SIZE, y * TILE_SIZE, true, false);
            } else if (tile == 3) {
                walls.emplace_back(x * TILE_SIZE, y * TILE_SIZE, false, true);
            }
            x++;
        }
        map.push_back(row);
        y++;
    }
    file.close();
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
        while (!validPosition) {
            x = (rand() % ((SCREEN_WIDTH - 2 * TILE_SIZE) / TILE_SIZE)) * TILE_SIZE + TILE_SIZE;
            y = (rand() % ((SCREEN_HEIGHT - 2 * TILE_SIZE) / TILE_SIZE)) * TILE_SIZE + TILE_SIZE;
            SDL_Rect enemyRect = {x, y, TILE_SIZE, TILE_SIZE};
            validPosition = true;

            for (Wall& wall : walls) {
                SDL_Rect wallRect = wall.getRect();
                if (SDL_HasIntersection(&enemyRect, &wallRect)) {
                    validPosition = false;
                    break;
                }
            }

            SDL_Rect playerRect = player.getRect();
            if (SDL_HasIntersection(&enemyRect, &playerRect)) {
                validPosition = false;
            }

            for (EnemyTank& enemy : enemyTanks) {
                SDL_Rect enemyHitbox = enemy.getRect();
                if (SDL_HasIntersection(&enemyRect, &enemyHitbox)) {
                    validPosition = false;
                    break;
                }
            }

            if (x < TILE_SIZE || x > SCREEN_WIDTH - TILE_SIZE - TILE_SIZE ||
                y < TILE_SIZE || y > SCREEN_HEIGHT - TILE_SIZE - TILE_SIZE) {
                validPosition = false;
            }
        }
        if (validPosition) {
            enemyTanks.emplace_back(x, y);
        }
    }
}


void Game::removeWall(int x, int y) {
    walls.erase(remove_if(walls.begin(), walls.end(),
        [x, y](const Wall& w) {
            return w.getRect().x == x && w.getRect().y == y;
        }),
        walls.end());
}
