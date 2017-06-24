/*
 *  data.h
 *  ManicMiner
 *
 *  Created by Stefan Wessels, Dec. 2010.
 *
 */

#ifndef _DATA_H_
#define _DATA_H_

/*-----------------------------------------------------------------------*/
#define VIC_BASE_RAM			(0x8000)
#define	SCREEN_RAM				((char*)VIC_BASE_RAM+0x2800)
#define SPRITEDATAPTRS			((char*)SCREEN_RAM+0x03f8)
#define CHARMAP_RAM				((char*)VIC_BASE_RAM+0x2000)
#define	SPRITE_RAM				((char*)VIC_BASE_RAM+0x3000)
#define LVL_SPRITE_RAM			((char*)VIC_BASE_RAM+0x3200)
#define COLOUR_RAM				((char*)0xd800)
#ifdef __C64__
#define MEM_KRNL_PRNT			((char*)0x288)
#endif
#ifdef __C128__
#define MEM_KRNL_PRNT			((char*)0xa3b)
#endif

//-------------------------------------------------------------------------
#define CharY					8
#define CharX					8
#define ScreenY					25
#define ScreenX					40
#define LevelX					32
#define LevelY					16
#define NumLevels				20
#define	SpritePreX				56
#define SpritePreY				50
#define SpritePixelsX			16
#define SpritePixelsY			16
#define	SpriteBytes				64
#define NumWillyFrames			8
#define MaxWillyJumpHeight		8
#define MaxWillyFallHeight		-9
#define WillyJumpSpeed			4
#define MaxKeysPerLevel			5
#define LevelTiles				8
#define DoorCycleCount			4
#define	AirScaler				8
#define DemoTime				72
#define FETime					200
#define ScrollTimer				1
#define	GameTimerHiMin			0xfd
#define GameOverRow				12
#define TextRow					16
#define	AirRow					17
#define	ScoreRow				19
#define	LivesRow				21
#define	ThisVersionRow			ScreenY-1
#define LevelSpriteFrames		22
#define	ExtraLife				10000
#define MaxSprites				8
#define	UpperFontStart			193
#define	NO						0
#define	YES						1

//-------------------------------------------------------------------------
#define	WorldEmpty				0
#define WorldFloor				1
#define WorldSpecial			2
#define WorldWall				3
#define WorldCollapse			4
#define WorldBush				5
#define WorldRock				6
#define WorldConveyor			7
#define	WorldKey1				10
#define	WorldKey2				11
#define	WorldKey3				12
#define	WorldKey4				13
#define	WorldKey5				14
#define WorldCollapse1			15
#define WorldCollapse2			16
#define WorldCollapse3			17
#define WorldCollapse4			18
#define WorldCollapse5			19
#define WorldCollapse6			20
#define WorldCollapse7			21
#define WorldCollapse8			22
#define WorldConveyor1			23
#define WorldConveyor2			24
#define WorldConveyor3			25
#define WorldConveyor4			26
#define WorldSwitchLeft1		27
#define WorldSwitchLeft2		28
#define WorldSwitchRight		29
#define WorldAir2				30
#define WorldAir1				31
#define	BlankChar				32

//-------------------------------------------------------------------------
#define COLOUR_BLACK			0x00
#define COLOUR_WHITE			0x01
#define COLOUR_RED				0x02
#define COLOUR_CYAN				0x03
#define COLOUR_VIOLET			0x04
#define COLOUR_PURPLE			COLOUR_VIOLET
#define COLOUR_GREEN			0x05
#define COLOUR_BLUE				0x06
#define COLOUR_YELLOW			0x07
#define COLOUR_ORANGE			0x08
#define COLOUR_BROWN			0x09
#define COLOUR_LIGHTRED			0x0A
#define COLOUR_GRAY1			0x0B
#define COLOUR_GRAY2			0x0C
#define COLOUR_LIGHTGREEN		0x0D
#define COLOUR_LIGHTBLUE		0x0E
#define COLOUR_GRAY3			0x0F

//-------------------------------------------------------------------------
typedef 	char 	BOOL;

//-------------------------------------------------------------------------
typedef struct _iPoint
{
	char	x, y;
} iPoint;

//-------------------------------------------------------------------------
typedef struct _willy
{
	iPoint position;
	signed char direction;
} Willy;

//-------------------------------------------------------------------------
enum 
{
	JumpStateOff, 
	JumpStateUp, 
	JumpStateDown
};

//-------------------------------------------------------------------------
enum
{
	KEY_Left	= (1<<0), 
	KEY_Right	= (1<<1), 
	KEY_Jump	= (1<<2), 
};

/*-----------------------------------------------------------------------*/
enum
{
	LEVEL_Central_Cavern,
	LEVEL_The_Cold_Room,
	LEVEL_The_Menagerie,
	LEVEL_Abandoned_Uranium_Workings,
	LEVEL_Eugenes_Lair,
	LEVEL_Processing_Plant,
	LEVEL_The_Vat,
	LEVEL_Miner_Willy_meets_the_Kong,
	LEVEL_Wacky_Amoebatrons,
	LEVEL_The_Endorian_Forest,
	LEVEL_Attack_of_the_Mutant_Telephones,
	LEVEL_Return_of_the_Alien_Kong_Beast,
	LEVEL_Ore_Refinery,
	LEVEL_Skylab_Landing_Bay,
	LEVEL_The_Bank,
	LEVEL_The_Sixteenth_Cavern,
	LEVEL_The_Warehouse,
	LEVEL_Amoebatrons_Revenge,
	LEVEL_Solar_Power_Generator,
	LEVEL_The_Final_Barrier,
};

/*-----------------------------------------------------------------------*/
enum 
{ 
	ColNoCol			= 0, 
	ColSpecial			= 1<<0,
	ColWall				= 1<<1, 
	ColDie				= 1<<2, 
	ColFloor			= 1<<3, 
	ColConveyor			= 1<<4, 
	ColCollapse			= 1<<5, 
	ColSwitch			= 1<<6,
	ColDoor				= 1<<7,
};

/*-----------------------------------------------------------------------*/
enum
{
	SPClass_Horizontal	= (0<<0),
	SPClass_Vertical	= (1<<0),
	SPClass_Full_Range	= (1<<1),
	SPClass_Eugene		= (1<<2),
	SPClass_Door		= (1<<3),
	SPClass_ExtendMaxX	= (1<<4),
	SPClass_Kong		= (1<<5),
	SPClass_SkyLab		= (1<<6),
	SPClass_HoldAtEnd	= (1<<7),
};

/*-----------------------------------------------------------------------*/
enum
{
	COLOUR_Background,
	COLOUR_Ground,
	COLOUR_Special,
	COLOUR_Wall,
	COLOUR_Collapse,
	COLOUR_Bush,
	COLOUR_Rock,
	COLOUR_Conveyor,
	COLOUR_Boarder,
	COLOUR_Door,
};

//-------------------------------------------------------------------------
enum 
{
	GSTATE_PreFrontEnd, 
	GSTATE_FrontEnd, 
	GSTATE_TextScroll, 
	GSTATE_PreDemo, 
	GSTATE_Demo, 
	GSTATE_PrePreIngame, 
	GSTATE_PreIngame, 
	GSTATE_Ingame, 
	GSTATE_PreBeatLevel,
	GSTATE_BeatLevel, 
	GSTATE_PreDied,
	GSTATE_PreLost,
	GSTATE_Lost, 
	GSTATE_Won, 
};

//-------------------------------------------------------------------------
enum
{
	FERState_PreRender,
	FERState_PreTextScroll,
	FERState_TitleFlash,
	FERState_FlashColour,
	FERState_Cleanup,
	FERState_NOP,
};

/*-----------------------------------------------------------------------*/
typedef struct
{
	char			curPos;
	char			curFrame;
	signed char		curDirection;
	char			curEnabled;
	char			curLocalStorage;
	char			min;
	char			max;
	char			start;
	char			otherAxis;
	char			speed;
	char			frameIndex;
	char			colour;
	signed char		startDirection;
	char			startEnabled;
	char			spriteClass;
} SpriteData;

/*-----------------------------------------------------------------------*/
typedef struct 
{
	char index;
	char count;
} Level2Sprite;

/*-----------------------------------------------------------------------*/
extern char array[ScreenX*ScreenY];
extern const int screenRowStart[ScreenY];
extern const char *levelBuffer[NumLevels];
extern const char manicText[][ScreenX];
extern const char minerText[][ScreenX];
extern const unsigned char backCharMap[][CharY];
extern const unsigned char fontCharMapUpper[][CharY];
extern const unsigned char fontSizeUpper;
extern const unsigned char fontCharMapLower[][CharY];
extern const unsigned char fontSizeLower;
extern const char levelCharIndex[][LevelTiles];
extern const char levelColour[][10];
extern const char spriteImages[][32];
extern const char *levelNames[NumLevels];
extern const char *szIntroText;
extern const char *szThisVersionText;
extern const char *szScoreText;
extern const char *szGameOverText;
extern const char *szAir;
extern const char AirStrLen;
extern const iPoint willyStartPositions[NumLevels+1];
extern const signed char willyStartDirections[NumLevels+1];
extern const signed char conveyorDirections[NumLevels];
extern const iPoint keyPositions[NumLevels][MaxKeysPerLevel];
extern const iPoint switchPositions[2];
extern const char skylabData[3][4];
extern const iPoint doorValues[NumLevels];
extern SpriteData spriteData[];
extern Level2Sprite level2Sprite[NumLevels+1];

#endif