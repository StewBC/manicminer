/*
 *  render.h
 *  ManicMiner
 *
 *  Created by Stefan Wessels, Dec. 2010.
 *
 */

#ifndef _RENDER_H_
#define _RENDER_H_

void renderSetup(void);
void renderPrepBackgroundChars(void);
void renderPrepBackgroundScreen(void);
void renderPrepSprites(void);
void renderPrepLevel(void);
void beginFrame(void);
void syncEndFrame(void);
void andSpriteMask(char mask);
void setColourRam(int index, char colour);
void incColourRamBy(int index, char colour);
void setScreenRam(int index, char character);
char incScreenRam(int index);
void drawElements(void);
void undrawLife(void);
void drawScores(void);
void drawSprites(void);
char checkSpriteCollisions(void);
void renderFrame(void);
void renderFrontEnd(char feState);
void setupLostScreen(void);
char renderLostScreen(void);
void readKeys(void);

#endif