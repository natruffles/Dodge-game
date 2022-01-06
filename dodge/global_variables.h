#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <Vector.h> //used for storing obstacle structures

//USER CUSTOMIZABLE VALUES////////////////////////////////////////
//defines IO pins for joystick
#define X_PIN A15
#define Y_PIN A14
#define BUTTON_PIN 50
#define HIGH_PIN 52

#define ORBITER_HSIZE_PROP 1
#define ORBITER_DISTANCE 2
#define ORBITER_ROT_VELOCITY 2 //1-5 reasonably

#define FRAME_RATE 24  //frame rate of the player, boss, orbiters
#define OBS_FRAME_RATE 8    //frame rate of the obstacles, good idea to have this be less

#define PLAYER_COLOR 0x0000 //black


//PROVIDED BY ELEGOO///////////////////////////////////////////////////////////////
#define DRIVER_IDENTIFIER 0x9341  //TFT LCD driver that is included in the library

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define RED     0xF800
#define DARK_RED 0xE000
#define YELLOW  0xFFE0
#define GOLD     0xFE80
#define ORANGE  0xFC60
#define GREEN   0x07E0
#define DARK_GREEN 0x0601
#define BLUE    0x001F
#define CYAN    0x07FF
#define MAGENTA 0xF81F
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
  bool hasBoss; //true if has a boss, false if not
  int bossHalfSize; //size of the boss
  int bossDiff; //higher number = boss follows closer, 10 is reasonable max
  int numOrbiters; //number of small obstacles orbiting the boss
  int colors[NUM_COLORS]; //different colors created during play, first element is bg color
};

//individual levels
#define NUM_LEVELS 10
//pHalf,maxobs,ogenrate,mindim,maxdim,time,pmove,minospeed,maxospeed,colors
Level customLevel = {0,5,5,4,4,10,20,80.0,50.0,100.0,false,0,0,0,{WHITE, WHITE, WHITE, WHITE}};
Level level1 = {1,5,5,4,4,10,20,80.0,50.0,100.0, false,0,0,0,{GREEN, DARK_GREEN, DARK_GREEN, GREEN}};
Level level2 = {2,5,6,4,6,12,20,80.0,55.0,105.0,false,0,0,0,{GOLD, YELLOW, GOLD, ORANGE}};
Level level3 = {3,6,7,4,7,13,25,90.0,60.0,110.0,false,0,0,0,{BLUE,CYAN,CYAN,BLUE}};
Level level4 = {4,5,5,4,10,15,30,90.0,80.0,130.0,false,0,0,0,{MAGENTA,RED,MAGENTA,MAGENTA}};
Level level5 = {5,5,3,3,5,10,30,100.0,50.0,100.0,true,8,2,0,{RED,DARK_RED,RED,MAGENTA}};
Level level6 = {6,5,5,3,7,12,30,100.0,40.0,120.0,true,10,4,0,{DARK_RED,RED,MAGENTA,DARK_RED}};
Level level7 = {7,6,5,4,7,12,30,90.0,40.0,120.0,true,10,2,4,{WHITE,YELLOW,GOLD,WHITE}};
Level level8 = {8,6,10,2,5,12,30,100.0,70.0,160.0,false,0,0,0,{GREEN,DARK_GREEN,GREEN,GOLD}};
Level level9 = {9,6,6,3,7,12,30,80.0,40.0,80.0,true,12,2,3,{MAGENTA,RED,RED,DARK_RED}};
Level levels[NUM_LEVELS] = {customLevel, level1, level2, level3, level4, level5, level6, level7, level8, level9};

//stores the current level number 
int lvl = 0;

//used for selecting a number for level select, etc.
struct customNumberSelect {
  float value; //current value stored in the number
  int xPos; //x position of number printed on screen
  int yPos; //y position of number printed on screen
  int minValue; //minimum value of number before looping round to maximum
  int maxValue; //maximum value of number before looping round to minimum
  int changeInValue; //how much does value change by when scrolling
  int scrollDelay; //how many milliseconds does each scroll take
};

//when you select the level number
struct customNumberSelect lvlSelect = {0, 172, 159, 0, NUM_LEVELS-1, 1, 250};

//when you select the 9 different values when creating a custom level
#define NUM_CUSTOM_NUMS 9
customNumberSelect customLevelScreen[NUM_CUSTOM_NUMS] = {
  {5, 195, 31, 1, 20, 1, 250}, //player Size
  {5, 195, 56, 1, 20, 1, 250}, //max num obstacles
  {4, 195, 81, 1, 10, 1, 250}, //obs gen time
  {4, 195, 106, 1, 20, 1, 250}, //min obs length
  {10, 195, 131, 1, 20, 1, 250}, //max obs length
  {20, 195, 156, 5, 60, 1, 150}, //lvl duration
  {80, 195, 181, 20, 200, 5, 150}, //player speed
  {50, 195, 206, 10, 200, 5, 150}, //min obs speed
  {100, 195, 231, 10, 200, 5, 150}, //max obs speed
};

#define MAX_ORBITERS 4
struct Orbiter {
  float x, y, priorX, priorY, angularPosition;
};

struct Boss {
  float x, y, priorX, priorY; //things like size, speed are level specific
  Orbiter orbiters[MAX_ORBITERS]; //currently 4
};
struct Boss boss = {0,0,0,0,{{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0},{0,0,0,0,0}}}; 
