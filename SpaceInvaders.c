#include "mbed.h"
#include "stm32f413h_discovery_lcd.h"
#include "string.h"

#define SCREEN_WIDTH 240
#define LEFT -1
#define RIGHT 1
#define NUM_ALEIN_ROWS 6
#define NUM_ALEIN_COLUMNS 6
#define ALIEN_X_SIZE 15
#define ALIEN_Y_SIZE 15
#define PLAYER_X_SIZE 15
#define PLAYER_Y_SIZE 10
#define ROW_SIZE 10
#define COLUMN_SIZE 5
#define MAX_ROW 6
#define MAX_COLUMN 18
#define BUILDING_ROW 4
/* playerBulletY = screen size - height of land - height of player */
/* Always the same starting height */
#define PLAYER_BULLET_STARTING_Y_POS 205
#define BULLET_LENGTH 10
#define ALIEN_SCORE_AMOUNT 50
#define COLOR_ALIEN 0xEF80
#define COLOR_PLAYER 0x0560
#define COLOR_LAND 0xE6E3
#define COLOR_BULLET 0xE71C
#define COLOR_BUILDING 0xFA04
#define COLOR_PAUSE 0x8000

// For easier sprite declaration
#define O 1
#define _ 0

enum Entity {PLAYER, ALIEN};

typedef struct {
    char firstVariant[225];
    char secondVariant[225];
} AlienSprite;

static AlienSprite alienSprites[6] = {
    // FIRST
    {
        {
            _,_,_,_,_,_,_,_,_,_,_,_,_,O,O,
            _,_,_,O,_,_,_,_,_,_,_,_,O,_,_,
            _,_,O,_,O,_,_,_,_,_,_,O,_,_,_,
            _,_,O,_,_,O,_,_,_,_,O,_,_,_,_,
            _,_,_,_,O,O,O,O,O,O,O,O,_,_,_,
            _,_,_,O,O,O,O,O,O,O,O,O,O,_,_,
            _,_,O,O,O,O,O,O,O,O,O,O,O,O,_,
            _,_,O,O,O,O,_,_,O,O,_,_,O,O,_,
            _,_,O,O,O,O,_,_,O,O,_,_,O,O,_,
            _,_,O,O,O,O,O,O,O,O,O,O,O,O,_,
            _,_,O,O,O,O,O,O,O,O,O,O,O,O,_,
            _,_,O,O,O,O,O,O,O,O,O,O,O,O,_,
            _,_,O,O,_,_,_,_,_,_,_,_,O,O,_,
            _,_,O,O,_,_,_,_,_,_,_,_,O,O,_,
            _,O,O,O,O,_,_,_,_,_,_,O,O,O,O
        }, {
            O,O,_,_,_,_,_,_,_,_,_,_,_,_,_,
            _,_,O,_,_,_,_,_,_,_,_,O,_,_,_,
            _,_,_,O,_,_,_,_,_,_,O,_,O,_,_,
            _,_,_,_,O,_,_,_,_,O,_,_,O,_,_,
            _,_,_,O,O,O,O,O,O,O,O,_,_,_,_,
            _,_,O,O,O,O,O,O,O,O,O,O,_,_,_,
            _,O,O,O,O,O,O,O,O,O,O,O,O,_,_,
            _,O,O,_,_,O,O,_,_,O,O,O,O,_,_,
            _,O,O,_,_,O,O,_,_,O,O,O,O,_,_,
            _,O,O,O,O,O,O,O,O,O,O,O,O,_,_,
            _,O,O,O,O,O,O,O,O,O,O,O,O,_,_,
            _,O,O,O,O,O,O,O,O,O,O,O,O,_,_,
            _,O,O,_,_,_,_,_,_,_,_,O,O,_,_,
            _,O,O,_,_,_,_,_,_,_,_,O,O,_,_,
            O,O,O,O,_,_,_,_,_,_,O,O,O,O,_
        }
    },
    // SECOND
    {
        {
            _,_,_,O,O,O,O,O,O,O,O,O,_,_,_,
            _,_,O,O,O,O,O,O,O,O,O,O,O,_,_,
            _,_,O,O,O,O,O,O,O,O,O,O,O,_,_,
            _,_,O,O,_,_,O,O,O,_,_,O,O,_,_,
            O,O,O,O,_,_,O,O,O,_,_,O,O,O,O,
            O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
            O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
            O,O,_,_,O,O,_,_,_,O,O,_,_,O,O,
            O,O,_,_,_,_,O,O,O,_,_,_,_,O,O,
            O,O,_,_,_,_,O,O,O,_,_,_,_,O,O,
            O,O,_,_,O,O,_,_,_,O,O,_,_,O,O,
            O,O,_,_,O,O,_,_,_,O,O,_,_,O,O,
            O,O,_,_,O,O,_,_,_,O,O,_,_,O,O,
            O,O,_,_,O,O,_,_,_,O,O,_,_,O,O,
            O,O,_,_,O,O,_,_,_,O,O,_,_,O,O
        }, {
            _,_,_,O,O,O,O,O,O,O,O,O,_,_,_,
            _,_,O,O,O,O,O,O,O,O,O,O,O,_,_,
            _,_,O,O,O,O,O,O,O,O,O,O,O,_,_,
            _,_,O,O,_,_,O,O,O,_,_,O,O,_,_,
            O,O,O,O,_,_,O,O,O,_,_,O,O,O,O,
            O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
            O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
            O,O,_,_,O,O,_,_,_,O,O,_,_,O,O,
            O,O,_,_,_,_,O,O,O,_,_,_,_,O,O,
            O,O,_,_,_,_,O,O,O,_,_,_,_,O,O,
            _,_,O,O,_,_,O,O,O,_,_,O,O,_,_,
            _,_,O,O,_,_,O,O,O,_,_,O,O,_,_,
            _,_,O,O,_,_,O,O,O,_,_,O,O,_,_,
            _,_,O,O,_,_,_,_,_,_,_,O,O,_,_,
            _,_,O,O,_,_,_,_,_,_,_,O,O,_,_
        }
    },
    // THIRD
    {
        {
            O,O,_,_,_,_,_,_,_,_,_,_,_,O,O,
            O,O,_,_,_,_,_,_,_,_,_,_,_,O,O,
            O,O,_,_,_,_,O,O,O,_,_,_,_,O,O,
            O,O,_,_,_,_,O,O,O,_,_,_,_,O,O,
            _,O,O,_,_,O,O,O,O,O,_,_,O,O,_,
            _,O,O,O,O,O,O,O,O,O,O,O,O,O,_,
            _,_,_,O,O,_,O,O,O,_,O,O,_,_,_,
            _,_,_,_,_,_,O,O,O,_,_,_,_,_,_,
            _,_,_,_,_,_,O,O,O,_,_,_,_,_,_,
            _,_,_,_,_,_,O,O,O,_,_,_,_,_,_,
            _,_,_,_,_,_,O,O,O,_,_,_,_,_,_,
            _,_,_,_,_,_,O,O,O,_,_,_,_,_,_,
            _,_,_,_,O,O,O,O,O,O,O,_,_,_,_,
            O,O,O,O,O,O,_,_,_,O,O,O,O,O,O,
            O,O,O,O,O,_,_,_,_,_,O,O,O,O,O,
        }, {
            _,_,_,_,_,_,O,O,O,_,_,_,_,_,_,
            _,_,O,O,_,_,O,O,O,_,_,O,O,_,_,
            _,O,O,O,O,O,O,O,O,O,O,O,O,O,_,
            O,O,_,_,O,O,O,O,O,O,O,_,_,O,O,
            O,O,_,_,_,_,O,O,O,_,_,_,_,O,O,
            O,O,_,_,_,_,O,O,O,_,_,_,_,O,O,
            _,O,O,_,_,_,O,O,O,_,_,_,O,O,_,
            _,_,O,O,_,_,O,O,O,_,_,O,O,_,_,
            _,_,_,_,_,_,O,O,O,_,_,_,_,_,_,
            _,_,_,_,O,O,O,O,O,O,O,_,_,_,_,
            _,_,_,O,O,O,O,O,O,O,O,O,_,_,_,
            _,_,O,O,_,_,_,_,_,_,O,O,O,_,_,
            _,_,O,O,_,_,_,_,_,_,_,O,O,_,_,
            _,_,_,O,O,O,_,_,_,O,O,O,_,_,_,
            _,_,_,_,_,O,O,_,O,O,_,_,_,_,_,
        }
    },
    // FOURTH
    {
        {
            _,_,_,O,O,O,O,O,O,O,O,O,_,_,_,
            _,_,O,O,O,O,O,O,O,O,O,O,O,_,_,
            _,O,O,O,O,O,O,O,O,O,O,O,O,O,_,
            O,O,O,O,_,_,O,O,O,O,O,_,_,O,O,
            O,O,O,O,_,_,O,O,O,O,O,_,_,O,O,
            _,O,O,O,O,O,O,O,O,O,O,O,O,O,_,
            _,_,O,O,O,O,O,O,O,O,O,O,O,_,_,
            _,_,_,O,O,O,O,O,O,O,O,O,_,_,_,
            _,_,_,_,O,O,_,_,_,O,O,_,_,_,_,
            _,_,_,O,O,_,_,_,_,_,O,O,_,_,_,
            _,_,_,O,O,_,_,_,_,_,O,O,_,_,_,
            _,_,O,O,O,_,_,_,_,_,O,O,O,_,_,
            _,_,O,O,_,_,_,_,_,_,_,O,O,_,_,
            _,O,O,O,_,_,_,_,_,_,_,O,O,O,_,
            _,O,O,_,_,_,_,_,_,_,_,_,O,O,_,
        }, {
            _,_,_,O,O,O,O,O,O,O,O,O,_,_,_,
            _,_,O,O,O,O,O,O,O,O,O,O,O,_,_,
            _,O,O,O,O,O,O,O,O,O,O,O,O,O,_,
            O,O,_,_,O,O,O,O,O,_,_,O,O,O,O,
            O,O,_,_,O,O,O,O,O,_,_,O,O,O,O,
            _,O,O,O,O,O,O,O,O,O,O,O,O,O,_,
            _,_,O,O,O,O,O,O,O,O,O,O,O,_,_,
            _,_,_,O,O,O,O,O,O,O,O,O,_,_,_,
            _,_,_,_,O,O,_,_,_,_,O,O,_,_,_,
            _,_,O,O,O,O,_,_,_,_,O,O,O,_,_,
            _,O,O,O,_,_,_,_,_,_,_,O,O,O,_,
            _,O,O,_,_,_,_,_,_,_,_,_,O,O,_,
            _,O,O,_,_,O,O,_,O,O,_,_,O,O,_,
            _,_,O,O,O,O,O,_,O,O,O,O,O,_,_,
            _,_,_,O,O,_,_,_,_,_,O,O,_,_,_,
        }
    },
    // FIFTH
    {
        {
            _,_,_,O,O,O,O,O,O,O,O,O,_,_,_,
            _,_,O,O,O,O,O,O,O,O,O,O,O,_,_,
            _,O,O,O,O,O,O,O,O,O,O,O,O,O,_,
            O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
            O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
            O,O,O,O,O,O,O,O,_,_,_,O,O,O,O,
            O,O,O,O,O,O,O,O,_,_,_,O,O,O,O,
            O,O,O,O,O,O,O,O,_,_,_,O,O,O,O,
            _,_,O,O,_,_,_,_,O,O,O,O,O,_,_,
            _,_,O,O,_,_,_,_,O,O,O,O,O,_,_,
            _,_,O,O,_,_,_,_,_,_,_,O,O,_,_,
            _,_,O,O,_,_,_,_,_,_,_,O,O,_,_,
            _,_,O,O,_,_,_,_,_,_,_,O,O,_,_,
            _,_,O,O,O,O,_,_,_,_,_,O,O,O,O,
            _,_,O,O,O,O,_,_,_,_,_,O,O,O,O
        }, {
            _,_,_,O,O,O,O,O,O,O,O,O,_,_,_,
            _,_,O,O,O,O,O,O,O,O,O,O,O,_,_,
            _,O,O,O,O,O,O,O,O,O,O,O,O,O,_,
            O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
            O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
            O,O,O,O,_,_,_,O,O,O,O,O,O,O,O,
            O,O,O,O,_,_,_,O,O,O,O,O,O,O,O,
            O,O,O,O,_,_,_,O,O,O,O,O,O,O,O,
            _,_,O,O,O,O,O,_,_,_,_,O,O,_,_,
            _,_,O,O,O,O,O,_,_,_,_,O,O,_,_,
            _,_,O,O,_,_,_,_,_,_,_,O,O,_,_,
            _,_,O,O,_,_,_,_,_,_,_,O,O,_,_,
            _,_,O,O,_,_,_,_,_,_,_,O,O,_,_,
            O,O,O,O,_,_,_,_,_,O,O,O,O,_,_,
            O,O,O,O,_,_,_,_,_,O,O,O,O,_,_
        }
    },
    // SIXTH
    {
        {
            _,_,_,O,O,O,O,O,O,O,O,O,_,_,_,
            _,_,O,O,O,O,O,O,O,O,O,O,O,_,_,
            _,O,O,O,O,O,O,O,O,O,O,O,O,O,_,
            O,O,O,O,_,_,O,O,O,_,_,O,O,O,O,
            O,O,O,O,_,_,O,O,O,_,_,O,O,O,O,
            O,O,O,O,_,_,O,O,O,_,_,O,O,O,O,
            O,O,O,O,_,_,O,O,O,_,_,O,O,O,O,
            O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
            _,_,O,O,_,_,_,_,_,_,_,O,O,_,_,
            _,_,O,O,_,_,_,_,_,_,_,O,O,_,_,
            _,_,O,O,_,_,_,_,_,_,O,O,O,_,_,
            _,O,O,O,_,_,_,_,_,_,O,O,_,_,_,
            _,O,O,O,_,_,_,_,_,_,O,O,_,_,_,
            _,O,O,_,_,_,_,_,_,O,O,O,_,_,_,
            _,O,O,_,_,_,_,_,_,O,O,_,_,_,_,
        }, {
            _,_,_,O,O,O,O,O,O,O,O,O,_,_,_,
            _,_,O,O,O,O,O,O,O,O,O,O,O,_,_,
            _,O,O,O,O,O,O,O,O,O,O,O,O,O,_,
            O,O,O,O,_,_,O,O,O,_,_,O,O,O,O,
            O,O,O,O,_,_,O,O,O,_,_,O,O,O,O,
            O,O,O,O,_,_,O,O,O,_,_,O,O,O,O,
            O,O,O,O,_,_,O,O,O,_,_,O,O,O,O,
            O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
            _,_,O,O,_,_,_,_,_,_,_,O,O,_,_,
            _,_,O,O,_,_,_,_,_,_,_,O,O,_,_,
            _,_,O,O,O,_,_,_,_,_,_,O,O,_,_,
            _,_,_,O,O,_,_,_,_,_,_,O,O,O,_,
            _,_,_,O,O,_,_,_,_,_,_,O,O,O,_,
            _,_,_,O,O,O,_,_,_,_,_,_,O,O,_,
            _,_,_,_,O,O,_,_,_,_,_,_,O,O,_,
        }
    }
};
char currentAlienSpriteVariant;

static char buildingSprite[350] = {
    _,_,_,_,_,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,_,_,_,_,_,
    _,_,_,_,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,_,_,_,_,
    _,_,_,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,_,_,_,
    _,_,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,_,_,
    _,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,_,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_,O,O,O,O,O,O,O,O
};
static char buildings[4][350];

static char playerSprite[150] = {
    _,_,_,_,_,_,_,O,_,_,_,_,_,_,_,
    _,_,_,_,_,_,O,O,O,_,_,_,_,_,_,
    _,_,_,O,O,O,O,O,O,O,O,O,_,_,_,
    _,O,O,O,O,O,O,O,O,O,O,O,O,O,_,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
    O,O,O,O,O,O,O,O,O,O,O,O,O,O,O,
};
unsigned char scoreText[] = "HI-SCORE 000000  SCORE 000000";

bool buildingsDestroyed;
bool playerBulletFired;
bool pauseButtonPressed;

const double alienMovementInterval = 0.8;

/* Destroyed or alive */
bool aliens[6][6];
int remainingAliens;
int bottomAlienRow;
int rightAlienColumn;
int leftAlienColumn;

/* Top-left most alien */
int aliens_x;
int aliens_y;

int playerLives;
int currentAlienColumn;
int currentAlienRow;
int alienMoveDirection;

int playerPosition;
int playerMoveDirection;

int collectedScore;
int playerScore;
int playerHighscore;
int playerHighestWave;
int currentWave;

int playerBulletX;
int playerBulletY;

struct { bool exists; int x; int y; } alienBullets[4];

// Random seed
unsigned int random_seed;

Ticker tickerStepAliens;
Ticker tickerMovement;

InterruptIn btnLeft(p5);
InterruptIn btnRight(p6);
InterruptIn btnFire(p7);
InterruptIn btnPause(p8);

void drawSprite(int x, int y, char *array, int size, unsigned int color) {
    for(int i = 0; i < size; i++) {
        if(array[i]) BSP_LCD_DrawPixel(x + i % 15, y + i / 15, color);
    }
}

void drawAlien(int x, int y, int type) {
    switch(currentAlienSpriteVariant) {
        case 1:
            drawSprite(x, y, alienSprites[type].firstVariant, 225, COLOR_ALIEN);
            break;
        case -1:
            drawSprite(x, y, alienSprites[type].secondVariant, 225, COLOR_ALIEN);
            break;
    }
}

void drawAliens() {
    for(int row = 0; row < NUM_ALEIN_ROWS; row++) {
        for(int column = 0; column < NUM_ALEIN_COLUMNS; column++) {
            if(aliens[column][row]) drawAlien(4 + column * 25 + currentAlienColumn*COLUMN_SIZE, 30 + row * 20 + currentAlienRow*ROW_SIZE, row);
        }
    }
}
void drawPlayer() {
    BSP_LCD_SetTextColor(COLOR_PLAYER);
    drawSprite(playerPosition, 215, playerSprite, 150, COLOR_PLAYER);
}
void stunPlayer() {
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    drawPlayer();
    wait_ms(500);
}

void drawBackground() {
    BSP_LCD_SetTextColor(COLOR_LAND);
    BSP_LCD_FillRect(0, 225, 240, 15);
}
void drawBuildings() {
    if(!buildingsDestroyed) {
        for (int i = 0; i < 350; i++) {
            if(buildings[0][i] == 1) BSP_LCD_DrawPixel(20 + (i % 35), 190 + (i / 35), COLOR_BUILDING);
            if(buildings[1][i] == 1) BSP_LCD_DrawPixel(75 + (i % 35), 190 + (i / 35), COLOR_BUILDING);
            if(buildings[2][i] == 1) BSP_LCD_DrawPixel(130 + (i % 35), 190 + (i / 35), COLOR_BUILDING);
            if(buildings[3][i] == 1) BSP_LCD_DrawPixel(185 + (i % 35), 190 + (i / 35), COLOR_BUILDING);
        }
    }
}
void drawHighscore() {
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_DisplayStringAt(0, 5, scoreText, CENTER_MODE);
}

void fillStringWithInt(unsigned char string[], int number, int numberSize, int offset) {
    int x = number;
    for (int i = numberSize - 1; i >= 0; i--) {
        string[i + offset] = x % 10 + 48;
        x /= 10;
    }
}

void updateAliens() {
    /* Steps the current column by 1, steps row by 1 if aliens reach the edge*/
    currentAlienSpriteVariant *= -1;
    currentAlienColumn += alienMoveDirection;
    aliens_x += COLUMN_SIZE*alienMoveDirection;    
    if(currentAlienColumn - rightAlienColumn * 5 > MAX_COLUMN) {
        currentAlienRow++;
        aliens_y += ROW_SIZE;
        currentAlienColumn--;
        aliens_x -= COLUMN_SIZE;
        alienMoveDirection = -alienMoveDirection;
    } else if(currentAlienColumn + leftAlienColumn * 5 < 0) {
        currentAlienRow++;
        aliens_y += ROW_SIZE;
        currentAlienColumn++;
        aliens_x += COLUMN_SIZE;
        alienMoveDirection = -alienMoveDirection;
    }
    if(currentAlienRow - bottomAlienRow * 1.5 > BUILDING_ROW) buildingsDestroyed = true;
}
void updateScore(int score) {
    playerScore += score;
    collectedScore += score;
    if(collectedScore >= 3500 && playerLives < 3) { playerLives++; collectedScore = 0; }
    if(playerScore > playerHighscore) playerHighscore = playerScore;

    fillStringWithInt(scoreText, playerHighscore, 6, 9);
    fillStringWithInt(scoreText, playerScore, 6, 23);
}
void updateBottomRow() {
    for(int row = 5; row >= 0; row--) {
        bool emptyRow = true;
        for (int column = 0; column < 6; column++) {
            if(aliens[column][row]) {
                emptyRow = false;
                return;
            }
        }
        if(emptyRow) bottomAlienRow = 6 - row;
    }
}
void updateLeftColumn() {
    for(int column = 0; column < 6; column++) {
        bool emptyColumn = true;
        for(int row = 0; row < 6; row++) {
            if(aliens[column][row]) {
                emptyColumn = false;
                return;
            }
        }
        if(emptyColumn) leftAlienColumn = column + 1;
    }
}
void updateRightColumn() {
    for(int column = 5; column >= 0; column--) {
        bool emptyColumn = true;
        for(int row = 0; row < 6; row++) {
            if(aliens[column][row]) {
                emptyColumn = false;
                return;
            }
        }
        if(emptyColumn) rightAlienColumn = 6 - column;
    }
}
void buildingImpact(int building, int location) {
    int height = 5;
    for(int i = -height; i <= height; i++) {
        int width = 6 - abs(i);
        //
        if(width % 2 == 0) width--;
        for(int j = -width; j < width; j++) {
            int offset = location + i*35 + j;
            if(location % 35 + j >= 0 && location %35 + j <= 35 && offset > 0 && offset < 350) buildings[building][offset] = 0;
        }
    }
}

bool buildingIsHit(int x, int y, Entity entity) {
    if(y < 180 || y > 190
        || x < 20 || buildingsDestroyed) return false;

    int horizontalRegion = (x - 20) / 55;
    int horizontalOffset = (x - 20) % 55;

    if(horizontalRegion >= 0 && horizontalRegion < 4 && horizontalOffset <= 35) {
        if(entity == ALIEN) {
            for(int i = 0; i < 10; i++) {
                if(buildings[horizontalRegion][i*35 + horizontalOffset]) {
                    buildingImpact(horizontalRegion, i*35 + horizontalOffset);
                    return true;
                }
            }
        } else {
            for(int i = 9; i >= 0; i--) {
                if(buildings[horizontalRegion][i*35 + horizontalOffset]) {
                    buildingImpact(horizontalRegion, i*35 + horizontalOffset);
                    return true;
                }
            }
        }
    }
    return false;
}
bool playerExistsAt(int x, int y) {
    if(y >= 207 && y <= 215 && x - playerPosition <= 15 && x - playerPosition >= 0) return true;
    return false;
}
void playerImpact() {
    playerLives--;
    for(int i = 0; i < 4; i++) alienBullets[i].exists = false;
    stunPlayer();
}
bool alienImpact(int x, int y) {
    if(playerBulletY < 20 || playerBulletY > 210
        || x < aliens_x || y < aliens_y) return false;

    int horizontalRegion = (x - aliens_x) / 25;
    int verticalRegion = (y - aliens_y) / 20;

    int horizontalOffset = (x - aliens_x) % 25;
    int verticalOffset = (y - aliens_y) % 20;

    if(horizontalRegion >= 0 && horizontalRegion < 6
        && verticalRegion >= 0 && verticalRegion < 6
        && aliens[horizontalRegion][verticalRegion]
        && horizontalOffset <= ALIEN_X_SIZE
        && verticalOffset <= ALIEN_Y_SIZE) {
        aliens[horizontalRegion][verticalRegion] = false;
        remainingAliens--;
        updateBottomRow();
        updateLeftColumn();
        updateRightColumn();
        updateScore(ALIEN_SCORE_AMOUNT);
        return true;
    }
    return false;
}

void updatePlayerBullet() {
    if(playerBulletFired) {
        playerBulletY -= 10;
        /* Remove bullet if it reaches edge */
        if(playerBulletY < 20 || alienImpact(playerBulletX, playerBulletY)) playerBulletFired = false;
        if(buildingIsHit(playerBulletX, playerBulletY, PLAYER)) playerBulletFired = false;
    }
}
void updateAlienBullets() {
    for(int i = 0; i < 4; i++) {
        // Every 0.01s (the interval at which the function is called) there is a check that if one of the four bullets exists exists, there will be a chance of it firing.
        // Isn't really random, but it's sufficient.
        bool aliensFire = rand() % 80 == 0;

        if(alienBullets[i].exists) {
            if(buildingIsHit(alienBullets[i].x, alienBullets[i].y, ALIEN) || alienBullets[i].y > 215) alienBullets[i].exists = false;
            else if(playerExistsAt(alienBullets[i].x, alienBullets[i].y)) playerImpact();
            else alienBullets[i].y += 2;
        } else if(aliensFire) {
            int columnFiring = rand() % 6;
            int rowFiring = -1;

            for(int y = 0; y < 6; y++) {
                if(aliens[columnFiring][y]) {
                    rowFiring = y;
                }
            }
            if(rowFiring != -1) {
                alienBullets[i].exists = true;
                alienBullets[i].x = columnFiring * 25 + 4 + currentAlienColumn*COLUMN_SIZE + 7;
                alienBullets[i].y = rowFiring * 20 + 30 + currentAlienRow*ROW_SIZE + 15;
            }
        }
    }
}

void updatePlayer() {
    playerPosition += playerMoveDirection * 5;
    if(playerPosition < 2) playerPosition = 2;
    if(playerPosition > SCREEN_WIDTH - 15 - 2) playerPosition = SCREEN_WIDTH - 15 - 2;
}
void updateBullets() {
    updatePlayerBullet();
    updateAlienBullets();
}
void drawBullets() {
    BSP_LCD_SetTextColor(COLOR_BULLET);
    /* Draw player bullet */
    if(playerBulletFired) BSP_LCD_DrawVLine(playerBulletX, playerBulletY, BULLET_LENGTH);

    /* Draw alien bullets */
    for(int i = 0; i < 4; i++) {
        if(alienBullets[i].exists) {
            BSP_LCD_DrawVLine(alienBullets[i].x, alienBullets[i].y, BULLET_LENGTH);
        }
    }
}
void drawPlayerLives() {
    if(playerLives > 0) drawSprite(5, 227, playerSprite, 150, COLOR_PLAYER);
    if(playerLives > 1) drawSprite(25, 227, playerSprite, 150, COLOR_PLAYER);
    if(playerLives > 2) drawSprite(45, 227, playerSprite, 150, COLOR_PLAYER);
}
void drawGameObjects() {
    drawBackground();
    drawHighscore();
    drawPlayer();
    drawAliens();
    drawBullets();
    drawBuildings();
    drawPlayerLives();
}

void showStartScreen() {
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(COLOR_BUILDING);
    BSP_LCD_DisplayStringAt(5, 20, (unsigned char*)"PRESS PAUSE TO PLAY!", CENTER_MODE);

    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_DisplayStringAt(5, 110, (unsigned char*)"WAVE: 001", CENTER_MODE);

    unsigned char highscoreTemp[] = "HI-SCORE 000000";
    fillStringWithInt(highscoreTemp, playerHighscore, 6, 9);

    BSP_LCD_DisplayStringAt(5, 130, highscoreTemp, CENTER_MODE);

    while(!pauseButtonPressed) wait_ms(10);
    pauseButtonPressed = false;
}
void initializeGame() {
    /* Clear the LCD */
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    
    playerBulletFired = false;

    /* Initialize variables */
    pauseButtonPressed = false;    
    currentAlienColumn = 0;
    currentAlienRow = 0;
    playerPosition = SCREEN_WIDTH / 2;
    playerMoveDirection = 0;
    alienMoveDirection = RIGHT;

    /* Initialize aliens */
    for(int i = 0; i < 4; i++) alienBullets[i].exists = false;
    for(int i = 0; i < 6; i++) {
        for(int j = 0; j < 6; j++) aliens[i][j] = true;
    }
    aliens_x = 4;
    aliens_y = 30;
    bottomAlienRow = 0;
    rightAlienColumn = 0;
    leftAlienColumn = 0;
    currentAlienSpriteVariant = 1;
    remainingAliens = 36;

    /* Update alien column position with ticker */
    tickerStepAliens.attach(callback(&updateAliens), alienMovementInterval);
}
void initializePersistantVariables() {
    buildingsDestroyed = false;

    playerLives = 3;
    playerScore = 0;
    collectedScore = 0;
    playerHighscore = 0;
    playerHighestWave = 1;
    currentWave = 1;

    /* Initialize buildings */
    for(int i = 0; i < 4; i++)
        memcpy(buildings[i], buildingSprite, 350);
}
void gameOver() {
    drawGameObjects();

    BSP_LCD_SetTextColor(COLOR_PAUSE);
    BSP_LCD_FillRect(0, 107, 240, 20);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_SetBackColor(COLOR_PAUSE);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_DisplayStringAt(5, 110, (unsigned char*)"GAME OVER!", CENTER_MODE);
    wait_ms(3000);

    buildingsDestroyed = false;

    playerLives = 3;
    playerScore = 0;
    fillStringWithInt(scoreText, playerScore, 6, 23);
    collectedScore = 0;
    currentWave = 1;

    for(int i = 0; i < 4; i++)
        memcpy(buildings[i], buildingSprite, 350);

    tickerStepAliens.detach();
    initializeGame();

    showStartScreen();
}

void showWaveInfo() {
    unsigned char waveInfo[] = "WAVE: 000";
    fillStringWithInt(waveInfo, currentWave, 3, 6);

    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_DisplayStringAt(5, 100, waveInfo, CENTER_MODE);
    wait_ms(2000);

    tickerStepAliens.detach();
    initializeGame();
}
void nextWave() {
    drawGameObjects();

    wait_ms(1000);
    currentWave++;
    if(playerHighestWave < currentWave) playerHighestWave = currentWave;
    showWaveInfo();
}

void movePlayerLeft() { playerMoveDirection = LEFT; }
void movePlayerRight() { playerMoveDirection = RIGHT; }
void stopPlayerMovement() { playerMoveDirection = 0; }
void firePlayerBullet() {
    if(!playerBulletFired){
        playerBulletFired = true;
        playerBulletX = playerPosition + PLAYER_X_SIZE / 2;
        playerBulletY = PLAYER_BULLET_STARTING_Y_POS;
    }
}
void pausePressed() {
    pauseButtonPressed = true;
}

void pauseGame() {
    tickerStepAliens.detach();
    drawGameObjects();

    BSP_LCD_SetTextColor(COLOR_PAUSE);
    BSP_LCD_FillRect(0, 100, 240, 33);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_SetBackColor(COLOR_PAUSE);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_DisplayStringAt(5, 110, (unsigned char*)"PAUSED", CENTER_MODE);
    pauseButtonPressed = false;
    while(!pauseButtonPressed) wait_ms(10);
    pauseButtonPressed = false;
    tickerStepAliens.attach(callback(&updateAliens), alienMovementInterval);
}

void initializeControls() {
    btnLeft.fall(callback(&stopPlayerMovement));
    btnLeft.rise(callback(&movePlayerLeft));
    btnRight.fall(callback(&stopPlayerMovement));
    btnRight.rise(callback(&movePlayerRight));
    
    btnFire.rise(callback(&firePlayerBullet));
    btnPause.rise(callback(&pausePressed));
}

void seedRandomNumberGenerator() {
    srand((unsigned) time(0));
}

void loopGame() {
    /* Clear the LCD */
    BSP_LCD_Clear(LCD_COLOR_BLACK);

    if(pauseButtonPressed) { pauseGame(); return; }
    if(playerLives <= 0 || currentAlienRow - bottomAlienRow * 1.5 > MAX_ROW) { gameOver(); return; }
    else if(remainingAliens == 0) { nextWave(); return; }
    else {
        drawGameObjects();

        updatePlayer();
        updateBullets();
    }
}

int main() {
    seedRandomNumberGenerator();

    BSP_LCD_Init();

    initializeControls();
    showStartScreen();

    initializeGame();
    initializePersistantVariables();
    
    while (true) {

        loopGame();

        wait_ms(10);
    }
}