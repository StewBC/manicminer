/*
 *  render.h
 *  ManicMiner
 *
 *  Created by Stefan Wessels, Dec. 2010.
 *
 */

#ifndef _RENDER_H_
#define _RENDER_H_

void renderSetup();
void renderPrepBackgroundChars();
void renderPrepBackgroundScreen();
void renderPrepSprites();
void renderPrepLevel();
void beginFrame();
void syncEndFrame();
void andSpriteMask(char mask);
void setColourRam(int index, char colour);
void incColourRamBy(int index, char colour);
void setScreenRam(int index, char character);
char incScreenRam(int index);
void drawElements();
void undrawLife();
void drawScores();
void drawSprites();
char checkSpriteCollisions();
void renderFrame();
void renderFrontEnd(char feState);
void setupLostScreen();
char renderLostScreen();
void readKeys();

#endif