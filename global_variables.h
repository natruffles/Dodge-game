#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <Vector.h> //used for storing obstacle structures

//USER CUSTOMIZABLE VALUES////////////////////////////////////////
//defines IO pins for joystick
#define X_PIN A15
#define Y_PIN A14
#define BUTTON_PIN 50
#define HIGH_PIN 52

#define FRAME_RATE 30

#define HALF_SIZE 10 //player's width is 2 * half size
#define PLAYER_COLOR 0xFFFF //white

#define MOVEMENT_SPEED 2.0 //how many pixels the player moves per frame

#define MAX_OBSTACLES 5 //maximum number of obstacles on the screen at once
#define MIN_OBS_SPEED 3.0
#define MAX_OBS_SPEED 6.0

#define SMALLEST_OBS_DIMENSION 5
#define LARGEST_OBS_DIMENSION 15

//PROVIDED BY ELEGOO///////////////////////////////////////////////////////////////
#define DRIVER_IDENTIFIER 0x9341  //TFT LCD driver that is included in the library

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

////////////////////////////////////////////////////////////////////////////////////

const int MILLIS_DELAY = 1/((float)FRAME_RATE) * 1000.0;

// ROYGBIV but indigo is cyan and violet is magenta
#define COLORS [0xF800, 0xFBE0, 0xFFE0, 0x07E0, 0x001F, 0x07FF, 0xF81F]

//define joystick struct which contains all hardware values and information
struct Joystick {
  int xPin, yPin, bPin, hPin, x, y, buttonVal;
};

//initializing joystick and joystick pointer
struct Joystick aJoystick = {X_PIN, Y_PIN, BUTTON_PIN, HIGH_PIN, 512, 512, 0};
Joystick* joystick = &aJoystick;

//define player struct which contains all information relating to the player
struct Player {
  float x, y, priorX, priorY;
  int halfSize, color;
};

struct Player aPlayer = {100, 100, 100, 100, HALF_SIZE, PLAYER_COLOR};
Player* player = &aPlayer;

//define obstacle struct which contains all information needed for an obstacle
struct Obstacle {
  float x, y, priorX, priorY, dX, dY;
  int hsX, hsY, color; //can be a rectangle rather than just square
};

Obstacle oArray[MAX_OBSTACLES];
Vector<Obstacle> oVector(oArray);

//stores if gameOver
bool gameOver = false;

//stores user score
float score = 0;
