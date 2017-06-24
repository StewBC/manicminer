/*
 *  render.c
 *  ManicMiner
 *
 *  Created by Stefan Wessels, Dec. 2010.
 *
 */

#include <stdio.h>
#include <string.h>
#include <cbm.h>
#include "data.h"
#include "globals.h"
#include "render.h"

//-------------------------------------------------------------------------
void renderSetup() 
{
	int  i;
	char y, *pDest;
	const char *pData;
	char *spriteDataLoc = SPRITE_RAM;
	
	memset(SCREEN_RAM, BlankChar, ScreenX*ScreenY);

	CIA2.ddra |= 0x03;
	CIA2.pra = (CIA2.pra & 0xfc) | (3-(VIC_BASE_RAM / 0x4000));
#ifdef __C64__
	VIC.addr = ((((int)(SCREEN_RAM - VIC_BASE_RAM) / 0x0400) << 4) + (((int)(CHARMAP_RAM - VIC_BASE_RAM) / 0x0800) << 1));
#endif
#ifdef __C128__
	*((char*)0xa04) = 0;
	*((char*)0xa2c) = ((((int)(SCREEN_RAM - VIC_BASE_RAM) / 0x0400) << 4) + (((int)(CHARMAP_RAM - VIC_BASE_RAM) / 0x0800) << 1));
#endif
	*MEM_KRNL_PRNT = (int)SCREEN_RAM / 256;
	CIA1.cra = (CIA1.cra & 0xc0) | 0x10;
	CIA1.crb |= 0x40;

	pData = &fontCharMapLower[0][0];
	for(i = 0; i < CharY * fontSizeLower; ++i)
		CHARMAP_RAM[(CharY*WorldSwitchLeft2)+i] = *pData++;

	pData = &fontCharMapUpper[0][0];
	for(i = 0; i < CharY * fontSizeUpper; ++i)
		CHARMAP_RAM[(CharY*UpperFontStart)+i] = *pData++;
		
	for(i = 0; i < 4; ++i)
	{
		pData = &spriteImages[i][0];
		pDest = &CHARMAP_RAM[CharY * (fontSizeUpper + UpperFontStart) + (4 * CharY * i)];
		for(y = 0; y < CharY; ++y)
		{
			pDest[        y] = pData[          (2*y)];
			pDest[  CharY+y] = pData[        1+(2*y)];
			pDest[2*CharY+y] = pData[2*CharY  +(2*y)];
			pDest[3*CharY+y] = pData[2*CharY+1+(2*y)];
		}
	}
		
	for(i = 0; i < CharY; ++i)
	{
		CHARMAP_RAM[i] = 0xff;
		CHARMAP_RAM[(CharY*WorldSwitchLeft1)+i] = CHARMAP_RAM[(CharY*WorldSwitchLeft2)+i];
	}

	VIC.spr0_color = COLOR_WHITE;
	bzero(spriteDataLoc, SpriteBytes * LevelSpriteFrames);
	for(i = 0; i < NumWillyFrames; ++i)
	{
		pData = spriteImages[i];
		for(y = 0; y < SpritePixelsY; ++y)
		{
			*spriteDataLoc++ = *pData++;
			*spriteDataLoc++ = *pData++;
			++spriteDataLoc;
		}
		spriteDataLoc += SpriteBytes-(SpritePixelsY*3);
	}
	
	for(i = 0; i < NumLevels; ++i)
	{
		char spriteCount = level2Sprite[i].count;
		
		if(spriteCount > MaxSprites - 1)
		{
			level2Sprite[i].count = MaxSprites - 1;
			level2Sprite[i].index += (spriteCount - (MaxSprites - 1));
		}
	}
}

//-------------------------------------------------------------------------
void renderPrepBackgroundChars()
{
	unsigned char i, y, cell, index, roll;
	const char *charData;
	
	index = CharY;
	for(i = 0; i < LevelTiles - 1; ++i)
	{
		cell = levelCharIndex[m_level][i];
		charData = &backCharMap[cell][0];

		for(y = 0; y < CharY; ++y)
		{
			CHARMAP_RAM[index] = *charData;
			++index;
			++charData;
		}
	}
	
	index = CharY * WorldKey1;
	cell = levelCharIndex[m_level][LevelTiles-1];
	charData = &backCharMap[cell][0];
	for(i = 0; i < MaxKeysPerLevel; ++i)
	{
		for(y = 0; y < CharY; ++y)
		{
			CHARMAP_RAM[index] = charData[y];
			++index;
		}
	}

	index = CharY * WorldCollapse1;
	cell = levelCharIndex[m_level][WorldCollapse-1];
	charData = &backCharMap[cell][0];
	for(i = CharY; i; --i)
	{
		for(y = 0; y < (CharY - i); ++y)
		{
			CHARMAP_RAM[index] = 0;
			++index;
		}
		for(y = 0; y < i; ++y)
		{
			CHARMAP_RAM[index] = charData[y];
			++index;
		}
	}

	index = CharY * WorldConveyor1;
	cell = levelCharIndex[m_level][WorldConveyor-1];
	charData = &backCharMap[cell][0];

	for(y = 0; y < CharY; ++y)
	{
		CHARMAP_RAM[index] = charData[y];
		++index;
		CHARMAP_RAM[CharY*WorldAir2+y] = CHARMAP_RAM[CharY*WorldAir1+y];
	}
	
	for(i = 0; i < (CharY-1)/2; ++i)
	{
		charData = &CHARMAP_RAM[CharY*(WorldConveyor1+i)];
		for(y = 0; y < CharY; ++y)
		{
			cell = charData[y];
			if(0 == y)
			{
				roll = cell & 0xc0;
				cell <<= 2;
				cell |= roll >> 6;
			}
			else if(2 == y)
			{
				roll = cell & 0x3;
				cell >>= 2;
				cell |= roll << 6;
			}
			CHARMAP_RAM[index] = cell;
			++index;
		}
	}
}

//-------------------------------------------------------------------------
void renderPrepBackgroundScreen()
{
	char i, x, y, levelLen, run, cells, outChars, pokeChar;
	const char *dataStream;
	int yOffset;
	
	VIC.bordercolor = levelColour[m_level][COLOUR_Boarder];
	VIC.bgcolor0 = levelColour[m_level][COLOUR_Background];

	x = 0;
	y = 0;
	levelLen = levelBuffer[m_level][0];
	dataStream = &levelBuffer[m_level][1];
	yOffset = 4;
	while(levelLen)
	{
		cells = *dataStream++;
		--levelLen;
		if(cells >= 0x80)
		{
			run = cells & 0x7f;
			cells = *dataStream++;
			--levelLen;
		}
		else
			run = 1;
			
		while(run)
		{
			--run;
			outChars = cells;
			for(i = 0; i < 2; ++i)
			{
				pokeChar = outChars & 0x0f;
				if(WorldCollapse == pokeChar)
					pokeChar = WorldCollapse1;
				SCREEN_RAM[yOffset+x] = pokeChar;
				COLOUR_RAM[yOffset+x] = levelColour[m_level][outChars & 0x0f];
				if(++x >= LevelX)
				{
					x = 0;
					yOffset += ScreenX;
				}
				outChars >>= 4;
			}
		}
	}
	
	for(i = 0; i < LevelX; ++i)
	{
		pokeChar = levelNames[m_level][i];
		SCREEN_RAM[screenRowStart[TextRow]+i] = pokeChar;
		COLOUR_RAM[screenRowStart[TextRow]+i] = COLOR_YELLOW;
	}
		
	for(i = 0; i < AirStrLen; ++i)
	{
		SCREEN_RAM[screenRowStart[AirRow]+i] = szAir[i];
		COLOUR_RAM[screenRowStart[AirRow]+i] = COLOR_WHITE;
	}
	
	m_airHead = (m_airAmount/AirScaler) / CharX;	
	pokeChar = levelColour[m_level][COLOUR_Ground];
	memset(&SCREEN_RAM[screenRowStart[AirRow]+AirStrLen], WorldAir1, m_airHead+1);
	memset(&COLOUR_RAM[screenRowStart[AirRow]+AirStrLen], pokeChar, m_airHead+1);

	for(i = 0; i < MaxKeysPerLevel; ++i)
	{
		if(keyPositions[m_level][i].x != 255)
		{
			m_keysToFind++;
			SCREEN_RAM[screenRowStart[keyPositions[m_level][i].y]+keyPositions[m_level][i].x] = WorldKey1 + i;
			COLOUR_RAM[screenRowStart[keyPositions[m_level][i].y]+keyPositions[m_level][i].x] = COLOR_YELLOW;
		}
	}

	yOffset = screenRowStart[doorValues[m_level].y];
	yOffset += doorValues[m_level].x;
	cells = levelColour[m_level][COLOUR_Door];
	COLOUR_RAM[yOffset] = cells;
	++yOffset;
	COLOUR_RAM[yOffset] = cells;
	yOffset += ScreenX;
	COLOUR_RAM[yOffset] = cells;
	--yOffset;
	COLOUR_RAM[yOffset] = cells;
	
	if(LEVEL_Miner_Willy_meets_the_Kong == m_level || LEVEL_Return_of_the_Alien_Kong_Beast == m_level)
	{
		SCREEN_RAM[screenRowStart[switchPositions[0].y]+switchPositions[0].x] = WorldSwitchLeft1;
		SCREEN_RAM[screenRowStart[switchPositions[1].y]+switchPositions[1].x] = WorldSwitchLeft2;
		COLOUR_RAM[screenRowStart[switchPositions[0].y]+switchPositions[0].x] = COLOR_YELLOW;
		COLOUR_RAM[screenRowStart[switchPositions[1].y]+switchPositions[1].x] = COLOR_YELLOW;
	}

	memset(&COLOUR_RAM[screenRowStart[LivesRow]], COLOR_CYAN, ScreenX*2);
}

//-------------------------------------------------------------------------
void renderPrepSprites()
{
	char y, nSprite, index, frame, frames, spriteBit, msbMask, spriteOffset, usedIndex[8];
	int  iPos;
	char *spritePositions, *pSpriteDataPtrs, *pSpriteDataDest, *pSpriteColours;
	const char *pSpriteData;
	char spriteIndex = level2Sprite[m_level].index;
	char spriteCount = level2Sprite[m_level].count + spriteIndex;

	spritePositions = &VIC.spr1_x;
	pSpriteDataPtrs = SPRITEDATAPTRS+1;
	pSpriteColours = &VIC.spr1_color;
	spriteBit = 0;
	msbMask = 0;
	index = 0;
	spriteOffset = (int)(LVL_SPRITE_RAM - VIC_BASE_RAM) / SpriteBytes;
	pSpriteDataDest = LVL_SPRITE_RAM;

	VIC.spr0_x = 0;
	VIC.spr0_y = 0;
	for(nSprite = spriteIndex; nSprite < spriteCount; ++nSprite)
	{
		SpriteData *sprt = &spriteData[nSprite];
		spriteBit <<= 1;
		spriteBit |= 1;
		
		*pSpriteColours++ = sprt->colour;
		
		if(sprt->spriteClass & (SPClass_Kong | SPClass_Eugene))
			sprt->curEnabled = (nSprite - spriteIndex) + 1;
		
		if(!(sprt->spriteClass & SPClass_Vertical))
		{
			iPos = sprt->curPos + SpritePreX;
			y = sprt->otherAxis + SpritePreY;
		}
		else
		{
			iPos = sprt->otherAxis + SpritePreX;
			y = sprt->curPos + SpritePreY;
		}
		
		if(iPos > 255)
		{
			msbMask |= spriteBit<<1;
			iPos -= 256;
		}
		
		*spritePositions++ = iPos & 0xff;
		*spritePositions++ = y;

		m_spriteIndicies[index] = spriteOffset;
		for(frame = 0; frame < index; ++frame)
		{
			if(usedIndex[frame] == sprt->frameIndex)
			{
				usedIndex[index] = sprt->frameIndex;
				m_spriteIndicies[index] = m_spriteIndicies[frame];
				break;
			}
		}
		
		usedIndex[index] = sprt->frameIndex;
		*pSpriteDataPtrs++ = m_spriteIndicies[index];

		if(m_spriteIndicies[index] == spriteOffset)
		{
			pSpriteData = spriteImages[sprt->frameIndex];
			frames = sprt->spriteClass & SPClass_Full_Range ? 4 : 8;
			for(frame = 0; frame < frames; ++frame)
			{
				for(y = 0; y < SpritePixelsY; ++y)
				{
					*pSpriteDataDest++ = *pSpriteData++;
					*pSpriteDataDest++ = *pSpriteData++;
					*pSpriteDataDest++;
				}
				pSpriteDataDest += SpriteBytes-(SpritePixelsY*3);
				++spriteOffset;
			}
		}
		++index;
	}
	
	spriteBit <<= 1;
	spriteBit |= 1;
	VIC.spr_hi_x = msbMask;
	VIC.spr_ena = spriteBit;
	m_doorBit = 0x80;
	while(!(m_doorBit & spriteBit))
		m_doorBit >>= 1;
}

//-------------------------------------------------------------------------
void renderPrepLevel()
{
	m_conveyorFrame = m_frameCounter = 0;

	VIC.spr_ena = 0;
	VIC.ctrl1 &= 0xef;
	renderPrepBackgroundChars();
	renderPrepBackgroundScreen();
	VIC.ctrl1 |= 0x10;
	renderPrepSprites();
}

//-------------------------------------------------------------------------
void beginFrame()
{
	CIA1.crb &= 0xfe;
	CIA1.cra &= 0xfe;
	CIA1.ta_lo = 0xff;
	CIA1.tb_lo = 0xff;
	CIA1.ta_hi = 0x7f;
	CIA1.tb_hi = 0x7f;
	CIA1.crb |= 1;
	CIA1.cra |= 1;
}

//-------------------------------------------------------------------------
void syncEndFrame()
{
	while(CIA1.tb_lo > GameTimerHiMin);
}

//-------------------------------------------------------------------------
void andSpriteMask(char mask)
{
	VIC.spr_ena &= mask;
}

//-------------------------------------------------------------------------
void setColourRam(int index, char colour)
{
	COLOUR_RAM[index] = colour;
}

//-------------------------------------------------------------------------
void incColourRamBy(int index, char colour)
{
	COLOUR_RAM[index] += colour;
}

//-------------------------------------------------------------------------
void setScreenRam(int index, char character)
{
	SCREEN_RAM[index] = character;
}

//-------------------------------------------------------------------------
char incScreenRam(int index)
{
	char character = SCREEN_RAM[index];
	SCREEN_RAM[index] = ++character;
	return character;
}

//-------------------------------------------------------------------------
void drawElements()
{
	char y, index, airLength, airLen, airMask, *charData;
	int yOffset;
	
	++m_frameCounter;

	index = (m_airAmount/AirScaler);
	airLength = index / CharX;
	airLen = index % CharX;
	airMask = 0xff << (CharX - airLen);
	index = (CharY * WorldAir2);
	if(airLength != m_airHead)
	{
		SCREEN_RAM[screenRowStart[AirRow]+AirStrLen+m_airHead] = WorldEmpty;
		COLOUR_RAM[screenRowStart[AirRow]+AirStrLen+m_airHead] = levelColour[m_level][COLOUR_Background];
		--m_airHead;
	}
	CHARMAP_RAM[index+2] = airMask;
	CHARMAP_RAM[index+3] = airMask;
	CHARMAP_RAM[index+4] = airMask;
	CHARMAP_RAM[index+5] = airMask;
	SCREEN_RAM[screenRowStart[AirRow]+AirStrLen+m_airHead] = WorldAir2;

	m_conveyorFrame = (m_conveyorFrame - conveyorDirections[m_level]) & 3;

	charData = &CHARMAP_RAM[CharY * (WorldConveyor1 + m_conveyorFrame)];
	index = CharY * WorldConveyor;

	for(y = 0; y < CharY; ++y)
	{
		CHARMAP_RAM[index] = charData[y];
		++index;
	}

	if(m_frameCounter&1)
	{
		index = fontSizeUpper + UpperFontStart + (4*(++m_livesTimer % 4));
		for(y = 0; y < m_lives; ++y)
		{
			yOffset = screenRowStart[LivesRow] + (2 * y);
			SCREEN_RAM[yOffset] = index;
			++yOffset;
			SCREEN_RAM[yOffset] = index+1;
			yOffset += ScreenX;
			SCREEN_RAM[yOffset] = index+3;
			--yOffset;
			SCREEN_RAM[yOffset] = index+2;
		}
	}
}

//-------------------------------------------------------------------------
void undrawLife()
{
	int yOffset = screenRowStart[LivesRow] + (2 * m_lives);
	SCREEN_RAM[yOffset] = BlankChar;
	++yOffset;
	SCREEN_RAM[yOffset] = BlankChar;
	yOffset += ScreenX;
	SCREEN_RAM[yOffset] = BlankChar;
	--yOffset;
	SCREEN_RAM[yOffset] = BlankChar;
}

//-------------------------------------------------------------------------
void drawScores()
{
	char x, cell, *charData, scoreText[LevelX+1];
	char scoreDiv;

	if(m_score > m_highScore)
		m_highScore = m_score;
		
	scoreDiv = (m_score / ExtraLife);
	if(scoreDiv != m_oldScore)
	{
		m_lives++;
		m_oldScore = scoreDiv;
	}
	
	sprintf(scoreText, szScoreText, m_highScore, m_score);
	charData = &SCREEN_RAM[screenRowStart[ScoreRow]];
	x = 0;
	while((cell = scoreText[x]))
	{
		*charData = cell;
		++charData;
		++x;
	}
}

//-------------------------------------------------------------------------
void drawSprites()
{
	char i, index, spriteBit, msbMask, willyFrame, *pSpriteDataPtrs;
	int  iPos;
	char *spritePosLoc = &VIC.spr1_x;
	char willyCollision = ColNoCol;
	char spriteIndex = level2Sprite[m_level].index;
	char spriteCount = level2Sprite[m_level].count + spriteIndex;

	while(VIC.rasterline < (ScreenY*CharY));
	
	index = 0;
	msbMask = 0;
	spriteBit = 2;
	pSpriteDataPtrs = SPRITEDATAPTRS+1;
	
	for(i = spriteIndex; i < spriteCount; ++i)
	{
		SpriteData *sprt = &spriteData[i];
		if(sprt->spriteClass & SPClass_Door)
		{
			if(sprt->curEnabled  && !sprt->curLocalStorage)
			{
				char cellCol, sprtCol;
				int iPos = screenRowStart[doorValues[m_level].y];
				iPos += doorValues[m_level].x;

				m_doorToggle = 1 - m_doorToggle;
				if(m_doorToggle)
				{
					cellCol = levelColour[m_level][COLOUR_Door];
					sprtCol = sprt->colour;
				}
				else
				{
					cellCol = sprt->colour;
					sprtCol = levelColour[m_level][COLOUR_Door];
				}
				
				COLOUR_RAM[iPos] = cellCol;
				++iPos;
				COLOUR_RAM[iPos] = cellCol;
				iPos += ScreenX;
				COLOUR_RAM[iPos] = cellCol;
				iPos -= 1;
				COLOUR_RAM[iPos] = cellCol;
				
				(&(VIC.spr1_color))[index] = sprtCol;
			}
		}
		else if(sprt->spriteClass & SPClass_Eugene && sprt->curLocalStorage)
		{
			++((&(VIC.spr0_color))[sprt->curEnabled]);
		}
		
		if(sprt->spriteClass & SPClass_Vertical)
		{
			if(sprt->otherAxis >= 256 - SpritePreX)
				msbMask |= spriteBit;
			*spritePosLoc++ = (sprt->otherAxis + SpritePreX);
			*spritePosLoc++ = (sprt->curPos + SpritePreY);
		}
		else
		{
			iPos = sprt->curPos + SpritePreX;
			if(iPos > 255)
			{
				msbMask |= spriteBit;
				iPos -= 256;
			}
			
			*spritePosLoc++ = iPos & 0xf8;
			*spritePosLoc++ = (sprt->otherAxis + SpritePreY);
		}
		*pSpriteDataPtrs++ = m_spriteIndicies[index] + sprt->curFrame;
		++index;
		spriteBit <<= 1;
	}
	
	if(m_gameState != GSTATE_PreDemo && m_gameState != GSTATE_Demo)
	{
		iPos = m_willy.position.x + SpritePreX;
		if(iPos > 255)
		{
			msbMask |= 1;
			iPos -= 256;
		}
		
		VIC.spr0_x = iPos & 0xf8;
		VIC.spr0_y = m_willy.position.y + SpritePreY;

		willyFrame = (m_willy.position.x % NumWillyFrames) / 2;
		if(-1 == m_willy.direction)
			willyFrame |= (NumWillyFrames/2);

		*SPRITEDATAPTRS = ((int)(SPRITE_RAM - VIC_BASE_RAM) / SpriteBytes) + willyFrame;
		
		willyFrame = 0;
	}
	VIC.spr_hi_x = msbMask;
}

//-------------------------------------------------------------------------
char checkSpriteCollisions()
{	
	char i, x, y, index;
	char spriteIndex = level2Sprite[m_level].index;
	char spriteCount = level2Sprite[m_level].count + spriteIndex;
	
	if( m_gameState != GSTATE_Demo)
	{
		char collision = VIC.spr_coll;
		if(collision & 1)
		{
			index = m_doorBit;
			for(i = spriteCount; i > spriteIndex; --i, index >>= 1)
			{
				if(collision & index)
				{
					SpriteData *sprt = &spriteData[i-1];
					
					if(sprt->spriteClass & SPClass_Vertical)
					{
						x = sprt->otherAxis;
						y = sprt->curPos;
					}
					else
					{
						x = sprt->curPos;
						y = sprt->otherAxis;
					}
				
					if(m_willy.position.x 			< (2*CharX + x) &&
					   m_willy.position.x + CharX 	> x 			&&
					   m_willy.position.y 			< (2*CharY + y) &&
					   m_willy.position.y + 2*CharY > y)
					{
						if(sprt->spriteClass & SPClass_Door)
						{
							if(sprt->curEnabled)
								return ColDoor;
								
							continue;
						}
						return ColDie;
					}
				}
			}
		}
	}

	return ColNoCol;
}

//-------------------------------------------------------------------------
void renderFrame()
{
	drawElements();
	drawSprites();
}

//-------------------------------------------------------------------------
void renderFrontEnd(char feState)
{
	char i;
	
	switch(feState)
	{
		case FERState_PreRender:
			VIC.spr_ena = 0;
			VIC.bordercolor = COLOR_RED;
			VIC.bgcolor0 = COLOR_BLACK;

			memset(SCREEN_RAM+(ScreenX*LevelY), BlankChar, ScreenX*(ScreenY-LevelY));
			memset(COLOUR_RAM, COLOR_BLACK, ScreenY*ScreenX);
			memset(&COLOUR_RAM[screenRowStart[ScoreRow]], COLOR_YELLOW, LevelX);
			bzero(SCREEN_RAM, ScreenX * LevelY);
			memset(&COLOUR_RAM[screenRowStart[ThisVersionRow]], COLOR_YELLOW, ScreenX);
			for(i = 0; szThisVersionText[i]; ++i)
				SCREEN_RAM[screenRowStart[ThisVersionRow]+i] = szThisVersionText[i];
		break;
		
		case FERState_PreTextScroll:
			bzero(COLOUR_RAM+(ScreenX*8), ScreenX);
			memcpy(COLOUR_RAM+ScreenX, manicText, ScreenX * 7);
			memcpy(COLOUR_RAM+(9*ScreenX), minerText, ScreenX * 7);
			memset(COLOUR_RAM+screenRowStart[LevelY], COLOR_YELLOW, LevelX);
		break;
		
		case FERState_TitleFlash:
			if(0 == (m_timer % (FETime/16)))
				memcpy(COLOUR_RAM+(4*ScreenX), manicText, ScreenX * 7);
			else if((FETime/32) == (m_timer % (FETime/16)))
				memcpy(COLOUR_RAM+(4*ScreenX), minerText, ScreenX * 7);
		break;
		
		case FERState_FlashColour:
		{
			for(i = 0; i < 131; ++i)
			{
				while(VIC.rasterline < i);
				VIC.bordercolor = i;
			}
		}
		break;
		
		case FERState_Cleanup:
			memset(SCREEN_RAM, BlankChar, ScreenX * LevelY);
			memset(&SCREEN_RAM[screenRowStart[ThisVersionRow]], BlankChar, ScreenX);
		break;
	}

	if(m_scrollText)
	{
		for(i = 0; i < 32 && m_scrollText[i]; ++i)
			SCREEN_RAM[screenRowStart[TextRow]+i] = m_scrollText[i];
	}
}

//-------------------------------------------------------------------------
void setupLostScreen()
{

	VIC.spr_ena = 0;

	VIC.bordercolor = COLOR_BLACK;
	VIC.bgcolor0 = COLOR_BLACK;
	bzero(SCREEN_RAM, ScreenX*ScreenY);
	bzero(COLOUR_RAM, ScreenX*ScreenY);
	
	renderPrepSprites();
}

//-------------------------------------------------------------------------
char renderLostScreen()
{
	char i;
	char sprite = level2Sprite[m_level].index + level2Sprite[m_level].count - 1;
	SpriteData *sprt = &spriteData[sprite];
	
	if(sprt->curDirection)
	{
		sprt->curPos += sprt->speed;
		if(sprt->curPos >= sprt->max - 8)
			VIC.spr_ena &= 0xfe;
		
		if(sprt->curPos >= sprt->max)
		{
			sprt->curDirection = 0;
			i = 0;
			while(szGameOverText[i])
			{
				SCREEN_RAM[screenRowStart[GameOverRow]+10+i] = szGameOverText[i];
				++i;
			}
		}
	}
	else
	{
		i = 0;
		while(szGameOverText[i])
		{
			COLOUR_RAM[screenRowStart[GameOverRow]+10+i] = ++sprt->curLocalStorage;
			++i;
		}
		if(!sprt->curLocalStorage)
			return YES;
	}
	drawSprites();
	
	return NO;
}

//-------------------------------------------------------------------------
void readKeys()
{
	char keys;
	
	m_userKeyBits = 0;
	CIA1.pra = 0;
	keys = 255 - CIA1.prb;

	if(keys & 0x90)
	{
		m_userKeyBits |= KEY_Jump;
		keys &= 0x6f;
	}
	
	if(keys & 0x55)
		m_userKeyBits |= KEY_Left;

	if(keys & 0xaa)
		m_userKeyBits |= KEY_Right;
}
