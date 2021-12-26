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

//clears display, prints starting screen, displays "PONG"
void displayStartScreen(Elegoo_TFTLCD * tft) {
  tft->reset();
  tft->begin(DRIVER_IDENTIFIER);
  tft->setRotation(2);  //rotates the screen 180 degrees
  tft->fillScreen(BLACK);
  
  tft->setCursor(tft->width()/2 - 60, tft->height() / 2 - 30);
  tft->setTextColor(WHITE);  tft->setTextSize(3);
  tft->print("DODGE!");
  tft->setTextSize(2);
  tft->setCursor(tft->width()/2 - 100, tft->height() / 2);
  tft->print("Press to start");
}

void setUpForNextGame(Player* p, Joystick* j, Elegoo_TFTLCD * tft) {
  oVector.clear();   //remove all obstacles
  p->x = tft->width()/2; //set player to middle of screen
  p->y = tft->height()/2;
  randomSeed(analogRead(A13)); //new randoms

  score = 0;
  gameOver = false;
  freqCounter = 0;

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

  tft->fillScreen(RED);
  delay(1000);
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
