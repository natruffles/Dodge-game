/*
 * TODO:
 * fix the start screen so that it gives direction and prompts for user input. aka fix the setup function to work with other setup function
 * make unpausing numbers larger and centered. 
 * make it so obstacles can move diagonally
 * connect a speaker and make it play a background track and a death sound 
 */

#include "global_variables.h"

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

//SETUP FUNCTIONS///////////////////////////////////////////////////
//sets up the joystick hardware
void joystickSetup() {
  pinMode(joystick->xPin, INPUT);
  pinMode(joystick->yPin, INPUT);
  pinMode(joystick->bPin, INPUT);
  pinMode(joystick->hPin, OUTPUT);
  digitalWrite(joystick->hPin, HIGH);
  digitalWrite(joystick->bPin, HIGH);
}

//clears display, prints starting screen, displays "PONG"
void displayStartScreen() {
  tft.reset();
  tft.begin(DRIVER_IDENTIFIER);
  tft.setRotation(2);  //rotates the screen 180 degrees
  tft.fillScreen(BLACK);
  
  tft.setCursor(tft.width()/2 - 60, tft.height() / 2 - 30);
  tft.setTextColor(WHITE);  tft.setTextSize(3);
  tft.print("DODGE!");
  tft.setTextSize(2);
  tft.setCursor(tft.width()/2 - 100, tft.height() / 2);
  tft.print("Press to start");
}


//INPUT FUNCTIONS//////////////////////////////////////////////////////////////
void getJoystickStatus(struct Joystick* aJoystick) {
  aJoystick->x = analogRead(aJoystick->xPin);
  aJoystick->y = analogRead(aJoystick->yPin);
  aJoystick->buttonVal = digitalRead(aJoystick->bPin);
}


//PROCESSING FUNCTIONS////////////////////////////////////////////////////////
void pauseIfJoystickPressed(struct Joystick* j) {
  //if the user presses the button, game is paused
  if (j->buttonVal == 1) {
    //waits for the user to release the button
    getJoystickStatus(j);
    while (j->buttonVal == 1) {
      getJoystickStatus(j);
    }

    //waits for the user to click the button again
    getJoystickStatus(j);
    while (j->buttonVal == 0) {
      getJoystickStatus(j);
    }
    tft.fillRect(0,0,20,30, BLACK);
    tft.setCursor(0,0);
    tft.setTextColor(WHITE);  tft.setTextSize(3);
    tft.print("3");
    delay(1000);
    tft.fillRect(0,0,20,30, BLACK);
    tft.setCursor(0,0);
    tft.print("2");
    delay(1000);
    tft.fillRect(0,0,20,30, BLACK);
    tft.setCursor(0,0);
    tft.print("1");
    delay(1000);
    tft.fillRect(0,0,20,30, BLACK);
  }
}


//moves player in 8 directions with a speed 
void movePlayer(struct Player* p, struct Joystick* j) {
  p->priorX = p->x;
  p->priorY = p->y;
  
  //move left
  if (j->x > 1000) {
    (p->x) -= MOVEMENT_SPEED/FRAME_RATE;
  }
  //or move right
  else if (j->x < 10) {
    (p->x) += MOVEMENT_SPEED/FRAME_RATE;
  }

  //move down
  if (j->y > 1000) {
    (p->y) += MOVEMENT_SPEED/FRAME_RATE;
  }
  //or move up
  else if (j->y < 10) {
    (p->y) -= MOVEMENT_SPEED/FRAME_RATE;
  }

  //makes sure that the player is inbounds
  if ((p->x - p->halfSize) < 0) {
    p->x = p->halfSize;
  }
  else if ((p->x + p->halfSize) > tft.width()) {
    p->x = tft.width() - p->halfSize;
  }
  if ((p->y - p->halfSize) < 0) {
    p->y = p->halfSize;
  }
  else if ((p->y + p->halfSize) > tft.height()) {
    p->y = tft.height() - p->halfSize;
  }
}

//generates an obstacle every so often with a random size and start position
void generateObstacles() {

  //if there are less than the number of allowable obstacles, generate a new one 
  if (oVector.size() < MAX_OBSTACLES) {
    Obstacle obstacle = {0,0,0,0, //x,y,priorX,priorY positions
                       (float)random(MIN_OBS_SPEED/FRAME_RATE * 100.0, MAX_OBS_SPEED/FRAME_RATE * 100.0) * 0.01, //movement speedX
                       0, //placeholder for movement speedY 
                       random(SMALLEST_OBS_DIMENSION, LARGEST_OBS_DIMENSION), //halfsizeX
                       random(SMALLEST_OBS_DIMENSION, LARGEST_OBS_DIMENSION), //halfsizeY
                       GREEN};

    int obsPosition = random(0, 4);  //determines which edge the obstacle will come from
    int diagonalVelocity = random((int)-obstacle.dX, (int)obstacle.dX + 1);
    
    //based on which edge, initial position (x,y) and velocity (dX, dY) are set
    switch (obsPosition) {
      case 0:
        //left side
        obstacle.x = 0 - obstacle.hsX;
        obstacle.y = random(obstacle.hsY * 2, tft.height() - obstacle.hsY * 2);
        obstacle.dY = diagonalVelocity;
        break;
      case 1:
        //top side
        obstacle.x = random(obstacle.hsX * 2, tft.width() - obstacle.hsX * 2);
        obstacle.y = 0 - obstacle.hsY;
        (obstacle.dY) = (obstacle.dX);
        obstacle.dX = diagonalVelocity;
        break;
      case 2:
        //right side 
        obstacle.x = tft.width() + obstacle.hsX;
        obstacle.y = random(obstacle.hsY * 2, tft.height() - obstacle.hsY * 2);
        (obstacle.dX) *= -1; //moves to the left
        obstacle.dY = diagonalVelocity;
        break;
      case 3:
        //bottom side
        obstacle.x = random(obstacle.hsX * 2, tft.width() - obstacle.hsX * 2);
        obstacle.y = tft.height() + obstacle.hsY;
        (obstacle.dY) = (obstacle.dX) * -1;
        obstacle.dX = diagonalVelocity;
        break;
    }
    obstacle.priorX = obstacle.x;
    obstacle.priorY = obstacle.y;

    oVector.push_back(obstacle);
  }
  
}


//moves all obstacles based on their movement speed
void moveObstacles() {
  //move obstacles every FREQUENCY frames
  if (freqCounter < FREQUENCY) {
    freqCounter++;
  }
  else {
    freqCounter = 0; //resets freq counter and also tells to display obstacles
    //moves each obstacle based on its movement speed
    //and sets the prior value of x and y
    for (int i = 0; i < oVector.size(); i++) {
      
      
      oVector[i].priorX = oVector[i].x;
      oVector[i].priorY = oVector[i].y;
      
      oVector[i].x += oVector[i].dX * FREQUENCY;
      oVector[i].y += oVector[i].dY * FREQUENCY;
    }
  
    //removes obstacle from array if it is out of bounds
    for (int j = oVector.size() - 1; j >= 0; j--) {
      if ((oVector[j].priorX > tft.width() + oVector[j].hsX)
          || (oVector[j].priorX < (-1 * oVector[j].hsX))
          || (oVector[j].priorY > tft.height() + oVector[j].hsY)
          || (oVector[j].priorY < (-1 * oVector[j].hsY))) {
            oVector.remove(j);
          }   
    }
    

  }
  
}

//returns true if there is a collision on any of the four sides
bool rectVsRect(Player* p, Obstacle o) {
  return (p->x + p->halfSize > o.x - o.hsX &&
    p->x - p->halfSize < o.x + o.hsX &&
    p->y + p->halfSize > o.y - o.hsY &&
    p->y - p->halfSize < o.y + o.hsY);
}

//checks collision between player and any obstacle
bool collisionDetect(Player* p) {
  for (int i = 0; i < oVector.size(); i++) {
    if (rectVsRect(p, oVector[i])) {
      return true;
    }
  }
  return false;
}


//OUTPUT FUNCTIONS///////////////////////////////////////////////////////////
//displays the player, a small square with its center at (x,y)
void displayPlayer(struct Player* p) {
  if (p->x != p->priorX || p->y != p->priorY) {
    tft.fillRect(p->priorX - p->halfSize, p->priorY - p->halfSize, 
    p->halfSize*2, p->halfSize*2, BLACK);
  }
  
  tft.fillRect(p->x - p->halfSize, p->y - p->halfSize, 
    p->halfSize*2, p->halfSize*2, p->color);
}


//makes the game run at ~30 fps (not counting time it takes to display the pixels)
void frameTimeDelay() {
  static int currentTime = 0;
  static int oldFrameTime = 0;

  currentTime = millis();
  //do nothing until MILLIS_DELAY time has passed (based on framerate)
  while ((currentTime - oldFrameTime) < MILLIS_DELAY) {
    currentTime = millis();
  }
  oldFrameTime = millis();
}


//displays all obstacles on the screen
void displayObstacles() {
  if (freqCounter == 0) {
    for (int i = 0; i < oVector.size(); i++) {
      tft.fillRect((int)(oVector[i].priorX - oVector[i].hsX), 
      (int)(oVector[i].priorY - oVector[i].hsY),  
      oVector[i].hsX * 2, 
      oVector[i].hsY * 2, 
      BLACK);
  
      tft.drawRect(oVector[i].x - oVector[i].hsX, 
      oVector[i].y - oVector[i].hsY,  
      oVector[i].hsX * 2, 
      oVector[i].hsY * 2, 
      oVector[i].color);
    }
  }
}


//displays player death animation
void deathAnimation(Player* p) {
  for (int i = 0; i < 5; i++) {
    for (int radius = 5; radius <= 15; radius += 2) {
      tft.fillCircle(p->x, p->y, radius, RED);
      delay(33);
      tft.fillCircle(p->x, p->y, radius, BLACK);
    }
  }
}


//displays the score every time
void displayScore(float playerScore) {
  tft.fillRect(0,0,24,8,BLACK);
  tft.drawRect(-1,-1,26,10, WHITE);
  tft.setCursor(0,0);
  tft.setTextSize(1);
  tft.setTextColor(WHITE);
  tft.print((int)playerScore);
}

void displayGameScore(float playerScore) {
  tft.fillScreen(BLACK);
  tft.setCursor(tft.width()/2-100, tft.height()/2-50);
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  tft.print("Your score: ");
  tft.print((int)playerScore);
  tft.setCursor(tft.width()/2-100, tft.height()/2);
  tft.print("Play again?");
}


void setUpForNextGame(Player* p, Joystick* j) {
  oVector.clear();   //remove all obstacles
  p->x = tft.width()/2; //set player to middle of screen
  p->y = tft.height()/2;
  randomSeed(analogRead(A13)); //new randoms

  score = 0;
  gameOver = false;
  freqCounter = 0;

  getJoystickStatus(j);
  while (j->buttonVal == 0) {
      getJoystickStatus(j);
  }
  tft.fillRect(0,0,20,30, BLACK);
  tft.setCursor(0,0);
  tft.setTextColor(WHITE);  tft.setTextSize(3);
  tft.print("3");
  delay(1000);
  tft.fillRect(0,0,20,30, BLACK);
  tft.setCursor(0,0);
  tft.print("2");
  delay(1000);
  tft.fillRect(0,0,20,30, BLACK);
  tft.setCursor(0,0);
  tft.print("1");
  delay(1000);
  tft.fillRect(0,0,20,30, BLACK);

  tft.fillScreen(BLACK);

  
  
}





void setup(void) {
  //for debugging
  Serial.begin(9600);

  //to use random numbers, make sure that pin is unconnected
  randomSeed(analogRead(A13));

  joystickSetup();
  
  displayStartScreen();
  setUpForNextGame(player, joystick);
}


void loop(void) {
  
  for (int i = 0; i < oVector.size(); i++) {
    Serial.print(oVector[i].hsX);
    Serial.print(oVector[i].hsY);
    Serial.print(" ");
    Serial.print(oVector.size());
  }
  Serial.println();
  
  
  //INPUT//////////////////////////////////////////////////////////////
  getJoystickStatus(joystick);








  //PROCESSING/////////////////////////////////////////////////////////
  pauseIfJoystickPressed(joystick);

  //uses the global obstacles vector
  generateObstacles();
  moveObstacles();

  movePlayer(player, joystick);

  if (collisionDetect(player)) {
    gameOver = true;
  }
  else {
    score += (float)1/FRAME_RATE;
  }








  //OUTPUT AND GAME OVER//////////////////////////////////////////////////////////////
  if (!gameOver) {
    frameTimeDelay();
    displayPlayer(player);
    displayObstacles();
    displayScore(score);
    
  }
  else {   //game is over
    freqCounter = 0; //forces obstacles to be displayed
    displayObstacles();
    
    deathAnimation(player);
    displayGameScore(score);
    setUpForNextGame(player, joystick);
  }   
}
