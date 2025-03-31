#include "Game.h"
#include "ModeSelection.h"
#include "MainMenu.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

using namespace std;
 // Hàm kiểm tra file hợp lệ
bool isFileValid(const string& filename) {
    FILE* file= fopen(filename.c_str(), "r");
    if (!file) return false;
    fseek(file, 0, SEEK_END);
    long size= ftell(file);
    fclose(file);
    return size> 0;
}
  // Constructor
Game::Game() : window(nullptr), renderer(nullptr), running(true), gameOver(false), gameWin(false),
 currentMap(1), backgroundMusic(nullptr), shootSound(nullptr), explosionSound(nullptr), player(400, 320), otherPlayer(360,320) {
    srand(time(nullptr));
    spriteSheet= nullptr;
    generateBorders();
}
 // Destructor
Game::~Game() {
    close();
}
 // Khởi tạo game (SDL, cửa sổ, renderer, âm thanh)
bool Game::init() {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << endl;
        return false;
    }

    window= SDL_CreateWindow("Battle City", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(!window) {
        cerr << "SDL_CreateWindow Error: " << SDL_GetError() << endl;
        return false;
    }

    renderer= SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) {
        cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << endl;
        return false;
    }

    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cerr << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    backgroundMusic= Mix_LoadMUS("Music\\Mesmerizing Galaxy Loop.mp3");
    if(!backgroundMusic) {
        cerr << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    } else{
        Mix_VolumeMusic(32);
        Mix_PlayMusic(backgroundMusic, -1);
    }
    Mix_PlayMusic(backgroundMusic, -1);

    shootSound= Mix_LoadWAV("Music\\shoot.wav");
    if(!shootSound) {
        cerr << "Failed to load shooting sound! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    explosionSound= Mix_LoadWAV("Music\\explosion.wav");
    if(!explosionSound) {
        cerr << "Failed to load explosion sound! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }

    spriteSheet= IMG_LoadTexture(renderer, "C:\\SDL2\\BattleCity\\Ảnh\\assets.png");
    if(!spriteSheet) {
        cerr << "Failed to load textures!" << endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }

    return true;
}
 // Xử lý khi game over
void Game::setGameOver(PlayerTank& player, PlayerTank& otherPlayer) {
    if(!player.isAlive() && !otherPlayer.isAlive()) {
        gameOver= true;
        ofstream file("Save/Save1.txt", ios::trunc);
        file.close();
    }
}
 // Xử lý khi chiến thắng
void Game::setGameWin() {
    gameWin= true;
    ofstream file("Save/Save1.txt", ios::trunc);
    file.close();
}
 // Hiển thị màn hình game over
void Game::renderGameOverScreen() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_Texture* gameOverTexture = IMG_LoadTexture(renderer, "C:\\SDL2\\BattleCity\\Ảnh\\Lose.png");
    if(!gameOverTexture) {
        cerr << "Failed to load game over texture! SDL Error: " << SDL_GetError() << endl;
        return;
    }
    int width, height;
    SDL_QueryTexture(gameOverTexture, nullptr, nullptr, &width, &height);
    SDL_Rect gameOverRect= {(SCREEN_WIDTH- width)/ 2, (SCREEN_HEIGHT- height)/ 2, width, height};
    SDL_RenderCopy(renderer, gameOverTexture, nullptr, &gameOverRect);
    SDL_DestroyTexture(gameOverTexture);
    SDL_RenderPresent(renderer);
}
 // Hiển thị màn hình chiến thắng
void Game::renderWinScreen() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_Texture* winTextTexture= IMG_LoadTexture(renderer, "C:\\SDL2\\BattleCity\\Ảnh\\Win.png");
    if(!winTextTexture) {
        cerr << "Failed to load win text texture! SDL Error: " << SDL_GetError() << endl;
        return;
    }
    int width, height;
    SDL_QueryTexture(winTextTexture, nullptr, nullptr, &width, &height);
    SDL_Rect winTextRect= {(SCREEN_WIDTH - width) / 2, (SCREEN_HEIGHT - height) / 2, width, height};
    SDL_RenderCopy(renderer, winTextTexture, nullptr, &winTextRect);
    SDL_DestroyTexture(winTextTexture);
    SDL_RenderPresent(renderer);
}
 // Render toàn bộ game
void Game::render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    renderBorders();

    for(const Wall& wall : walls) {
        if(!wall.isCamouflaged()) {
            wall.render(renderer, spriteSheet);
        }
    }

    if (player.isAlive()) {
        player.render(renderer, spriteSheet, false);
    }

    if (otherPlayer.isAlive()) {
        otherPlayer.render(renderer, spriteSheet, true);
    }

    for(EnemyTank& enemy : enemyTanks) {
        enemy.render(renderer, spriteSheet);
    }

    for(BossTank& boss : bossTanks) {
        boss.render(renderer, spriteSheet);
    }

    if (player.isAlive()) {
        for(const Bullet& bullet : player.getBullets()) {
            bullet.render(renderer, spriteSheet);
        }
    }

    if (otherPlayer.isAlive()) {
        for(const Bullet& bullet : otherPlayer.getBullets()) {
            bullet.render(renderer, spriteSheet);
        }
    }

    for (EnemyTank& enemy : enemyTanks) {
        for (const Bullet& bullet : enemy.getBullets()) {
            bullet.render(renderer, spriteSheet);
        }
    }

    for (BossTank& boss : bossTanks) {
        for (const Bullet& bullet : boss.getBullets()) {
            bullet.render(renderer, spriteSheet);
        }
    }

    for(const Explosion& explosion : explosions) {
        SDL_Rect srcRect;
        switch(explosion.currentFrame) {
            case 0: srcRect= {160, 392, 11, 11}; break;
            case 1: srcRect= {173, 391, 13, 13}; break;
            case 2: srcRect= {191, 393, 11, 10}; break;
            case 3: srcRect= {205, 391, 15, 13}; break;
            case 4: srcRect= {165, 409, 32, 29}; break;
            default: srcRect= {165, 409, 32, 29}; break;
        }
        SDL_Rect destRect= {explosion.centerX- explosion.currentRadius, explosion.centerY- explosion.currentRadius,
        explosion.currentRadius* 2, explosion.currentRadius* 2};
        SDL_RenderCopy(renderer, spriteSheet, &srcRect, &destRect);
    }

    for(const Wall& wall : walls) {
        if(wall.isCamouflaged()) {
            wall.render(renderer, spriteSheet);
        }
    }
    SDL_RenderPresent(renderer);
}
 // Vòng lặp game chính
void Game::run() {
    while(true) {
        running= true;
        MainMenu menu(renderer);
        switch(menu.ShowMenu(isFileValid("Save/Save1.txt"))) {
            case MainMenu::START_GAME: {
                ModeSelection modeSelection(renderer);
                ModeSelection::Mode selectedMode = modeSelection.ShowSelection();
                if (selectedMode == ModeSelection::BACK) {
                    running= false;
                    break;
                }
                if (isFileValid("Save/Save1.txt")) {
                    remove("Save/Save1.txt");
                }
                currentMap = 1;
                otherPlayer.setAlive(true);
                player.setAlive(selectedMode== ModeSelection::TWO_PLAYERS);
                gameOver = false;
                gameWin = false;
                loadNextMap();
                break;
            }
            case MainMenu::CONTINUE_GAME:
                break;
            case MainMenu::EXIT_GAME:
                return;
        }
        if(isFileValid("Save/Save1.txt")) {
            loadGame("Save/Save1.txt");
        }
        while(running) {
            if(player.isAlive()) {
                if(player.getIsMoving()) {
                    switch(player.getDirection()) {
                        case 0: player.move(0, -1, walls, enemyTanks, bossTanks, otherPlayer); break;
                        case 1: player.move(0, 1, walls, enemyTanks, bossTanks, otherPlayer); break;
                        case 2: player.move(-1, 0, walls, enemyTanks, bossTanks, otherPlayer); break;
                        case 3: player.move(1, 0, walls, enemyTanks, bossTanks, otherPlayer); break;
                    }
                }
            }
            if(otherPlayer.isAlive()) {
                if(otherPlayer.getIsMoving()) {
                    switch(otherPlayer.getDirection()) {
                        case 0: otherPlayer.move(0, -1, walls, enemyTanks, bossTanks, player); break;
                        case 1: otherPlayer.move(0, 1, walls, enemyTanks, bossTanks, player); break;
                        case 2: otherPlayer.move(-1, 0, walls, enemyTanks, bossTanks, player); break;
                        case 3: otherPlayer.move(1, 0, walls, enemyTanks, bossTanks, player); break;
                    }
                }
            }
            handleEvents(player, otherPlayer);
            updateBullets();
            updateEnemies();
            updateBosses();
            updateExplosions();
            render();
            if(enemyTanks.empty()&& bossTanks.empty()) {
                if(currentMap == 1) {
                    SDL_Delay(200);
                    currentMap = 2;
                    loadNextMap();
                } else if(currentMap == 2) {
                    SDL_Delay(200);
                    currentMap = 3;
                    loadNextMap();
                } else{
                    SDL_Delay(200);
                    setGameWin();
                }
            }
            if(gameOver) {
                renderGameOverScreen();
                SDL_Delay(3000);
                running = false;
            }
            if(gameWin) {
                renderWinScreen();
                SDL_Delay(3000);
                running = false;
            }
            SDL_Delay(10);
        }
    }
}
 // Xử lý sự kiện bàn phím
void Game::handleEvents(PlayerTank& player, PlayerTank& otherPlayer) {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type== SDL_QUIT) {
                saveGame("Save/Save1.txt");
                running= false;
        } else if(event.type== SDL_KEYDOWN) {
            if (event.key.keysym.sym== SDLK_ESCAPE) {
                saveGame("Save/Save1.txt");
                running= false;
            } else if(event.key.keysym.sym== SDLK_LALT) {
                if(otherPlayer.isAlive()) {
                    otherPlayer.setBoostedMoves(15);
                }
            }else if(event.key.keysym.sym== SDLK_RALT) {
                if(player.isAlive()) {
                    player.setBoostedMoves(15);
                }
            }
            if(player.isAlive()) {
                switch(event.key.keysym.sym) {
                    case SDLK_UP: player.setDirection(0); player.setIsMoving(true); break;
                    case SDLK_DOWN: player.setDirection(1); player.setIsMoving(true); break;
                    case SDLK_LEFT: player.setDirection(2); player.setIsMoving(true); break;
                    case SDLK_RIGHT: player.setDirection(3); player.setIsMoving(true); break;
                    case SDLK_RCTRL: player.shoot(*this); break;
                }
            }
            if(otherPlayer.isAlive()) {
                switch(event.key.keysym.sym) {
                    case SDLK_w: otherPlayer.setDirection(0); otherPlayer.setIsMoving(true); break;
                    case SDLK_s: otherPlayer.setDirection(1); otherPlayer.setIsMoving(true); break;
                    case SDLK_a: otherPlayer.setDirection(2); otherPlayer.setIsMoving(true); break;
                    case SDLK_d: otherPlayer.setDirection(3); otherPlayer.setIsMoving(true); break;
                    case SDLK_SPACE: otherPlayer.shoot(*this); break;
                }
            }
        } else if(event.type == SDL_KEYUP) {
            if(player.isAlive()) {
                switch(event.key.keysym.sym) {
                    case SDLK_UP: case SDLK_DOWN: case SDLK_LEFT: case SDLK_RIGHT:
                        player.setIsMoving(false);
                        break;
                }
            }
            if(otherPlayer.isAlive()) {
                switch(event.key.keysym.sym) {
                    case SDLK_w: case SDLK_s: case SDLK_a: case SDLK_d:
                        otherPlayer.setIsMoving(false);
                        break;
                }
            }
        }
    }
}
 // Giải phóng tài nguyên
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
 // Phát âm thanh bắn đạn
void Game::playShootSound() {
    if(shootSound) {
        Mix_VolumeChunk(shootSound, 32);
        Mix_PlayChannel(-1, shootSound, 0);
    }
}
 // Phát âm thanh nổ
void Game::playExplosionSound() {
    if(explosionSound) {
        Mix_VolumeChunk(explosionSound, 32);
        Mix_PlayChannel(-1, explosionSound, 0);
    }
}
 // Thêm hiệu ứng nổ
void Game::addExplosion(int x, int y, bool isBoss) {
    Explosion explosion;
    explosion.centerX= x;
    explosion.centerY= y;
    explosion.startTime= SDL_GetTicks();
    explosion.currentFrame= 0;
    if(isBoss) {
        explosion.currentRadius= 10;
        explosion.maxRadius= TILE_SIZE;
    } else{
        explosion.currentRadius= 5;
        explosion.maxRadius= TILE_SIZE/ 2;
    }
    explosions.push_back(explosion);
}
 // Cập nhật hiệu ứng nổ
void Game::updateExplosions() {
    Uint32 currentTime= SDL_GetTicks();
    for(auto it= explosions.begin(); it != explosions.end(); ) {
        Explosion& e= *it;
        Uint32 elapsed= currentTime- e.startTime;
        if(elapsed < 500) {
            e.currentRadius= 5+ (elapsed* (e.maxRadius - 5))/ 500;
        }
        e.currentFrame= min(4, (int)(elapsed/ 100));
        if(elapsed >= 500) {
            it= explosions.erase(it);
        } else {
            ++it;
        }
    }
}
 // Xóa tường
void Game::removeWall(int x, int y) {
    walls.erase(remove_if(walls.begin(), walls.end(),
        [x, y](const Wall& w) {
            return w.getRect().x== x && w.getRect().y== y;
        }),
        walls.end());
}
 // Tải bản đồ tiếp theo
void Game::loadNextMap() {
    map.clear();
    walls.clear();
    enemyTanks.clear();
    bossTanks.clear();
    player.getBullets().clear();
    otherPlayer.getBullets().clear();
    explosions.clear();

    if(player.isAlive()) {
        player= PlayerTank(400, 320);
    }
    if(otherPlayer.isAlive()) {
        otherPlayer= PlayerTank(360, 320);
    }
    if(currentMap== 1) {
        loadMapFromFile("Map/Map1.txt");
        generateEnemies(4);
    } else if(currentMap== 2) {
        loadMapFromFile("Map/Map2.txt");
        generateEnemies(5);
    } else if(currentMap== 3) {
        loadMapFromFile("Map/Map3.txt");
        generateEnemies(3);
        generateBosses(2);
    }
}
 // Lưu game
void Game::saveGame(const string& filename) {
    ofstream file(filename);
    if(!file) {
        cerr << "Không thể mở file để lưu trạng thái trò chơi!" << endl;
        return;
    }

    file << player.getRect().x << " " << player.getRect().y << " " << player.getHealth() << " ";
    file << player.getSpeed() << " " << player.getBoostedMoves() << " " << player.getDirection() << " ";
    file << player.getCurrentFrame() << " " << player.isAlive() << endl;

    file << player.getBullets().size() << endl;
    for(const auto& bullet : player.getBullets()) {
        file << bullet.getRect().x << " " << bullet.getRect().y << " " << bullet.getDirection() << endl;
    }

    file << otherPlayer.getRect().x << " " << otherPlayer.getRect().y << " " << otherPlayer.getHealth() << " ";
    file << otherPlayer.getSpeed() << " " << otherPlayer.getBoostedMoves() << " " << otherPlayer.getDirection() << " ";
    file << otherPlayer.getCurrentFrame() << " " << otherPlayer.isAlive() << endl;

    file << otherPlayer.getBullets().size() << endl;
    for(const auto& bullet : otherPlayer.getBullets()) {
            file << bullet.getRect().x << " " << bullet.getRect().y << " " << bullet.getDirection() << endl;
    }

    file << enemyTanks.size() << endl;
    for(auto& enemy : enemyTanks) {
        file << enemy.getRect().x << " " << enemy.getRect().y << " " << enemy.getHealth()<< " " << enemy.getDirection() << " " << enemy.getChangeDirectionCooldown() << endl;
        file << enemy.getBullets().size() << endl;
        for(const auto& bullet : enemy.getBullets()) {
            file << bullet.getRect().x << " " << bullet.getRect().y << " " << bullet.getDirection() << endl;
        }
    }

    file << bossTanks.size() << endl;
    for(auto& boss : bossTanks) {
        file << boss.getRect().x << " " << boss.getRect().y << " " << boss.getHealth()<< " " << boss.getDirection() << " " << boss.getChangeDirectionCooldown() << endl;
        file << boss.getBullets().size() << endl;
        for(const auto& bullet : boss.getBullets()) {
            file << bullet.getRect().x << " " << bullet.getRect().y << " " << bullet.getDirection() << endl;
        }
    }

    file << walls.size() << endl;
    for(const auto& wall : walls) {
        file << wall.getRect().x << " " << wall.getRect().y << " " << wall.getRect().w << " " << wall.getRect().h << " ";
        file << wall.isBreakable() << " " << wall.isCamouflaged() << " ";
        file << wall.getHitPoints() << " " << wall.isHalfDamaged() << " " << wall.getDamageDirection() << endl;
    }

    file << currentMap << endl;
    file.close();
}
 // Tải game
void Game::loadGame(const string& filename) {
    ifstream file(filename);
    if(!file) {
        cerr << "Không thể mở file để khôi phục trạng thái trò chơi!" << endl;
        return;
    }


    int playerX, playerY, playerHealth, speed, boostedMoves, direction, currentFrame;
    int numBullets;
    bool playerAlive;
    file >> playerX >> playerY >> playerHealth >> speed >> boostedMoves >> direction >> currentFrame >> playerAlive;
    player = PlayerTank(playerX, playerY);
    player.setHealth(playerHealth);
    player.setSpeed(speed);
    player.setBoostedMoves(boostedMoves);
    player.setDirection(direction);
    player.setCurrentFrame(currentFrame);
    player.setAlive(playerAlive);

    file >> numBullets;
    player.getBullets().clear();
    for(int i= 0; i< numBullets; ++i) {
        int bulletX, bulletY, direction;
        file >> bulletX >> bulletY >> direction;
        player.getBullets().emplace_back(bulletX, bulletY, direction, true, false);
    }

    file >> playerX >> playerY >> playerHealth >> speed >> boostedMoves >> direction >> currentFrame >> playerAlive;
    otherPlayer = PlayerTank(playerX, playerY);
    otherPlayer.setHealth(playerHealth);
    otherPlayer.setSpeed(speed);
    otherPlayer.setBoostedMoves(boostedMoves);
    otherPlayer.setDirection(direction);
    otherPlayer.setCurrentFrame(currentFrame);
    otherPlayer.setAlive(playerAlive);

    file >> numBullets;
    otherPlayer.getBullets().clear();
    for(int i= 0; i< numBullets; ++i) {
        int bulletX, bulletY, direction;
        file >> bulletX >> bulletY >> direction;
        otherPlayer.getBullets().emplace_back(bulletX, bulletY, direction, true, false);
    }

    int numEnemies;
    file >> numEnemies;
    enemyTanks.clear();
    for(int i=0;i< numEnemies; i++) {
        int enemyX, enemyY, enemyHealth, enemyDirection, enemyChangeCooldown;
        file >> enemyX >> enemyY >> enemyHealth >> enemyDirection >> enemyChangeCooldown;
        enemyTanks.emplace_back(enemyX, enemyY);
        enemyTanks.back().setHealth(enemyHealth);
        enemyTanks.back().setDirection(enemyDirection);
        enemyTanks.back().setChangeDirectionCooldown(enemyChangeCooldown);
        file >> numBullets;
        for (int j= 0; j< numBullets; j++) {
            int bulletX, bulletY, bulletDirection;
            file >> bulletX >> bulletY >> bulletDirection;
            enemyTanks.back().getBullets().emplace_back(bulletX, bulletY, bulletDirection, false, false);
        }
    }

    int numBosses;
    file >> numBosses;
    bossTanks.clear();
    for(int i=0;i< numBosses; i++) {
        int bossX, bossY, bossHealth, bossDirection, bossChangeCooldown;
        file >> bossX >> bossY >> bossHealth >> bossDirection >> bossChangeCooldown;
        bossTanks.emplace_back(bossX, bossY);
        bossTanks.back().setHealth(bossHealth);
        bossTanks.back().setDirection(bossDirection);
        bossTanks.back().setChangeDirectionCooldown(bossChangeCooldown);
        file >> numBullets;
        for (int j= 0; j< numBullets; j++) {
            int bulletX, bulletY, bulletDirection;
            file >> bulletX >> bulletY >> bulletDirection;
            bossTanks.back().getBullets().emplace_back(bulletX, bulletY, bulletDirection, false, true);
        }
    }

    int numWalls;
    file >> numWalls;
    walls.clear();
    for(int i= 0; i< numWalls; i++) {
        int wallX, wallY, width, height, hitPoints, damageDirection;
        bool breakable, camouflaged, halfDamaged;
        file >> wallX >> wallY >> width >> height >> breakable >> camouflaged >> hitPoints >> halfDamaged >> damageDirection;
        walls.emplace_back(wallX, wallY, breakable, camouflaged);
        walls.back().setSize(width, height);
        walls.back().setHitPoints(hitPoints);
        walls.back().setHalfDamaged(halfDamaged);
        walls.back().setDamageDirection(damageDirection);
    }

    file >> currentMap;
    file.close();
}
 // Tạo biên màn hình
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
 // Vẽ biên
void Game::renderBorders() {
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    for(const auto& border : borders) {
        SDL_RenderFillRect(renderer, &border);
    }
}
 // Tải bản đồ từ file
void Game::loadMapFromFile(const string& filename) {
    ifstream file(filename);
    if(!file) {
        cerr << "Không thể mở file bản đồ: " << filename << endl;
        return;
    }
    map.clear();
    walls.clear();
    int y = 0;
    string line;
    while(getline(file, line)) {
        vector<int> row;
        istringstream iss(line);
        int x= 0, tile;
        while(iss >> tile) {
            row.push_back(tile);
            if(tile== 1) {
                walls.emplace_back(x* TILE_SIZE, y* TILE_SIZE, false, false);
            } else if(tile== 2) {
                walls.emplace_back(x* TILE_SIZE, y* TILE_SIZE, true, false);
            } else if(tile== 3) {
                walls.emplace_back(x* TILE_SIZE, y* TILE_SIZE, false, true);
            }
            x++;
        }
        map.push_back(row);
        y++;
    }
    file.close();
}
 // Cập nhật đạn
void Game::updateBullets() {
    if(player.isAlive()) {
        vector<Bullet>& playerBullets = player.getBullets();
        for(Bullet& bullet : playerBullets) {
            bullet.update(walls, player, otherPlayer, enemyTanks, bossTanks, *this);
        }
        playerBullets.erase(remove_if(playerBullets.begin(), playerBullets.end(),
            [](const Bullet& b) { return b.hasCollided(); }),
            playerBullets.end());
    }

    if(otherPlayer.isAlive()) {
        vector<Bullet>& otherPlayerBullets = otherPlayer.getBullets();
        for(Bullet& bullet : otherPlayerBullets) {
            bullet.update(walls, player, otherPlayer, enemyTanks, bossTanks, *this);
        }
        otherPlayerBullets.erase(remove_if(otherPlayerBullets.begin(), otherPlayerBullets.end(),
            [](const Bullet& b) { return b.hasCollided(); }),
            otherPlayerBullets.end());
    }

    for(EnemyTank& enemy : enemyTanks) {
        enemy.updateBullets(walls, player, otherPlayer, enemyTanks, bossTanks, *this);
    }
    for(BossTank& boss : bossTanks) {
        boss.updateBullets(walls, player, otherPlayer, enemyTanks, bossTanks, *this);
    }
}
 // Cập nhật kẻ địch
void Game::updateEnemies() {
    for(EnemyTank& enemy : enemyTanks) {
        enemy.move(walls, player, otherPlayer, enemyTanks, bossTanks);
        enemy.shoot();
    }
}
 // Sinh kẻ địch
void Game::generateEnemies(int numEnemies) {
    for(int i= 0; i< numEnemies; i++) {
        bool validPosition= false;
        int x, y;
        while(!validPosition) {
            x= (rand() % ((SCREEN_WIDTH- 2 * TILE_SIZE)/ TILE_SIZE))* TILE_SIZE+ TILE_SIZE;
            y= (rand() % ((SCREEN_HEIGHT- 2 * TILE_SIZE)/ TILE_SIZE))* TILE_SIZE+ TILE_SIZE;
            SDL_Rect enemyRect= {x, y, TILE_SIZE, TILE_SIZE};
            validPosition= true;

            for(Wall& wall : walls) {
                SDL_Rect wallRect= wall.getRect();
                if(SDL_HasIntersection(&enemyRect, &wallRect)) {
                    validPosition= false;
                    break;
                }
            }

            if(player.isAlive()) {
                SDL_Rect playerRect= player.getRect();
                if(SDL_HasIntersection(&enemyRect, &playerRect)) {
                    validPosition= false;
                }
            }

            if(otherPlayer.isAlive()) {
                SDL_Rect otherPlayerRect= otherPlayer.getRect();
                if(SDL_HasIntersection(&enemyRect, &otherPlayerRect)) {
                    validPosition= false;
                }
            }

            for(EnemyTank& enemy : enemyTanks) {
                SDL_Rect enemyHitbox= enemy.getRect();
                if(SDL_HasIntersection(&enemyRect, &enemyHitbox)) {
                    validPosition= false;
                    break;
                }
            }

            if(x< TILE_SIZE || x> SCREEN_WIDTH- TILE_SIZE- TILE_SIZE ||
                y< TILE_SIZE || y> SCREEN_HEIGHT- TILE_SIZE- TILE_SIZE) {
                validPosition= false;
            }
        }
        if (validPosition) {
            enemyTanks.emplace_back(x, y);
        }
    }
}
 // Cập nhật boss
void Game::updateBosses() {
    for(BossTank& boss : bossTanks) {
        boss.move(walls, player, otherPlayer, enemyTanks, bossTanks);
        boss.shoot();
    }
}
 // Sinh boss
void Game::generateBosses(int numBosses) {
    for(int i= 0; i< numBosses; i++) {
        bool validPosition= false;
        int x, y;
        while(!validPosition) {
            x= (rand() % ((SCREEN_WIDTH- 2 * TILE_SIZE)/ TILE_SIZE))* TILE_SIZE+ TILE_SIZE;
            y= (rand() % ((SCREEN_HEIGHT- 2 * TILE_SIZE)/ TILE_SIZE))* TILE_SIZE+ TILE_SIZE;
            SDL_Rect bossRect= {x, y, TILE_SIZE* 2, TILE_SIZE* 2};
            validPosition= true;

            for(Wall& wall : walls) {
                SDL_Rect wallRect= wall.getRect();
                if(SDL_HasIntersection(&bossRect, &wallRect)) {
                    validPosition= false;
                    break;
                }
            }

            if(player.isAlive()) {
                SDL_Rect playerRect= player.getRect();
                if(SDL_HasIntersection(&bossRect, &playerRect)) {
                    validPosition= false;
                }
            }

            if(otherPlayer.isAlive()) {
                SDL_Rect otherPlayerRect= otherPlayer.getRect();
                if(SDL_HasIntersection(&bossRect, &otherPlayerRect)) {
                    validPosition= false;
                }
            }

            for(EnemyTank& enemy : enemyTanks) {
                SDL_Rect enemyHitbox= enemy.getRect();
                if(SDL_HasIntersection(&bossRect, &enemyHitbox)) {
                    validPosition= false;
                    break;
                }
            }

            for(BossTank& boss : bossTanks) {
                SDL_Rect bossHitbox= boss.getRect();
                if(SDL_HasIntersection(&bossRect, &bossHitbox)) {
                    validPosition= false;
                    break;
                }
            }

            if(x< TILE_SIZE || x+ TILE_SIZE* 2> SCREEN_WIDTH- TILE_SIZE ||
                y< TILE_SIZE || y+ TILE_SIZE* 2> SCREEN_HEIGHT- TILE_SIZE) {
                validPosition= false;
            }
        }
        if (validPosition) {
            bossTanks.emplace_back(x, y);
        }
    }
}
