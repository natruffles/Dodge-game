#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library

//USER CUSTOMIZABLE VALUES////////////////////////////////////////
//defines IO pins for joystick
#define X_PIN A15
#define Y_PIN A14
#define BUTTON_PIN 50
#define HIGH_PIN 52

#define HALF_SIZE 5 //player's width is 2 * half size
#define PLAYER_COLOR 0x07E0 //green

#define MOVEMENT_SPEED 2.0 //how many pixels the player moves per frame

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
