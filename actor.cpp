#include "actor.h"
#include "gamelevel.h"
#include "assets.h"

// =================
// = Player
// =================
void Player::initialize() {
  x  = SCREEN_W/2 - W/2;
  y  = SCREEN_H - H;
  vx = 0;
  vy = 0;

  hp = 100;
  exist = true;

  moveState = STATE_LAND;
  visibleState = VISIBLE;
  armorState = STATE_ARMOR_NORMAL;
}

void Player::move(GameLevel& context)
{
  if(context.isGameover()) { return; }
#ifndef PLAYER_DO_NOT_MOVE
  moveX(context);
  moveY(context);
#endif
}

void Player::draw(GameLevel& context) const {
  if(context.isGameover() && context.frameCount() % 5 == 0) {
    context.spawnParticle(x + random(W) - W/4, y + random(H) - H/4, PARTICLE_EXPLOSION);  
  }
  dron(context);
  if(isVisible()) {
    if(context.frameCount() % 30 == 0) { context.addScore(1); }
    // close the door
    context.core.arduboy.drawRect(x, y, W, H, 1);
    context.core.arduboy.drawCircle(x + W - W/3, y + H/2 + 1, 1, 1);
  } else {
    // open the door
    context.core.arduboy.fillRect(x, y, W, H, 1);
    context.core.arduboy.drawRect(x - W, y, W, H, 1);
    context.core.arduboy.drawCircle(x - W + W/3, y + H/2 + 1, 1, 1);

    char text[3];
    sprintf(text, "%d", hp);
    context.core.arduboy.setCursor(x + 2, y - 12);
    context.core.arduboy.print(text);    
  }
}

void Player::dron(GameLevel& context) {
  setVisible();
  if(context.core.hold(BTN_B) && hp >= 1 && !context.isGameover()) {
    setInvisible();
    if(context.frameCount() % 10 == 0) { hp -= 1; }
    context.decreaseScoreRate(0.1);
  }
}

void Player::onHit(GameLevel& context) {
  context.core.tone(150, 250);
  context.setGameover();
  //inactivate();
}

void Player::moveX(GameLevel& context) {
    if(context.core.pressed(BTN_R)) { vx += 1; }
    if(context.core.pressed(BTN_L)) { vx -= 1; }

    if(vx != 0) {
      if(vx > 0) { vx-=0.5; }
      else { vx+=0.5; }
    }

    x += vx;

    if(x+W >= SCREEN_W) {
      x  = SCREEN_W - W;
      vx = 0;
    }

    if(x < 0) {
      x  = 0;
      vx = 0;
    }
}

void Player::moveY(GameLevel& context)
{
    switch(moveState)
    {
    case STATE_LAND: land(context); break;
    case STATE_JUMP: jump(context); break;
    case STATE_FALL: fall(context); break;
    default:
        break;
    }
}

void Player::land(GameLevel& context)
{
    if(!context.core.pressed(BTN_A))
   {
        return;
    }

    context.addScoreRate(0.5);
    vy   = -14;
    moveState = STATE_JUMP;
}

void Player::jump(GameLevel& context)
{
    vy += 2;
    y  += vy;

    if(vy > 0)
    {
        moveState = STATE_FALL;
    }
}

void Player::fall(GameLevel& context)
{
    vy += 2;
    y  += vy;

    if(y+H >= SCREEN_H)
    {
        y  = SCREEN_H - H;
        vy = 0;

        moveState = STATE_LAND;
    }
}


// =================
// = Bullet
// =================
void Bullet::initialize(const char x, const char y, const float radian, const byte type) {
  this->x    = x;
  this->y    = y;
  this->type = type;
  exist = true;

  const byte SPEEDS[] = {BULLET_TYPE0_SPD, BULLET_TYPE1_SPD, BULLET_TYPE2_SPD, BULLET_TYPE3_SPD};
  byte speed = SPEEDS[type];

  switch(type) {
    case 0: {
      if (random(2) == 0) {
        vx = speed;
      } else {
        vx = speed * -1 ;
      }
      //vx = cos(radian) * speed;
      vy = 0;
    } break;
    case 1: {
      vx = cos(radian) * speed;
      vy = sin(radian) * speed;
    } break;
  }
    
}

void Bullet::move(GameLevel& context) {
  x += vx;
  y += vy;

  // frame out
  static const char MARGIN = W*2;
  if((byte)fieldX() < -MARGIN || (byte)fieldX() > SCREEN_W  + MARGIN) {
    inactivate();  
  }
}

void Bullet::draw(GameLevel& context) const {
  const byte frame = context.frameCount() / 3 % 2;

  const byte* bitmaps[] = {bitmapSbullet0, bitmapSbullet1};
  context.core.drawBitmap(fieldX() - bitmapSbullet0[0]/2, fieldY() - bitmapSbullet0[1]/2, bitmaps[frame], 2);
}

void Bullet::onHit(GameLevel& context) {
#ifndef LOW_FLASH_MEMORY
  context.core.drawBitmap(fieldX() - bitmapCircle[0]/2, fieldY() - bitmapCircle[1]/2, bitmapCircle, 2);
#endif
  inactivate();
}


// =================
// = Bomb
// =================
void Bomb::initialize(const char x, const char y, const float radian, const byte type) {
  this->x    = x;
  this->y    = y;
  this->type = type;
  exist = true;

  switch(type) {
    case 0: {
      if (random(2) == 0) {
        vx = (1 + random(2)) * 1;
      } else {
        vx = (1 + random(2)) * -1;
      }
      //vx = cos(radian) * (1 + random(2));
      vy = -1 - random(2);
    } break;
  }

}

void Bomb::move(GameLevel& context) {
  vx *= 0.99;
  x += vx;
  vy += 0.05;
  y += vy;

  // frame out
  static const char MARGIN = H * 2;
  if((byte)fieldY() > SCREEN_H - MARGIN) {
    if((byte)fieldX() >= 0 && (byte)fieldX() <= SCREEN_W) {
      context.spawnParticle(fieldX(), fieldY() - 2, PARTICLE_EXPLOSION);
      context.addScoreRate(0.2);
    }
    inactivate();
  }

  if((byte)fieldY() < 0) { vy *= 0.1; }
}

void Bomb::draw(GameLevel& context) const {
  const byte frame = context.frameCount() / 3 % 2;

  const byte* bitmaps[] = {bitmapSbullet0, bitmapSbullet1};
  context.core.drawBitmap(fieldX() - bitmapSbullet0[0]/2, fieldY() - bitmapSbullet0[1]/2, bitmaps[frame], 2);
}

void Bomb::onHit(GameLevel& context) {
#ifndef LOW_FLASH_MEMORY
  context.core.drawBitmap(fieldX() - bitmapCircle[0]/2, fieldY() - bitmapCircle[1]/2, bitmapCircle, 2);
#endif
  inactivate();
}


// =================
// = Particle
// =================
void Particle::move(GameLevel& context) {
  --limit;
  if(limit <= 0) {
    inactivate();
  }
}

void Particle::draw(GameLevel& context) const {
  switch(type) {
    case 0: {
      if(limit > 8) {
        context.core.drawBitmap(x, y, bitmapExplosion0, 2);
      }
      else if(limit > 4) {
        context.core.drawBitmap(x, y, bitmapExplosion1, 2);
      }
      else {
        context.core.drawBitmap(x, y, bitmapExplosion2, 2);
      }
    } break;
    
    default: {
      context.core.drawBitmap(x, y, bitmapTen, 2);
    } break;
  }
}
