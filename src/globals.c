/*
 *  globals.c
 *  ManicMiner
 *
 *  Created by Stefan Wessels, Dec. 2010.
 *
 */

#include "data.h"
#include "globals.h"

int				m_score;
int				m_highScore;
char			m_timer;
char			m_oldScore;
char			m_gameState;

char			m_level;
char			m_keysFound;
char			m_keysToFind;
char			m_lives;
char			m_livesTimer;

Willy  			m_willy;
char			m_willyJumpState;
signed char		m_willyJumpHeight;

signed char		m_forceDirection;
char			m_userKeyBits;
signed char		m_hMotion;

signed char 	m_trackingX[MaxSprites];
signed char 	m_trackingY[MaxSprites];
char 			m_trackedSprites;
char			m_solarBeamX[36];
char			m_solarBeamY[36];
char			m_solarBeamLength;

const char		*m_scrollText;
int				m_airAmount;
char			m_airHead;
char			m_conveyorFrame;
char			m_frameCounter;
char			m_spriteIndicies[MaxSprites];
char			m_doorBit;
char			m_doorToggle;
