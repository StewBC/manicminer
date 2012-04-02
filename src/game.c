/*
 *  game.c
 *  ManicMiner
 *
 *  Created by Stefan Wessels, Dec. 2010.
 *
 */

#include "data.h"
#include "globals.h"
#include "render.h"
#include "game.h"

//-------------------------------------------------------------------------
int main()
{
	setup();
	while(1)
		mainLoop();
	return 0;
}

//-------------------------------------------------------------------------
void mainLoop()
{
	beginFrame();
	readKeys();

	switch(m_gameState)
	{
		case GSTATE_PreFrontEnd:
			m_scrollText = 0;
			m_userKeyBits = 0;
			m_timer = 0;
			m_lives = 3;
			m_willy.position.x = 29*8;
			m_willy.position.y = 64+8;
			m_willy.direction = 1;
			m_score = 0;
			m_oldScore = 0;
			m_gameState = GSTATE_FrontEnd;
			renderFrontEnd(FERState_PreRender);
			drawScores();
			
		case GSTATE_FrontEnd:
		{
			char feState;

			if(!m_userKeyBits)
			{
				if(++m_timer > FETime)
				{
					m_timer = 0;
					m_gameState = GSTATE_TextScroll;
					m_scrollText = (char*)szIntroText;
					feState = FERState_PreTextScroll;
				}
				else
				{
					feState = FERState_TitleFlash;
				}
			}
			else
			{
				m_gameState = GSTATE_PrePreIngame;
				feState = FERState_Cleanup;
			}

			renderFrontEnd(feState);
			
			break;
		}
			
		case GSTATE_TextScroll:
		{
			char feState = FERState_NOP;
			
			if(!m_userKeyBits)
			{
				if(++m_timer > ScrollTimer)
				{
					if(!*(++m_scrollText))
					{
						m_gameState = GSTATE_PreDemo;
						feState = FERState_Cleanup;
					}
					m_timer = 0;
				}
			}
			else 
			{
				m_gameState = GSTATE_PrePreIngame;
				feState = FERState_Cleanup;
			}

			renderFrontEnd(feState);
			
			break;
		}
			
		case GSTATE_PreDemo:
			m_timer = 0;
			m_level = LEVEL_Central_Cavern;
			prepLevel();
			m_gameState = GSTATE_Demo;
			
		case GSTATE_Demo:
		{
			if(m_userKeyBits)
				m_gameState = GSTATE_PreFrontEnd;
			else
			{
				runGame();
				if(++m_timer > DemoTime)
				{
					m_timer = 0;
					if(LEVEL_The_Final_Barrier < ++m_level)
						m_gameState = GSTATE_PreFrontEnd;
					else
						prepLevel();
				}	
			}
			break;
		}
			
		case GSTATE_PrePreIngame:
			m_level = LEVEL_Central_Cavern;
			m_gameState = GSTATE_PreIngame;
			
		case GSTATE_PreIngame:
			m_userKeyBits = 0;
			m_hMotion = 0;
			prepLevel();
			m_gameState = GSTATE_Ingame;
			
		case GSTATE_Ingame:
		{
			char state = runGame();
			if(ColDie == state)
				m_gameState = GSTATE_PreDied;
			else if(ColDoor == state)
				m_gameState = GSTATE_PreBeatLevel;
			break;
		}
			
		case GSTATE_PreBeatLevel:
			m_gameState = GSTATE_BeatLevel;
	
		case GSTATE_BeatLevel:
			m_score += 24;
			drawScores();
			m_airAmount -= 2 * AirScaler; 
			if(m_airAmount <= 0)
			{
				m_airAmount = 0;
				if(LEVEL_The_Final_Barrier != m_level)
				{
					++m_level;
					m_gameState = GSTATE_PreIngame;
				}
				else
				{
					m_gameState = GSTATE_Won;
				}
			}
			renderFrame();
			break;
			
		case GSTATE_PreDied:
			--m_lives;
			undrawLife();
			renderFrontEnd(FERState_FlashColour);
			if(!m_lives)
				m_gameState = GSTATE_PreLost;
			else
				m_gameState = GSTATE_PreIngame;
			break;
			
		case GSTATE_PreLost:
			m_level = LEVEL_The_Final_Barrier+1;
			setupLostScreen();
			prepLevel();
			m_gameState = GSTATE_Lost;
			
		case GSTATE_Lost:
			if(renderLostScreen())		
				m_gameState = GSTATE_PreFrontEnd;
			break;
			
		case GSTATE_Won:
			m_gameState = GSTATE_PreFrontEnd;
			break;
	}
	syncEndFrame();
}

//-------------------------------------------------------------------------
void setup() 
{
	m_highScore = 0;
	m_airAmount = 0;
	m_trackedSprites = 0;
	m_solarBeamLength = 0;
	m_gameState = GSTATE_PreFrontEnd;
	renderSetup();
};

//-------------------------------------------------------------------------
void prepLevel()
{
	char i;
	char spriteIndex = level2Sprite[m_level].index;
	char spriteCount = level2Sprite[m_level].count + spriteIndex;
	
	m_willy.position = willyStartPositions[m_level];
	m_willy.direction = willyStartDirections[m_level];
	m_willyJumpState = JumpStateOff;
	m_willyJumpHeight = 0;
	m_forceDirection = 0;
	m_airAmount = (((LevelX * CharX) - (AirStrLen * CharX)) * AirScaler)-1;
	
	for(i = spriteIndex; i < spriteCount; ++i)
	{
		SpriteData *sprt = &spriteData[i];
		sprt->curPos = sprt->start;
		sprt->curDirection = sprt->startDirection;
		sprt->curEnabled = sprt->startEnabled;
		if(SPClass_ExtendMaxX & sprt->spriteClass && sprt->curLocalStorage)
			sprt->max = sprt->curLocalStorage;
		if(SPClass_Kong & sprt->spriteClass)
		{
			sprt->curDirection = 0;
			sprt->max = 0;
		}
		if(SPClass_SkyLab & sprt->spriteClass)
		{
			sprt->curDirection = 1;
			sprt->min = 0;
			sprt->otherAxis = skylabData[sprt->startEnabled-1][sprt->curEnabled-1];
		}
		sprt->curLocalStorage = 0;
	}

	m_keysFound = 0;
	m_keysToFind = 0;

	if(m_level < NumLevels)
		renderPrepLevel();
}

//-------------------------------------------------------------------------
char runGame()
{
	int offset;
	char i, willyCollideSprites = ColNoCol;
	BOOL willyCollideBackground = NO;
	if(m_keysFound == m_keysToFind)
	{
		++m_keysFound;
		for(i = level2Sprite[m_level].index ;; ++i)
		{
			SpriteData *sprt = &spriteData[i];
			if(sprt->spriteClass & SPClass_Door)
			{
				sprt->curEnabled = 1;
				m_doorToggle = 0;
				break;
			}
			else if(sprt->spriteClass & SPClass_Eugene)
			{
				sprt->curDirection = 1;
				sprt->curLocalStorage = 1;
			}
		}
	}
	else
	{
		for(i = 0; i < MaxKeysPerLevel; ++i)
		{
			offset = screenRowStart[keyPositions[m_level][i].y]+keyPositions[m_level][i].x ;
			if(WorldEmpty != SCREEN_RAM[offset])
				incColourRamBy(offset, (i+1));
		}
	}
	
	if(!--m_airAmount)
		return ColDie;
	
	moveSprites();
	
	if(LEVEL_Solar_Power_Generator == m_level)
	 	buildPowerBeamPath();

	if(GSTATE_Demo != m_gameState)
		willyCollideBackground = moveWilly();
	
	renderFrame();
	
	if(willyCollideBackground || (willyCollideSprites = checkSpriteCollisions()))
	{
		if(willyCollideSprites & ColDoor)
			return ColDoor;
			
		return ColDie;
	}
	
	return ColNoCol;
}

//-------------------------------------------------------------------------
char buildTracking(char bx)
{
	char i, x, y;
	char dy = 0xff;
	char spriteIndex = level2Sprite[m_level].index;
	char spriteCount = level2Sprite[m_level].count + spriteIndex - 1;

	m_trackedSprites = spriteCount - spriteIndex;

	for(i = 0; i < m_trackedSprites; ++i)
	{
		SpriteData *sprt = &spriteData[i + spriteIndex];
		
		if(SPClass_Vertical & sprt->spriteClass)
		{
			x = sprt->otherAxis;
			y = sprt->curPos;
		}
		else 
		{
			x = sprt->curPos;
			y = sprt->otherAxis;
		}
		
		x /= CharX;
		m_trackingX[i] = x;
		
		y /= CharY;
		m_trackingY[i] = y;
		
		if(x <= bx && bx < 2 + x && y < dy)
			dy = y;
	}
	return dy;
}

//-------------------------------------------------------------------------
void buildPowerBeamPath()
{
	char 	i, sbx, sby, x, y, delta;
	char	direction = 1, bx = 23, by = 0;
	char	levCol = levelColour[m_level][0];
	char	oSolarBeamLength = m_solarBeamLength;
	int		iColor = screenRowStart[by]+bx;

	delta = buildTracking(bx);
	m_solarBeamLength = 0;
	
	while(1)
	{
		if(!SCREEN_RAM[screenRowStart[by]+bx])
		{
			if(!delta)
			{
				delta = 0xff;
				direction = 1 - direction;

				if(!direction)
				{
					for(i = 0; i < m_trackedSprites; ++i)
					{
						x = m_trackingX[i];
						if(x >= bx)
							continue;
						x = bx - x - 1;
						y = m_trackingY[i];
						if(y <= by && by <= 2 + y && x < delta)
							delta = x;
					}
				}
				else
				{
					for(i = 0; i < m_trackedSprites; ++i)
					{
						y = m_trackingY[i];
						if(y + 1 < by)
							continue;
						x = m_trackingX[i];
						if(x <= bx && bx < 2 + x && y < delta)
							delta = y;
					}
					delta -= by;
				}
				if(!delta)
					delta = 1;
			}
			--delta;

			sbx = m_solarBeamX[m_solarBeamLength];
			if(sbx != bx)
			{
				sby = m_solarBeamY[m_solarBeamLength];
				if(m_solarBeamLength < oSolarBeamLength)
					setColourRam(screenRowStart[sby]+sbx, levCol);
				m_solarBeamX[m_solarBeamLength] = bx;
				m_solarBeamY[m_solarBeamLength] = by;
			}
			setColourRam(iColor, COLOUR_YELLOW);
			++m_solarBeamLength;
			
			if(!direction)
			{
				--bx;
				--iColor;
			}
			else
			{
				++by;
				iColor += ScreenX;
			}
		}
		else
		{
			break;
		}
	}
	
	while(oSolarBeamLength > m_solarBeamLength)
	{
		--oSolarBeamLength;
		sbx = m_solarBeamX[oSolarBeamLength];
		sby = m_solarBeamY[oSolarBeamLength];
		setColourRam(screenRowStart[sby]+sbx, levCol);
	}
}

//-------------------------------------------------------------------------
char checkCollision(char bufferValue)
{
	switch(bufferValue)
	{
		case WorldEmpty:
		case WorldSwitchRight:
			return ColNoCol;
			
		case WorldSpecial:
			return ColSpecial;
			
		case WorldWall:
			return ColWall;
			
		case WorldCollapse1:
		case WorldCollapse2:
		case WorldCollapse3:
		case WorldCollapse4:
		case WorldCollapse5:
		case WorldCollapse6:
		case WorldCollapse7:
		case WorldCollapse8:
			return ColCollapse;
			
		case WorldBush:
		case WorldRock:
			return ColDie;
			
		case WorldConveyor:
			return ColConveyor;
			
		case WorldSwitchLeft2:
		case WorldSwitchLeft1:
		{
			int  spClass;
			char i, maxExtend, y1, x1, y2, x2;
			char switchPos = bufferValue - WorldSwitchLeft1;
			char spriteIndex = level2Sprite[m_level].index;
			char spriteCount = level2Sprite[m_level].count + spriteIndex;

			if(!switchPos)
			{
				y1 = 11;
				x1 = 17;
				y2 = 12;
				x2 = 17;
				maxExtend = 24;
				spClass = SPClass_ExtendMaxX;
			}
			else
			{
				y1 =  2;
				x1 = 15;
				y2 =  2;
				x2 = 16;
				maxExtend = 104;
				spClass = SPClass_Kong;
			}

			setScreenRam(screenRowStart[y1]+x1, WorldEmpty);
			setScreenRam(screenRowStart[y2]+x2, WorldEmpty);
			setScreenRam(screenRowStart[switchPositions[switchPos].y]+switchPositions[switchPos].x, WorldSwitchRight);
			setColourRam(screenRowStart[y1]+x1, levelColour[m_level][COLOUR_Background]);
			setColourRam(screenRowStart[y2]+x2, levelColour[m_level][COLOUR_Background]);

			for(i = spriteIndex; i < spriteCount; ++i)
			{
				SpriteData *sprt = &spriteData[i];
				if(spClass & sprt->spriteClass)
				{
					if(spClass & SPClass_ExtendMaxX)
						sprt->curLocalStorage = sprt->max;
					else
						sprt->curDirection = 1;
					sprt->max += maxExtend;
					break;
				}
			}
			return ColNoCol;
		}
			
		case WorldKey1:
		case WorldKey2:
		case WorldKey3:
		case WorldKey4:
		case WorldKey5:
		{
			int keyPos = bufferValue - WorldKey1;
			int offset = screenRowStart[keyPositions[m_level][keyPos].y]+keyPositions[m_level][keyPos].x;
			setScreenRam(offset, WorldEmpty);
			setColourRam(offset, levelColour[m_level][COLOUR_Background]);
			++m_keysFound;
			m_score += 100;
			drawScores();
			return ColNoCol;
		}
			
		default:
			return ColFloor;
	}
	return ColNoCol;
}

//-------------------------------------------------------------------------
void doFrameAndClassForSprite(SpriteData* sprt)
{
	char curFrame = (sprt->curPos % 8) / 2;
	
	if(-1 == sprt->curDirection && !(sprt->spriteClass & SPClass_Full_Range))
		curFrame |= 4;
	
	if(sprt->spriteClass & SPClass_Door && sprt->curEnabled)
	{
		++sprt->curLocalStorage;
		if(sprt->curLocalStorage > DoorCycleCount)
			sprt->curLocalStorage = 0;
	}
	else if(sprt->spriteClass & SPClass_Eugene)
	{
		curFrame = 0;
		if(sprt->curLocalStorage)
		{
			if(sprt->curDirection < 0)
				sprt->curDirection = 0;
		}
	}
	else if(sprt->spriteClass & SPClass_Kong)
	{
		curFrame = sprt->curFrame;
		++sprt->curLocalStorage;
		if(sprt->curLocalStorage > 5)
		{
			++curFrame;
			curFrame &= 1;
			if(sprt->max)
			{
				curFrame += 2;
				if(sprt->curPos == sprt->max)
				{
					andSpriteMask(~(1<<sprt->curEnabled));
					sprt->max = 0;
					sprt->curPos = 0;
				}
			}
			sprt->curLocalStorage = 0;
		}
	}
	else if(sprt->spriteClass & SPClass_SkyLab)
	{
		if(sprt->curDirection)
			curFrame = 0;
		else
		{
			curFrame = sprt->curFrame;
			++curFrame;
			if(7 < curFrame)
			{
				curFrame = 0;
				if(4 < ++sprt->curEnabled)
					sprt->curEnabled = 1;
				sprt->curDirection = 1;
				sprt->min = 0;
				sprt->curPos = 0;
				sprt->otherAxis = skylabData[sprt->startEnabled-1][sprt->curEnabled-1];
			}
		}

	}
	
	sprt->curFrame = curFrame;
}

//-------------------------------------------------------------------------
void moveSprites()
{
	char i, j;
	char spriteIndex = level2Sprite[m_level].index;
	char spriteCount = level2Sprite[m_level].count + spriteIndex;

	for(i = spriteIndex, j = 1; i < spriteCount; ++i, ++j)
	{
		SpriteData *sprt = &spriteData[i];
		
		sprt->curPos += sprt->speed * sprt->curDirection;
		if(sprt->curPos < sprt->min || sprt->curPos > sprt->max) 
		{
			if(sprt->curPos < sprt->min)
				sprt->curPos = sprt->min;
			else 
				sprt->curPos = sprt->max;
			
			if(sprt->spriteClass & SPClass_HoldAtEnd)
				sprt->curDirection = 0;
			else 
				sprt->curDirection = -sprt->curDirection;
		}
		
		doFrameAndClassForSprite(sprt);
	}
}

//-------------------------------------------------------------------------
BOOL moveWilly()
{
	int wx = m_willy.position.x / CharX;
	int wy = m_willy.position.y / CharY;
	int midCellY = m_willy.position.y % CharY;
	char colUp = ColNoCol, colDown = ColNoCol, sideCol = ColNoCol, leftCol = ColNoCol, rightCol = ColNoCol;
	BOOL willyDies = NO;
	signed char userKeyDirection = 0;
	
	if(m_userKeyBits & KEY_Left)
		userKeyDirection = -1;
	else if(m_userKeyBits & KEY_Right)
		userKeyDirection = 1;

	if(!(userKeyDirection && m_forceDirection && 2 != m_forceDirection && userKeyDirection != m_forceDirection))
		m_hMotion = m_forceDirection ? 2 == m_forceDirection ? 0 : m_forceDirection : userKeyDirection;
		
	if(m_hMotion && m_hMotion != m_willy.direction)
	{
		if(2 != m_hMotion)
			m_willy.direction = m_hMotion;
		m_hMotion = 0;	
	}

	if(KEY_Jump & m_userKeyBits && JumpStateOff == m_willyJumpState)
	{
		m_willyJumpState = JumpStateUp;
		m_forceDirection = m_hMotion ? m_hMotion : 2;
	}

	if(m_hMotion)
	{
		int midCellX = m_willy.position.x % CharX;
		int nx = 0;
		
		if(!midCellX && -1 == m_hMotion)
			nx = -1;
		else if((CharX-2) == midCellX && 1 == m_hMotion)
			nx = 2;

		if(nx)
		{
			sideCol = checkCollision(SCREEN_RAM[screenRowStart[wy]+wx+nx]);
			sideCol |= checkCollision(SCREEN_RAM[screenRowStart[wy+1]+wx+nx]);
			if(midCellY)
				sideCol |= checkCollision(SCREEN_RAM[screenRowStart[wy+2]+wx+nx]);
		}
		
		if(ColDie & sideCol)
			willyDies = YES;
		else if(!(ColWall & sideCol))
		{
			m_willy.position.x += (m_hMotion << 1);
			wx = m_willy.position.x / CharX;
		}
	}
	
	if(JumpStateUp != m_willyJumpState)
	{
		if(!midCellY)
		{
			leftCol = checkCollision(SCREEN_RAM[screenRowStart[wy+2]+wx]);
			rightCol = checkCollision(SCREEN_RAM[screenRowStart[wy+2]+wx+1]);
			colDown = leftCol | rightCol;
			if(colDown & ColSpecial)
				colDown |= ColWall;
		}
		
		if(ColNoCol != colDown)
		{
			if(JumpStateOff != m_willyJumpState)
			{
				m_willyJumpState = JumpStateOff;
				if(MaxWillyFallHeight > m_willyJumpHeight)
					willyDies = YES;
				m_willyJumpHeight = 0;
				m_forceDirection = 0;
			}
			
			if(ColDie & colDown)
				willyDies = YES;
			else if(ColConveyor & colDown)
				m_forceDirection = conveyorDirections[m_level];
			
			if(ColCollapse == leftCol)
			{
				int left = screenRowStart[wy+2]+wx;
				if(incScreenRam(left) > WorldCollapse8)
				{
					setScreenRam(left, WorldEmpty);
					setColourRam(screenRowStart[wy+2]+wx, levelColour[m_level][COLOUR_Background]);
				}
			}
			
			if(ColCollapse == rightCol)
			{
				int right = screenRowStart[wy+2]+wx+1;
				if(incScreenRam(right) > WorldCollapse8)
				{
					setScreenRam(right, WorldEmpty);
					setColourRam(screenRowStart[wy+2]+wx+1, levelColour[m_level][COLOUR_Background]);
				}
			}
		}
		else
		{
			if(JumpStateOff == m_willyJumpState)
			{
				m_forceDirection = 2;
				m_willyJumpState = JumpStateDown;
			}
			else
			{
				int fallHeight = m_willyJumpHeight < 0 ? 0 : m_willyJumpHeight >> 1;
				m_willy.position.y += (WillyJumpSpeed - fallHeight);
				--m_willyJumpHeight;
				if(-1 > m_willyJumpHeight)
					m_forceDirection = 2;
			}
		}
	}
	else 
	{
		if(!midCellY)
		{
			colUp = checkCollision(SCREEN_RAM[screenRowStart[wy-1]+wx]);
			colUp |= checkCollision(SCREEN_RAM[screenRowStart[wy-1]+wx+1]);
		}
		
		if(ColDie & colUp)
			willyDies = YES;
		else if(ColWall & colUp)
		{
			m_forceDirection = 2;
			m_willyJumpState = JumpStateDown;
			--m_willyJumpHeight;
		}
		else
		{
			m_willy.position.y -= (WillyJumpSpeed - (m_willyJumpHeight >> 1));
			++m_willyJumpHeight;
			if(MaxWillyJumpHeight < m_willyJumpHeight)
			{
				m_willyJumpState = JumpStateDown;
				--m_willyJumpHeight;
			}
		}
	}
	
	return willyDies;
}
