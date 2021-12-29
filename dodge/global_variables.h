#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <Vector.h> //used for storing obstacle structures

//USER CUSTOMIZABLE VALUES////////////////////////////////////////
//defines IO pins for joystick
#define X_PIN A15
#define Y_PIN A14
#define BUTTON_PIN 50
#define HIGH_PIN 52

#define FRAME_RATE 24  //frame rate of the player
#define OBS_FRAME_RATE 10    //frame rate of the obstacles, good idea to have this be less

#define PLAYER_COLOR 0x0000 //black


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

const int MILLIS_DELAY = 1/((float)FRAME_RATE) * 1000.0; //milliseconds delay between frames
const int FREQUENCY = FRAME_RATE / OBS_FRAME_RATE; //after how many frames do obstacle displays update
int freqCounter = 0; //counts up to the frequency and then displays the new obstacle, updates priorX and priorY

//dark red, dark blue, violet, pink
const int COLORS_SIZE = 4;
const int COLORS[COLORS_SIZE] = {0xD882, 0x01F8, 0xA01F, 0xE01C};

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

struct Player aPlayer = {0,0,0,0,0, PLAYER_COLOR};
Player* player = &aPlayer;

//define obstacle struct which contains all information needed for an obstacle
struct Obstacle {
  float x, y, priorX, priorY, dX, dY;
  int hsX, hsY, color; //can be a rectangle rather than just square
};

Obstacle oArray[20]; //space in memory for 20 obstacles although that will not happen
Vector<Obstacle> oVector(oArray);

//stores if gameOver
bool gameOver = false;

//stores user score
float score = 0;

#define NUM_COLORS 4
struct Level {
  int levelNum, //same as value in the array
  pHalfSize, //half the width of the player
  maxObstacles, //maximum number of obstacles on the screen at a given time
  obsGenRate, //how many frames apart do obstacles spawn
  minObsDim, //minimum dimension of an obstacle
  maxObsDim, //maximum dimension of an obstacle
  duration; //number of seconds before beating the level
  float pMovementSpeed, //movement speed of player
  minObsSpeed, //minimum obstacle speed along one axis
  maxObsSpeed; //maximum obstacle speed along one axis
  int colors[NUM_COLORS]; //different colors created during play, first element is bg color
};

//individual levels
#define NUM_LEVELS 3
Level customLevel = {0,0,0,0,0,0,0,0,0,0,{RED, RED, RED, RED}};
Level level1 = {1,5,5,4,4,10,20,80.0,50.0,100.0, {BLUE,BLUE,BLUE,BLUE}};
Level level2 = {2,5,6,4,6,12,20,80.0,55.0,105.0,{RED,RED,RED,RED}};
Level levels[NUM_LEVELS] = {customLevel, level1, level2};

//stores the current level number 
int lvl = 0;
