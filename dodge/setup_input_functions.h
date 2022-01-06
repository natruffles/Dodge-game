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

//prototypes to use in the next function
//void getNextLvl(Joystick*, Elegoo_TFTLCD*);
//Level setCustomLvl(Joystick* , Elegoo_TFTLCD*);

//allows the user to scroll up and down to pick a level number
void getNextLvl(Joystick* j, Elegoo_TFTLCD * tft) {
  bool changed = true;
  tft->setTextSize(2);
  tft->setCursor(lvlSelect.xPos, lvlSelect.yPos);
  tft->print((int)lvlSelect.value);
  delay(500);

  getJoystickStatus(j);
  while (j->buttonVal == 0) {
    getJoystickStatus(j);

    //move down
    if ((j->y > 1000) && ((int)lvlSelect.value > lvlSelect.minValue)) {
      lvlSelect.value -= lvlSelect.changeInValue;
      changed = true;
    }
    else if ((j->y > 1000) && ((int)lvlSelect.value <= lvlSelect.minValue)) {
      lvlSelect.value = lvlSelect.maxValue;
      changed = true;
    } 
    //or move up
    else if ((j->y < 10) && ((int)lvlSelect.value < lvlSelect.maxValue)) {
      lvlSelect.value += lvlSelect.changeInValue;
      changed = true;
    }
    else if ((j->y < 10) && ((int)lvlSelect.value >= lvlSelect.maxValue)) {
      lvlSelect.value = lvlSelect.minValue;
      changed = true;
    }
    else {
      changed = false;
    }
    if (changed) {
      tft->fillRect(lvlSelect.xPos - 2, lvlSelect.yPos - 3, 14, 20, BLACK);
      tft->setCursor(lvlSelect.xPos, lvlSelect.yPos);
      tft->print((int)lvlSelect.value);

      delay(lvlSelect.scrollDelay);  
    }  
  }
  lvl = (int)lvlSelect.value;
}

//allows user to set custom properties for custom level
Level setCustomLvl(Joystick* j, Elegoo_TFTLCD * tft) {
  //set all the values on screen to what was last saved by the custom level
  customLevelScreen[0].value = levels[0].pHalfSize;
  customLevelScreen[1].value = levels[0].maxObstacles;
  customLevelScreen[2].value = levels[0].obsGenRate;
  customLevelScreen[3].value = levels[0].minObsDim;
  customLevelScreen[4].value = levels[0].maxObsDim;
  customLevelScreen[5].value = levels[0].duration;
  customLevelScreen[6].value = levels[0].pMovementSpeed;
  customLevelScreen[7].value = levels[0].minObsSpeed;
  customLevelScreen[8].value = levels[0].maxObsSpeed;

  //display everything
  tft->fillScreen(BLACK);
  tft->setTextSize(2); tft->setTextColor(WHITE);
  tft->setCursor(0,0); tft->print("Enter custom values"); 
  tft->setCursor(0, 31); tft->print("Player size:");
  tft->setCursor(195, 31); tft->print((int)customLevelScreen[0].value);
  tft->setCursor(0, 56); tft->print("Max # obstacles:");
  tft->setCursor(195, 56); tft->print((int)customLevelScreen[1].value);
  tft->setCursor(0, 81); tft->print("Obs gen time:");
  tft->setCursor(195, 81); tft->print((int)customLevelScreen[2].value);
  tft->setCursor(0, 106); tft->print("Min obs length:");
  tft->setCursor(195, 106); tft->print((int)customLevelScreen[3].value);
  tft->setCursor(0, 131); tft->print("Max obs length:"); 
  tft->setCursor(195, 131); tft->print((int)customLevelScreen[4].value);
  tft->setCursor(0, 156); tft->print("Lvl duration:");
  tft->setCursor(195, 156); tft->print((int)customLevelScreen[5].value);
  tft->setCursor(0, 181); tft->print("Player speed:");
  tft->setCursor(195, 181); tft->print((int)customLevelScreen[6].value);
  tft->setCursor(0, 206); tft->print("Min obs speed:");
  tft->setCursor(195, 206); tft->print((int)customLevelScreen[7].value);
  tft->setCursor(0, 231); tft->print("Max obs speed:");
  tft->setCursor(195, 231); tft->print((int)customLevelScreen[8].value);
  tft->setCursor(12, 285); tft->print("Click to continue!");

  int currentVertPos = 0; //current position on level select
  int priorVertPos = 0; //prior position on level select
  int currentHorizPos = (int)customLevelScreen[currentVertPos].value;
  bool vertChanged = false; //so display knows to change value
  bool horizChanged = false; //so display knows to change value

  //display the white indicator next to currentVertPos
  tft->fillRect(customLevelScreen[currentVertPos].xPos-4, 
                customLevelScreen[currentVertPos].yPos-2
                , 1, 18, WHITE);

  getJoystickStatus(j);
  //while the user doesnt press the joystick button
  while (j->buttonVal == 0) {
    //if scrolling down
    if (j->y > 1000) {
      vertChanged = true;
      priorVertPos = currentVertPos;
      if (currentVertPos < NUM_CUSTOM_NUMS - 1) {
        currentVertPos++; //current location moves down
      }
      else {
        currentVertPos = 0; //current location goes back to the top
      }
    }
    
    //if scrolling up
    else if (j->y < 10) {
      vertChanged = true;
      priorVertPos = currentVertPos;
      if (currentVertPos > 0) {
        currentVertPos--; //current location moves up
      }
      else {
        currentVertPos = NUM_CUSTOM_NUMS - 1; //current location goes down to the bottom
      }
    }
    
    //if scrolling left
    else if (j->x > 1000) {
      horizChanged = true;
      if (currentHorizPos > customLevelScreen[currentVertPos].minValue) {
        currentHorizPos -= customLevelScreen[currentVertPos].changeInValue;
      }
      else {
        currentHorizPos = customLevelScreen[currentVertPos].maxValue;
      }
    }
    
    //if scrolling right
    else if (j->x < 10) {
      horizChanged = true;
      if (currentHorizPos < customLevelScreen[currentVertPos].maxValue) {
        currentHorizPos += customLevelScreen[currentVertPos].changeInValue;
      }
      else {
        currentHorizPos = customLevelScreen[currentVertPos].minValue;
      }
    }

    //if vertical position changed
    if (vertChanged) {
      //store priorPos in correct custom number
      customLevelScreen[priorVertPos].value = currentHorizPos;

      //get rid of old indicator next to priorVertPos
      tft->fillRect(customLevelScreen[priorVertPos].xPos-4, 
                customLevelScreen[priorVertPos].yPos-2
                , 1, 18, BLACK);
      
      //draw new indicator next to current vertical position
      tft->fillRect(customLevelScreen[currentVertPos].xPos-4, 
                customLevelScreen[currentVertPos].yPos-2
                , 1, 18, WHITE);

      //set the current horizontal position to the value of the new vertical position
      currentHorizPos = (int)customLevelScreen[currentVertPos].value;

      vertChanged = false;
      delay(250); //vertical scrolling speed
    }

    //if horizontal position changed
    if (horizChanged) {
      //refresh that part of the screen with black
      tft->fillRect(customLevelScreen[currentVertPos].xPos,
                     customLevelScreen[currentVertPos].yPos,
                     40, 14, BLACK); 
      
      //draw new value
      tft->setCursor(customLevelScreen[currentVertPos].xPos,
                     customLevelScreen[currentVertPos].yPos);
      tft->print(currentHorizPos);

      horizChanged = false;
      delay(customLevelScreen[currentVertPos].scrollDelay);
    } 

    getJoystickStatus(j);
  }

  //when button is clicked, makes sure that the current value that is being edited is saved
  customLevelScreen[currentVertPos].value = currentHorizPos;

  //return a level structure using the current values from custom level screen
  return {levels[0].levelNum, 
  customLevelScreen[0].value,
  customLevelScreen[1].value,
  customLevelScreen[2].value,
  customLevelScreen[3].value,
  customLevelScreen[4].value,
  customLevelScreen[5].value,
  customLevelScreen[6].value,
  customLevelScreen[7].value,
  customLevelScreen[8].value, false,0,0,0,
  {levels[0].colors[0], levels[0].colors[1], 
  levels[0].colors[2], levels[0].colors[3]}
  };
}

//generates orbitals in the correct position relative to the boss based on how many there are
void generateOrbiters(Elegoo_TFTLCD * tft) {
  boss.orbiters[0].angularPosition = 0;
  if ((levels[lvl].numOrbiters == 2) || (levels[lvl].numOrbiters == 4)) {
    boss.orbiters[1].angularPosition = PI;
  }
  if (levels[lvl].numOrbiters == 4) {
    boss.orbiters[2].angularPosition = PI/2;
    boss.orbiters[3].angularPosition = 3 * PI/2;
  }
  else if (levels[lvl].numOrbiters == 3) {
    boss.orbiters[1].angularPosition = 2 * PI/3;
    boss.orbiters[2].angularPosition = 4 * PI/3;
  }
}

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

  getNextLvl(j, tft);
  if (lvl == 0) {
    levels[0] = setCustomLvl(j, tft);  //level 0 is the custom level
  }
  
  score = levels[lvl].duration;
  player->halfSize = levels[lvl].pHalfSize;
  if (levels[lvl].hasBoss) {
    //resets the position of boss and orbiters
    boss.x = 0; boss.y = 0;
    for (int i = 0; i < MAX_ORBITERS; i++) {
      boss.orbiters[i].x = boss.orbiters[i].priorX = boss.x;
      boss.orbiters[i].y = boss.orbiters[i].priorY = boss.y;
    }
    //if there are orbitals, generate their initial positions 
    if (levels[lvl].numOrbiters > 0) {
      generateOrbiters(tft);
    }
    
  } else {
    boss.x = -3* levels[lvl].bossHalfSize;
    boss.y = -3* levels[lvl].bossHalfSize; //move boss off the screen
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

  //will get overwritten once score is displayed
  tft->fillScreen(levels[lvl].colors[0]);
  tft->setCursor(0,0); tft->setTextColor(BLACK);
  tft->setTextSize(1);
  tft->print("Load");
  tft->setTextColor(WHITE);
}
