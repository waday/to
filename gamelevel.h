#pragma once

#include "gamecore.h"
#include "constants.h"
#include "util.h"
#include "actor.h"
#include "assets.h"

#define INDEX_NOT_FOUND (255)

struct Player;

struct GameLevel {
  GameCore& core;

  enum {
    DIFFICULTY_EASY = 1,
    DIFFICULTY_NORMAL,
    DIFFICULTY_HARD,
  };
  int difficulty = DIFFICULTY_EASY;
  
  GameLevel(GameCore& core) : core(core) {
  }

  inline int getDifficulty() { return difficulty; }
  inline unsigned long frameCount() const { return frames; }
  inline void setGameover() {
    if(gameoverCount < 0) {
      gameoverCount = 0;
    }
  }
  inline bool isGameover() const { return gameoverCount >= 0; }
  inline unsigned int getScore() const { return score; }

  void onEntry() {
    frames = 0;
    score  = 0;
    scoreRate = 1;
    gameoverCount = -1;
    
    player.initialize();
    inactivateCharacters<Bullet>(bullets, BULLET_MAX);
    inactivateCharacters<Bomb>(bombs, PARTICLE_MAX);
    inactivateCharacters<Particle>(particles, PARTICLE_MAX);
    randomSeed(core.frameCount());
  }
  bool loop() {    
    // === move ===
    player.move(*this);
    moveCharactersWithoutContext<Bullet>(bullets, BULLET_MAX);
    moveCharactersWithoutContext<Bomb>(bombs, BOMB_MAX);
    moveCharactersWithoutContext<Particle>(particles, PARTICLE_MAX);

    // === draw ===
    player.draw(*this);
    drawCharacters<Bullet>(bullets, BULLET_MAX);
    drawCharacters<Bomb>(bombs, BOMB_MAX);
    drawCharacters<Particle>(particles, PARTICLE_MAX);

    // === spawn ===
    if(random(100) > 98) {
       firingBullet();
    }
    if(random(100) > 90) {
       throwingBomb();
    }

    // === check collision ===
    // Bullet vs Player
    for(byte i = 0; i < BULLET_MAX; ++i) {
      if(!player.isExist() || !player.isVisible()) { break; }
      if(!bullets[i].isExist()) { continue; }
      if(Collision(bullets[i].fieldX(), bullets[i].fieldY(), bullets[i].W, bullets[i].H, 
                   player.fieldX(), player.fieldY(), player.W, player.H)) {
        bullets[i].onHit(*this);
        player.onHit(*this);
      }
    }
    
    // Bomb vs Player
    for(byte i = 0; i < BOMB_MAX; ++i) {
      if(!player.isExist() || !player.isVisible()) { break; }
      if(!bombs[i].isExist()) { continue; }
      if(Collision(bombs[i].fieldX(), bombs[i].fieldY(), bombs[i].W, bombs[i].H, 
                   player.fieldX(), player.fieldY(), player.W, player.H)) {
        bombs[i].onHit(*this);
        player.onHit(*this);
      }
    }

    // === print info ===
    // score
    char text[32];
    sprintf(text, "%dpt", score);
    core.arduboy.setCursor(1, 1);
    core.arduboy.print(text);

    // score rate
    char text2[2];
    sprintf(text2, "x%d", round(scoreRate));
    core.arduboy.setCursor(SCREEN_W - 13, 1);
    core.arduboy.print(text2);

    // === gameover ===
    if(gameoverCount > 100) {
      core.setCursor(SCREEN_W / 2 - 24, SCREEN_H / 2 - 3);
      core.print("GAMEOVER");
    }

    // === post process ===
    // clock
    ++frames;
    if(gameoverCount >= 0) { ++gameoverCount; }

    // exit game if return true
    if(gameoverCount > 100 && (core.pressed(BTN_A) || core.pressed(BTN_B))) {
      return true;  // skip gameover text;
    }
    return gameoverCount >= 300;
  }

  void addScore(unsigned int plus) {
    plus = round(plus * scoreRate);
    if(score + plus < score) {
      score = 0xffff; // count stop
    }
    else if(gameoverCount < 0) {
      score += plus;
    }
  }

  void addScoreRate(float plus) {
    scoreRate = scoreRate + plus;
    if(scoreRate >= 5) {
      scoreRate = 5; // stop add
    }
  }

  void decreaseScoreRate(float minus) {
    scoreRate = scoreRate - minus;
    if(scoreRate <= 1) {
      scoreRate = 1; // stop decrease
    }
  }

  void resetScoreRate() {
    scoreRate = 1;
  }

  float getPlayerAngle(const char bx, const char by) const {
    return atan2(player.fieldY() - by, player.fieldX() - bx);
  }

  void firingBullet() {
    const byte d = getDifficulty();
    if(d >= DIFFICULTY_EASY) {
      //const byte  bulletType = d;
      //const byte bulletType = random(2);
      const byte bulletType = 0;
      // position randimize
      byte x;
      if(random(2) == 0) {
        x = SCREEN_W + Bullet::W;
      } else {
        x = -Bullet::W;
      }
      const byte y = SCREEN_H / 2 - 20 + random(51);
      fireBullet(x, y, getPlayerAngle(x, y), bulletType);
    }
  }

  void throwingBomb() {
    const byte d = getDifficulty();
    if(d >= DIFFICULTY_EASY) {;
      const byte bombType = 0;
      // position randimize
      byte x;
      if(random(2) == 0) {
        x = SCREEN_W + Bomb::W*2;
      } else {
        x = -Bomb::W*2;
      }
      const byte y = SCREEN_H / 2 - 20 + random(51);
      spawnBomb(x, y, getPlayerAngle(x, y), bombType);
    }
  }

  // spawn actors
  void fireBullet(const char x, const char y, const float radian, const byte type) {
    const byte i = searchAvailableIndex<Bullet>(bullets, BULLET_MAX);
    if(i != INDEX_NOT_FOUND) {
      bullets[i].initialize(x, y, radian, type);
    }
  }
  void spawnBomb(const char x, const char y, const float radian, const byte type) {
    const byte i = searchAvailableIndex<Bomb>(bombs, BOMB_MAX);
    if(i != INDEX_NOT_FOUND) {
      bombs[i].initialize(x, y, radian, type);
    }
  }
  void spawnParticle(const int x, const char y, const byte type) {
    if(x > SCREEN_W || y > SCREEN_H) { return; }
    const byte i = searchAvailableIndex<Particle>(particles, PARTICLE_MAX);
    if(i != INDEX_NOT_FOUND) {
      particles[i].activate(x, y);
      particles[i].type = type;
      switch(type) {
        case PARTICLE_EXPLOSION: particles[i].limit = 12; break;
        default:                 particles[i].limit = 50; break;
      }
    }
  }
  
  private:
  // inactivate
  template<typename T> inline void inactivateCharacters(T pool[], const byte n) {
    for(byte i = 0; i < n; ++i) {
      pool[i].inactivate();
    }
  }

  // move
  template<typename T> inline void moveCharactersWithoutContext(T pool[], const byte n) {
    for(byte i = 0; i < n; ++i) {
      if(pool[i].isExist()) { pool[i].move(*this); }
    }
  }

  // draw
  template<typename T> inline void drawCharacters(T pool[], const byte n) {
    for(byte i = 0; i < n; ++i) {
      if(pool[i].isExist()) { pool[i].draw(*this); }
    }
  }

  // search
  template<typename T> inline byte searchAvailableIndex(const T pool[], const byte n) const {
    for(byte i = 0; i < n; ++i) {
      if(!pool[i].isExist()) { return i; }
    }
    return INDEX_NOT_FOUND;  // not found
  }

  private:
  Player  player;
  Bullet  bullets[BULLET_MAX];
  Bomb    bombs[BOMB_MAX];
  Particle   particles[PARTICLE_MAX];

  unsigned long frames;
  unsigned int  score;
  float scoreRate;
  int   gameoverCount;
};

