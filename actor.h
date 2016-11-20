#pragma once

#include "constants.h"

// exists threshold
#define EXIST_THRESHOLD -256

// speed of bullets
#define BULLET_TYPE0_SPD (1)
#define BULLET_TYPE1_SPD (1.5)
#define BULLET_TYPE2_SPD (2)
#define BULLET_TYPE3_SPD (2.5)

enum {
  PARTICLE_EXPLOSION = 0,
  PARTICLE_TEN_POINT,
};

enum {
  DIRECTION_RIGHT = 1,
  DIRECTION_LEFT = -1,
};

typedef float fixed;
typedef unsigned char byte;
struct GameLevel;

struct Player {
  enum {
    STATE_LAND = 0,
    STATE_JUMP,
    STATE_FALL,
  };
  byte moveState;
  
  enum {
    INVISIBLE = 0,
    VISIBLE
  };
  byte visibleState;

  enum {
    STATE_ARMOR_NORMAL = 0,
    STATE_ARMOR_UNTIBOMB,
    STATE_ARMOR_AUTOHEAL,
    STATE_ARMOR_FLASH,
  };
  byte armorState;
  
  static const byte W = 15;
  static const byte H = 29;

  fixed   x, y, vx, vy;

  int hp;
  bool exist = false;
  
  inline void inactivate() { exist = false; }
  inline bool isExist() const { return exist; }
  inline char fieldX() const { return x + W/2; }
  inline char fieldY() const { return y + H/2; }
  inline bool isVisible() { return visibleState == VISIBLE; }
  inline void setVisible() { visibleState = VISIBLE; }
  inline void setInvisible() { visibleState = INVISIBLE; }
  
  void initialize();
  void move(GameLevel& context);
  void draw(GameLevel& context) const;
  void dron(GameLevel& context);
  void onHit(GameLevel& context);

  private:
  void moveX(GameLevel& context);
  void moveY(GameLevel& context);
  void land(GameLevel& context);
  void jump(GameLevel& context);
  void fall(GameLevel& context);
};


struct Bullet {
  static const byte W = 2;
  static const byte H = 2;

  fixed x, y, vx, vy;
  byte type;
  bool exist = false;
  
  inline char fieldX() const { return x + W/2; }
  inline char fieldY() const { return y + H/2; }
  inline void inactivate() { exist = false; }
  inline bool isExist() const { return exist; }
  void initialize(const char x, const char y, const float radian, const byte type);
  void move(GameLevel& context);
  void draw(GameLevel& context) const;
  void onHit(GameLevel& context);
};


struct Bomb {
  static const byte W = 2;
  static const byte H = 2;

  fixed x, y, vx, vy;
  byte type;
  bool exist = false;
  
  inline char fieldX() const { return x + W/2; }
  inline char fieldY() const { return y + H/2; }
  inline void inactivate() { exist = false;  }
  inline bool isExist() const { return exist; }
  void initialize(const char x, const char y, const float radian, const byte type);
  void move(GameLevel& context);
  void draw(GameLevel& context) const;
  void onHit(GameLevel& context);
};


struct Particle {
  char x, y;
  byte type;
  byte limit;
  bool exist = false;

  inline void activate(const char x, const char y) {
    exist = true;
    this->x = x;
    this->y = y;
  }
  inline void inactivate() { exist = false; }
  inline bool isExist() const { return exist; }
  void move(GameLevel& context);
  void draw(GameLevel& context) const;
};
