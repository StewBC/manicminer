/*
 *  globals.h
 *  ManicMiner
 *
 *  Created by Stefan Wessels, Dec. 2010.
 *
 */

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

//-------------------------------------------------------------------------
extern int				m_score;
extern int				m_highScore;
extern char				m_timer;
extern char				m_oldScore;
extern char				m_gameState;

extern char				m_level;
extern char				m_keysFound;
extern char				m_keysToFind;
extern char				m_lives;
extern char				m_livesTimer;

extern Willy  			m_willy;
extern char				m_willyJumpState;
extern signed char		m_willyJumpHeight;

extern signed char		m_forceDirection;
extern char				m_userKeyBits;
extern signed char		m_hMotion;

extern signed char		m_trackingX[MaxSprites];
extern signed char		m_trackingY[MaxSprites];
extern char 			m_trackedSprites;
extern char				m_solarBeamX[36];
extern char				m_solarBeamY[36];
extern char				m_solarBeamLength;

extern const char		*m_scrollText;
extern int				m_airAmount;
extern char				m_airHead;
extern char				m_conveyorFrame;
extern char				m_frameCounter;
extern char				m_spriteIndicies[MaxSprites];
extern char				m_doorBit;
extern char				m_doorToggle;

#endif
