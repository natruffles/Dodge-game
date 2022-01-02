//displays the player, a small square with its center at (x,y)
void displayPlayer(struct Player* p, Elegoo_TFTLCD * tft) {
  if (p->x != p->priorX || p->y != p->priorY) {
    tft->fillRect(p->priorX - p->halfSize, p->priorY - p->halfSize, 
    p->halfSize*2, p->halfSize*2, levels[lvl].colors[random(0,NUM_COLORS)]);
  }
  
  tft->fillRect(p->x - p->halfSize, p->y - p->halfSize, 
    p->halfSize*2, p->halfSize*2, p->color);
}

//displays the boss, a small square with its center at (x,y)
void displayBoss(Elegoo_TFTLCD * tft) {
  tft->fillRect(boss.priorX - boss.halfSize, boss.priorY - boss.halfSize, 
  boss.halfSize*2, boss.halfSize*2, levels[lvl].colors[random(0,NUM_COLORS)]);
  
  tft->fillRect(boss.x - boss.halfSize, boss.y - boss.halfSize, 
    boss.halfSize*2, boss.halfSize*2, player->color);

  //make the face
  tft->fillRect(boss.x - boss.halfSize + 2, boss.y - boss.halfSize + 2, 
    2,2, WHITE);
  tft->fillRect(boss.x + boss.halfSize - 4, boss.y - boss.halfSize + 2, 
    2,2, WHITE);
  tft->fillRect(boss.x - boss.halfSize + 2, boss.y + boss.halfSize - 4, 
    2 * boss.halfSize - 4 ,2, WHITE); 
}

//makes the game run at a certain frame-rate by setting a timer buffer
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
void displayObstacles(Elegoo_TFTLCD * tft) {
  if (freqCounter == 0) {
    for (int i = 0; i < oVector.size(); i++) {
      tft->fillRect((int)(oVector[i].priorX - oVector[i].hsX), 
      (int)(oVector[i].priorY - oVector[i].hsY),  
      oVector[i].hsX * 2, 
      oVector[i].hsY * 2, 
      levels[lvl].colors[random(0,NUM_COLORS)]);
  
      tft->fillRect(oVector[i].x - oVector[i].hsX, 
      oVector[i].y - oVector[i].hsY,  
      oVector[i].hsX * 2, 
      oVector[i].hsY * 2, 
      oVector[i].color);
    }
  }
}

//displays player death animation
void deathAnimation(Player* p, Elegoo_TFTLCD * tft) {
  for (int i = 0; i < 5; i++) {
    for (int radius = 5; radius <= 15; radius += 2) {
      tft->fillCircle(p->x, p->y, radius, RED);
      delay(MILLIS_DELAY);
      tft->fillCircle(p->x, p->y, radius, WHITE);
    }
  }
}

//displays the score every frame
void displayScore(float playerScore, Elegoo_TFTLCD * tft) {
  tft->fillRect(0,0,24,8,BLACK);
  tft->drawRect(-1,-1,26,10, WHITE);
  tft->setCursor(0,0);
  tft->setTextSize(1);
  tft->setTextColor(WHITE);
  tft->print((int)playerScore);
}

//when player loses, ask them to play again
void displayEndScreen(float playerScore, Elegoo_TFTLCD * tft) {
  tft->fillRect(tft->width()/2-102, tft->height()/2-60, 200, 86, BLACK);
  tft->setCursor(tft->width()/2-100, tft->height()/2-50);
  tft->setTextColor(WHITE);
  tft->setTextSize(2);
  tft->print("Seconds left: ");
  tft->print((int)playerScore);
  
  if (score <= 0.0) {
    tft->setCursor(tft->width()/2-75, tft->height()/2);
    tft->print("You win!");
  }
  else {
    tft->setCursor(tft->width()/2-100, tft->height()/2);
    tft->print("Play again?");
  }
}
