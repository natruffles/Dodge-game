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
  
  tft.setCursor(tft.width()/2 - 60, tft.height() / 2);
  tft.setTextColor(WHITE);  tft.setTextSize(3);
  tft.println("DODGE!");
  delay(1500);
  tft.fillScreen(BLACK);
  delay(500);
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
    tft.fillRect(0,0,6,8, BLACK);
    tft.setCursor(0,0);
    tft.setTextColor(WHITE);  tft.setTextSize(1);
    tft.print("3");
    delay(1000);
    tft.fillRect(0,0,6,8, BLACK);
    tft.setCursor(0,0);
    tft.print("2");
    delay(1000);
    tft.fillRect(0,0,6,8, BLACK);
    tft.setCursor(0,0);
    tft.print("1");
    delay(1000);
    tft.fillRect(0,0,6,8, BLACK);
  }
}


//moves player in 8 directions with a speed 
void movePlayer(struct Player* p, struct Joystick* j) {
  p->priorX = p->x;
  p->priorY = p->y;
  
  //move left
  if (j->x > 1000) {
    (p->x) -= MOVEMENT_SPEED;
  }
  //or move right
  else if (j->x < 10) {
    (p->x) += MOVEMENT_SPEED;
  }

  //move down
  if (j->y > 1000) {
    (p->y) += MOVEMENT_SPEED;
  }
  //or move up
  else if (j->y < 10) {
    (p->y) -= MOVEMENT_SPEED;
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
                       (float)random(MIN_OBS_SPEED * 100.0, MAX_OBS_SPEED * 100.0) * 0.01, //movement speedX
                       0, //placeholder for movement speedY 
                       random(SMALLEST_OBS_DIMENSION, LARGEST_OBS_DIMENSION), //halfsizeX
                       random(SMALLEST_OBS_DIMENSION, LARGEST_OBS_DIMENSION), //halfsizeY
                       GREEN};

    int obsPosition = random(0, 4);  //determines which edge the obstacle will come from
    
    //based on which edge, initial position (x,y) and velocity (dX, dY) are set
    switch (obsPosition) {
      case 0:
        //left side
        obstacle.x = 0 - obstacle.hsX + 2;
        obstacle.y = random(obstacle.hsY + 2, tft.height() - obstacle.hsY - 2);
        break;
      case 1:
        //top side
        obstacle.x = random(obstacle.hsX + 2, tft.width() - obstacle.hsX - 2);
        obstacle.y = 0 - obstacle.hsY + 2;
        (obstacle.dY) = (obstacle.dX);
        obstacle.dX = 0;
        break;
      case 2:
        //right side 
        obstacle.x = tft.width() + obstacle.hsX - 2;
        obstacle.y = random(obstacle.hsY + 2, tft.height() - obstacle.hsY - 2);
        (obstacle.dX) *= -1; //moves to the left
        break;
      case 3:
        //bottom side
        obstacle.x = random(obstacle.hsX + 2, tft.width() - obstacle.hsX - 2);
        obstacle.y = tft.height() + obstacle.hsY - 2;
        (obstacle.dY) = (obstacle.dX) * -1;
        obstacle.dX = 0;
        break;
    }
    obstacle.priorX = obstacle.x;
    obstacle.priorY = obstacle.y;

    oVector.push_back(obstacle);
  }
  
}


//moves all obstacles based on their movement speed
void moveObstacles() {
  if (oVector.size() != 0) {
  
    //moves each obstacle based on its movement speed
    //and sets the prior value of x and y
    for (int i = 0; i < oVector.size(); i++) {
      oVector[i].priorX = oVector[i].x;
      oVector[i].priorY = oVector[i].y;
      
      oVector[i].x += oVector[i].dX;
      oVector[i].y += oVector[i].dY;
    }
  
    //removes obstacle from array if it is out of bounds
    for (int j = oVector.size() - 1; j >= 0; j--) {
      if ((oVector[j].x > tft.width() + oVector[j].hsX + 5)
          || (oVector[j].x < (-1 * oVector[j].hsX) - 5)
          || (oVector[j].y > tft.height() + oVector[j].hsY + 5)
          || (oVector[j].y < (-1 * oVector[j].hsY) - 5)) {
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

  getJoystickStatus(j);
  while (j->buttonVal == 0) {
      getJoystickStatus(j);
  }
  tft.fillRect(0,0,6,8, BLACK);
  tft.setCursor(0,0);
  tft.setTextColor(WHITE);  tft.setTextSize(1);
  tft.print("3");
  delay(1000);
  tft.fillRect(0,0,6,8, BLACK);
  tft.setCursor(0,0);
  tft.print("2");
  delay(1000);
  tft.fillRect(0,0,6,8, BLACK);
  tft.setCursor(0,0);
  tft.print("1");
  delay(1000);
  tft.fillRect(0,0,6,8, BLACK);

  tft.fillScreen(BLACK);

  
  
}





void setup(void) {
  //for debugging
  Serial.begin(9600);

  //to use random numbers, make sure that pin is unconnected
  randomSeed(analogRead(A13));

  
  joystickSetup();
  
  displayStartScreen();

  //center the player in the screen
  player->x = tft.width()/2;
  player->y = tft.width()/2;
  
  displayPlayer(player);
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
  else {
    deathAnimation(player);
    displayGameScore(score);
    setUpForNextGame(player, joystick);
  }





    
}










unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(BLACK);
  tft.fillScreen(RED);
  tft.fillScreen(GREEN);
  tft.fillScreen(BLUE);
  tft.fillScreen(BLACK);
  return micros() - start;
}

unsigned long testText() {
  tft.fillScreen(BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
  return micros() - start;
}

unsigned long testLines(uint16_t color) {
  unsigned long start, t;
  int           x1, y1, x2, y2,
                w = tft.width(),
                h = tft.height();

  tft.fillScreen(BLACK);

  x1 = y1 = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t     = micros() - start; // fillScreen doesn't count against timing

  tft.fillScreen(BLACK);

  x1    = w - 1;
  y1    = 0;
  y2    = h - 1;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  tft.fillScreen(BLACK);

  x1    = 0;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = w - 1;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);
  t    += micros() - start;

  tft.fillScreen(BLACK);

  x1    = w - 1;
  y1    = h - 1;
  y2    = 0;
  start = micros();
  for(x2=0; x2<w; x2+=6) tft.drawLine(x1, y1, x2, y2, color);
  x2    = 0;
  for(y2=0; y2<h; y2+=6) tft.drawLine(x1, y1, x2, y2, color);

  return micros() - start;
}

unsigned long testFastLines(uint16_t color1, uint16_t color2) {
  unsigned long start;
  int           x, y, w = tft.width(), h = tft.height();

  tft.fillScreen(BLACK);
  start = micros();
  for(y=0; y<h; y+=5) tft.drawFastHLine(0, y, w, color1);
  for(x=0; x<w; x+=5) tft.drawFastVLine(x, 0, h, color2);

  return micros() - start;
}

unsigned long testRects(uint16_t color) {
  unsigned long start;
  int           n, i, i2,
                cx = tft.width()  / 2,
                cy = tft.height() / 2;

  tft.fillScreen(BLACK);
  n     = min(tft.width(), tft.height());
  start = micros();
  for(i=2; i<n; i+=6) {
    i2 = i / 2;
    tft.drawRect(cx-i2, cy-i2, i, i, color);
  }

  return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long start, t = 0;
  int           n, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(BLACK);
  n = min(tft.width(), tft.height());
  for(i=n; i>0; i-=6) {
    i2    = i / 2;
    start = micros();
    tft.fillRect(cx-i2, cy-i2, i, i, color1);
    t    += micros() - start;
    // Outlines are not included in timing results
    tft.drawRect(cx-i2, cy-i2, i, i, color2);
  }

  return t;
}

unsigned long testFilledCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int x, y, w = tft.width(), h = tft.height(), r2 = radius * 2;

  tft.fillScreen(BLACK);
  start = micros();
  for(x=radius; x<w; x+=r2) {
    for(y=radius; y<h; y+=r2) {
      tft.fillCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testCircles(uint8_t radius, uint16_t color) {
  unsigned long start;
  int           x, y, r2 = radius * 2,
                w = tft.width()  + radius,
                h = tft.height() + radius;

  // Screen is not cleared for this one -- this is
  // intentional and does not affect the reported time.
  start = micros();
  for(x=0; x<w; x+=r2) {
    for(y=0; y<h; y+=r2) {
      tft.drawCircle(x, y, radius, color);
    }
  }

  return micros() - start;
}

unsigned long testTriangles() {
  unsigned long start;
  int           n, i, cx = tft.width()  / 2 - 1,
                      cy = tft.height() / 2 - 1;

  tft.fillScreen(BLACK);
  n     = min(cx, cy);
  start = micros();
  for(i=0; i<n; i+=5) {
    tft.drawTriangle(
      cx    , cy - i, // peak
      cx - i, cy + i, // bottom left
      cx + i, cy + i, // bottom right
      tft.color565(0, 0, i));
  }

  return micros() - start;
}

unsigned long testFilledTriangles() {
  unsigned long start, t = 0;
  int           i, cx = tft.width()  / 2 - 1,
                   cy = tft.height() / 2 - 1;

  tft.fillScreen(BLACK);
  start = micros();
  for(i=min(cx,cy); i>10; i-=5) {
    start = micros();
    tft.fillTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      tft.color565(0, i, i));
    t += micros() - start;
    tft.drawTriangle(cx, cy - i, cx - i, cy + i, cx + i, cy + i,
      tft.color565(i, i, 0));
  }

  return t;
}

unsigned long testRoundRects() {
  unsigned long start;
  int           w, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(BLACK);
  w     = min(tft.width(), tft.height());
  start = micros();
  for(i=0; i<w; i+=6) {
    i2 = i / 2;
    tft.drawRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(i, 0, 0));
  }

  return micros() - start;
}

unsigned long testFilledRoundRects() {
  unsigned long start;
  int           i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(BLACK);
  start = micros();
  for(i=min(tft.width(), tft.height()); i>20; i-=6) {
    i2 = i / 2;
    tft.fillRoundRect(cx-i2, cy-i2, i, i, i/8, tft.color565(0, i, 0));
  }

  return micros() - start;
}
