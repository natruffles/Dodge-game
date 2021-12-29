//sets up the joystick hardware
void joystickSetup() {
  pinMode(joystick->xPin, INPUT);
  pinMode(joystick->yPin, INPUT);
  pinMode(joystick->bPin, INPUT);
  pinMode(joystick->hPin, OUTPUT);
  digitalWrite(joystick->hPin, HIGH);
  digitalWrite(joystick->bPin, HIGH);
}

void getJoystickStatus(struct Joystick* aJoystick) {
  aJoystick->x = analogRead(aJoystick->xPin);
  aJoystick->y = analogRead(aJoystick->yPin);
  aJoystick->buttonVal = digitalRead(aJoystick->bPin);
}

void pauseIfJoystickPressed(struct Joystick* j, Elegoo_TFTLCD * tft) {
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
    tft->fillRect(0,0,20,26, BLACK);
    tft->setCursor(0,0);
    tft->setTextColor(WHITE);  tft->setTextSize(3);
    tft->print("3");
    delay(1000);
    tft->fillRect(0,0,20,26, BLACK);
    tft->setCursor(0,0);
    tft->print("2");
    delay(1000);
    tft->fillRect(0,0,20,26, BLACK);
    tft->setCursor(0,0);
    tft->print("1");
    delay(1000);
    tft->fillRect(0,0,20,26, BLACK);
  }
}

//clears display, prints starting screen, displays "PONG"
void displayBootScreen(Elegoo_TFTLCD * tft) {
  tft->reset();
  tft->begin(DRIVER_IDENTIFIER);
  tft->setRotation(2);  //rotates the screen 180 degrees
  tft->fillScreen(BLACK);
  
  tft->setCursor(tft->width()/2 - 50, tft->height() / 2 - 70);
  tft->setTextColor(WHITE);  tft->setTextSize(3);
  tft->print("DODGE!");
  tft->setTextSize(2);
  tft->setCursor(tft->width()/2 - 90, tft->height() / 2 - 40);
  tft->print("Select a level");
}

//prototype to use in the next function
int getNextLvl(Joystick*, Elegoo_TFTLCD*, int);
//sets up for next game depending on the level
void setUpForNextGame(Player* p, Joystick* j, Elegoo_TFTLCD * tft) {
  
  oVector.clear();   //remove all obstacles
  p->x = tft->width()/2; //set player to middle of screen
  p->y = tft->height()/2;
  randomSeed(analogRead(A13)); //new randoms

  gameOver = false;
  freqCounter = 0;

  //draw box and arrows for level select
  tft->drawRect(tft->width()/2 + 49, tft->height()/2 - 5, 16, 22, WHITE);
  tft->setTextSize(1);
  tft->setCursor(tft->width()/2 + 54, tft->height()/2 - 10);
  tft->print("^");
  tft->setCursor(tft->width()/2 + 54, tft->height()/2 + 17);
  tft->print("v");

  lvl = getNextLvl(j, tft, lvl);
  score = levels[lvl].duration;
  player->halfSize = levels[lvl].pHalfSize;
  
  tft->fillRect(0,0,20,26, BLACK);
  tft->setCursor(0,0);
  tft->setTextColor(WHITE);  tft->setTextSize(3);
  tft->print("3");
  delay(1000);
  tft->fillRect(0,0,20,26, BLACK);
  tft->setCursor(0,0);
  tft->print("2");
  delay(1000);
  tft->fillRect(0,0,20,26, BLACK);
  tft->setCursor(0,0);
  tft->print("1");
  delay(1000);
  tft->fillRect(0,0,20,26, BLACK);

  tft->fillScreen(levels[lvl].colors[0]);
  delay(750);
}

//allows the user to scroll up and down to pick a level number
int getNextLvl(Joystick* j, Elegoo_TFTLCD * tft, int lvlNum) {
  bool changed = true;
  tft->setTextSize(2);
  tft->setCursor(tft->width()/2 + 52, tft->height()/2 - 1);
  tft->print(lvlNum);
  delay(500);

  getJoystickStatus(j);
  while (j->buttonVal == 0) {
      getJoystickStatus(j);

      //move down
      if ((j->y > 1000) && (lvlNum >= 1)) {
        lvlNum--;
        changed = true;
      }
      else if ((j->y > 1000) && (lvlNum < 1)) {
        lvlNum = NUM_LEVELS - 1;
        changed = true;
      } 
      //or move up
      else if ((j->y < 10) && (lvlNum <= NUM_LEVELS - 2)) {
        lvlNum++;
        changed = true;
      }
      else if ((j->y < 10) && (lvlNum > NUM_LEVELS - 2)) {
        lvlNum = 0;
        changed = true;
      }
      else {
        changed = false;
      }
      if (changed) {
        tft->fillRect(tft->width()/2 + 50, tft->height()/2 - 4, 14, 20, BLACK);
        tft->setCursor(tft->width()/2 + 52, tft->height()/2 - 1);
        tft->print(lvlNum);
  
        delay(250);  
      }
      
  }
  return lvlNum;
}
