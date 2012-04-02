/*
 *  game.h
 *  ManicMiner
 *
 *  Created by Stefan Wessels, Dec. 2010.
 *
 */

#ifndef _GAME_H_
#define _GAME_H_

int main();
void mainLoop();
void setup();
void prepLevel();
char runGame();
char buildTracking(char bx);
void buildPowerBeamPath();
char checkCollision(char bufferValue);
void doFrameAndClassForSprite(SpriteData *sprt);
void moveSprites();
BOOL moveWilly();

#endif