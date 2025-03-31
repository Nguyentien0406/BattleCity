#include "Bullet.h"
#include "Game.h"

using namespace std;
// Constructor: Khởi tạo đạn với các thông số cơ bản
Bullet::Bullet(int x, int y, int dir, bool fromPlayer, bool fromBoss) : speed(BULLET_SPEED), direction(dir), collided(false), isFromPlayer(fromPlayer), isFromBoss(fromBoss) {
    rect.x= x;
    rect.y= y;
    if(!fromBoss) {
        rect.w= BULLET_SIZE;// Kích thước đạn thường
        rect.h= BULLET_SIZE;
    } else {
        rect.w= BULLET_SIZE* 1.5;// Kích thước đạn boss (lớn hơn)
        rect.h= BULLET_SIZE* 1.5;
    }
}
// Vẽ đạn theo hướng di chuyển
void Bullet::render(SDL_Renderer* renderer, SDL_Texture* spriteSheet) const{
    SDL_Rect srcRect;
    switch(direction) {
        case 0:
            srcRect= {163, 377, 3, 4};
            break;
        case 1:
            srcRect= {167, 378, 3, 4};
            break;
        case 2:
            srcRect= {171, 378, 4, 3};
            break;
        case 3:
            srcRect= {176, 378, 4, 3};
            break;
    }
    SDL_RenderCopy(renderer, spriteSheet, &srcRect, &rect);
}
// Cập nhật trạng thái đạn và xử lý va chạm
void Bullet::update(vector<Wall>& walls, PlayerTank& player, PlayerTank& otherPlayer, vector<EnemyTank>& enemies, vector<BossTank>& bosses, Game& game) {
    switch(direction) {  // Di chuyển đạn theo hướng
        case 0: rect.y-= speed; break;
        case 1: rect.y+= speed; break;
        case 2: rect.x-= speed; break;
        case 3: rect.x+= speed; break;
    }
    // Kiểm tra va chạm với tường
    for(Wall& wall : walls) {
        if(SDL_HasIntersection(&rect, &wall.getRect())) {
            if(wall.isBreakable()) {
                if(!isFromBoss) {
                    wall.takeDamage(direction);
                    if(wall.isDestroyed()) {
                        game.removeWall(wall.getRect().x, wall.getRect().y);
                    }
                } else{
                    game.removeWall(wall.getRect().x, wall.getRect().y);
                }
                collided= true;
                return;
            } else if(wall.isCamouflaged()) {
                continue;
            } else{
                collided= true;
                return;
            }
        }
    }

    // Kiểm tra va chạm với người chơi 2
    if(player.isAlive()) {
        SDL_Rect playerRect= player.getRect();
        if(!isFromPlayer && SDL_HasIntersection(&rect, &playerRect)) {
            if(!isFromBoss) {
                player.takeDamage();
                if(player.isDestroyed()) {
                    player.setAlive(false);
                    game.addExplosion(player.getRect().x+ player.getRect().w/2 , player.getRect().y+ player.getRect().h/2, false);
                    game.playExplosionSound();
                    game.setGameOver(player, otherPlayer);
                }
            } else{
                player.setAlive(false);
                game.addExplosion(player.getRect().x+ player.getRect().w/2 , player.getRect().y+ player.getRect().h/2, false);
                game.playExplosionSound();
                game.setGameOver(player, otherPlayer);
            }
            collided= true;
            return;
        }
    }

    // Kiểm tra va chạm với người chơi 1
    if(otherPlayer.isAlive()) {
        SDL_Rect otherPlayerRect= otherPlayer.getRect();
        if(!isFromPlayer && SDL_HasIntersection(&rect, &otherPlayerRect)) {
            if(!isFromBoss) {
                otherPlayer.takeDamage();
                if(otherPlayer.isDestroyed()) {
                    otherPlayer.setAlive(false);
                    game.addExplosion(otherPlayer.getRect().x+ otherPlayer.getRect().w/2 , otherPlayer.getRect().y+ otherPlayer.getRect().h/2, false);
                    game.playExplosionSound();
                    SDL_Delay(200);
                    game.setGameOver(player, otherPlayer);
                }
            } else{
                otherPlayer.setAlive(false);
                game.addExplosion(otherPlayer.getRect().x+ otherPlayer.getRect().w/2 , otherPlayer.getRect().y+ otherPlayer.getRect().h/2, false);
                game.playExplosionSound();
                SDL_Delay(200);
                game.setGameOver(player, otherPlayer);
            }
            collided= true;
            return;
        }
    }

    for(auto it= enemies.begin(); it!= enemies.end(); ++it) {
        SDL_Rect enemyRect= it->getRect();
        if(isFromPlayer&& SDL_HasIntersection(&rect, &enemyRect)) {
            it->takeDamage();
            if(it->isDestroyed()) {
                game.addExplosion(it->getRect().x+ it->getRect().w/2, it->getRect().y+ it->getRect().h/2, false);
                game.playExplosionSound();
                enemies.erase(it);
            }
        collided= true;
        return;
        }
    }

    for(auto it= bosses.begin(); it!= bosses.end(); ++it) {
        SDL_Rect bossRect= it->getRect();
        if(isFromPlayer&& SDL_HasIntersection(&rect, &bossRect)) {
            it->takeDamage();
            if(it->isDestroyed()) {
                game.addExplosion(it->getRect().x+ it->getRect().w/2, it->getRect().y+ it->getRect().h/2, true);
                game.playExplosionSound();
                bosses.erase(it);
            }
        collided= true;
        return;
        }
    }

    if(isFromPlayer) {
        for(auto& enemy : enemies) {
            vector<Bullet>& enemyBullets= enemy.getBullets();
            for(auto& enemyBullet : enemyBullets) {
                SDL_Rect enemyBulletRect= enemyBullet.getRect();
                if(SDL_HasIntersection(&rect, &enemyBulletRect)) {
                    collided= true;
                    enemyBullet.setCollided(true);
                    break;
                }
            }
        }

        for(auto& boss : bosses) {
            vector<Bullet>& bossBullets= boss.getBullets();
                for(auto& bossBullet : bossBullets) {
                    SDL_Rect bossBulletRect= bossBullet.getRect();
                    if(SDL_HasIntersection(&rect, &bossBulletRect)) {
                        collided= true;
                        break;
                    }
                }
        }
    }
    // Kiểm tra đạn ra khỏi biên màn hình
    if(rect.x<= TILE_SIZE|| rect.x+ BULLET_SIZE>= SCREEN_WIDTH- TILE_SIZE ||
        rect.y<= TILE_SIZE|| rect.y+ BULLET_SIZE>= SCREEN_HEIGHT- TILE_SIZE) {
        collided= true;
    }
}

bool Bullet::hasCollided() const {
    return collided;
}
