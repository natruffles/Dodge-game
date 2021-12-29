#include "global_variables.h"
#include "setup_input_functions.h"
#include "processing_functions.h"
#include "display_functions.h"

Elegoo_TFTLCD aTft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
Elegoo_TFTLCD* tft = &aTft;

void setup(void) {
  //for debugging
  Serial.begin(9600);

  //to use random numbers, make sure that pin is unconnected
  randomSeed(analogRead(A13));

  joystickSetup();
  
  displayBootScreen(tft);
  setUpForNextGame(player, joystick, tft);
}


void loop(void) {
  
  //INPUT//////////////////////////////////////////////////////////////
  getJoystickStatus(joystick);
  pauseIfJoystickPressed(joystick, tft);

  //PROCESSING/////////////////////////////////////////////////////////
  //uses the global obstacles vector
  generateObstacles(tft);
  moveObstacles(tft);

  movePlayer(player, joystick, tft);

  if ((collisionDetect(player)) || (score < 0)) {
    gameOver = true;
  }
  else {
    score -= (float)1/FRAME_RATE;
  }


  //OUTPUT AND GAME OVER//////////////////////////////////////////////////////////////
  if (!gameOver) {
    frameTimeDelay();
    displayPlayer(player, tft);
    displayObstacles(tft);
    displayScore(score, tft);
    
  }
  else {   //game is over
    freqCounter = 0; //forces obstacles to be displayed
    displayObstacles(tft);
    if (score > 0) {deathAnimation(player, tft);}
    displayEndScreen(score, tft);
    setUpForNextGame(player, joystick, tft);
  }   
}
