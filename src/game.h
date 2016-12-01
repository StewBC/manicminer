/*
 *  game.h
 *  ManicMiner
 *
 *  Created by Stefan Wessels, Dec. 2010.
 *
 */

#ifndef _GAME_H_
#define _GAME_H_

int main(void);
void mainLoop(void);
void setup(void);
void prepLevel(void);
char runGame(void);
char buildTracking(char bx);
void buildPowerBeamPath(void);
char checkCollision(char bufferValue);
void doFrameAndClassForSprite(SpriteData *sprt);
void moveSprites(void);
BOOL moveWilly(void);

#endif