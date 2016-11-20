#pragma once

//#define DEBUG

// define to omit some specifications in order to reduce flash memory
//#define LOW_FLASH_MEMORY

// define if use EEPROM for saving ranking data
#define USE_RANKING_EEPROM

// define if disable BGM (you dont need / reduce flash memory)
//#define DISABLE_MUSIC

// file index bases on sample sketch "ArduBreakout"
// > Each block of EEPROM has 10 high scores, and each high score entry
// > is 5 bytes long:  3 bytes for initials and two bytes for score.
// so base address is EEPROM_FILE_INDEX * 5(bytes) * 10(hi-scores)
#define EEPROM_FILE_INDEX  (3)

// each character numbers
#define BULLET_MAX       (4)
#define BOMB_MAX         (6)
#define PARTICLE_MAX     (10)

// TODO: impl gamelevel
//#define START_LIVES      (1)

#define SCENE_ENTRY_WAIT (30)

// screen size
#define SCREEN_W       WIDTH
#define SCREEN_H       HEIGHT

// player setting
//#define PLAYER_DO_NOT_MOVE
