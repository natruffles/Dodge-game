//moves player in 8 directions with a speed 
void movePlayer(struct Player* p, struct Joystick* j, Elegoo_TFTLCD * tft) {
  p->priorX = p->x;
  p->priorY = p->y;
  
  //move left
  if (j->x > 1000) {
    (p->x) -= levels[lvl].pMovementSpeed/FRAME_RATE;
  }
  //or move right
  else if (j->x < 10) {
    (p->x) += levels[lvl].pMovementSpeed/FRAME_RATE;
  }

  //move down
  if (j->y > 1000) {
    (p->y) += levels[lvl].pMovementSpeed/FRAME_RATE;
  }
  //or move up
  else if (j->y < 10) {
    (p->y) -= levels[lvl].pMovementSpeed/FRAME_RATE;
  }

  //makes sure that the player is inbounds
  if ((p->x - p->halfSize) < 0) {
    p->x = p->halfSize;
  }
  else if ((p->x + p->halfSize) > tft->width()) {
    p->x = tft->width() - p->halfSize;
  }
  if ((p->y - p->halfSize) < 0) {
    p->y = p->halfSize;
  }
  else if ((p->y + p->halfSize) > tft->height()) {
    p->y = tft->height() - p->halfSize;
  }
}

//moves the boss around the screen based on player location
void moveBoss(Player* p, Elegoo_TFTLCD * tft) {
  static float xDistance = 0;
  static float yDistance = 0;
  
  boss.priorX = boss.x;
  boss.priorY = boss.y;

  xDistance = p->x - boss.x;
  yDistance = p->y - boss.y;

  boss.x += BOSS_DIFF/(float)FRAME_RATE * xDistance;
  boss.y += BOSS_DIFF/(float)FRAME_RATE * yDistance;
  
}

//generates an obstacle every so often with a random size and start position
void generateObstacles(Elegoo_TFTLCD * tft) {
  //prevents obstacles from being generated too fast
  static int genCounter = 0;
  
  //if there are less than the number of allowable obstacles, generate a new one 
  if ((oVector.size() < levels[lvl].maxObstacles) && (genCounter >= levels[lvl].obsGenRate)) {
    Obstacle obstacle = {0,0,0,0, //x,y,priorX,priorY positions
                       (float)random(levels[lvl].minObsSpeed/FRAME_RATE * 100.0, levels[lvl].maxObsSpeed/FRAME_RATE * 100.0) * 0.01, //movement speedX
                       0, //placeholder for movement speedY 
                       random(levels[lvl].minObsDim, levels[lvl].maxObsDim), //halfsizeX
                       random(levels[lvl].minObsDim, levels[lvl].maxObsDim), //halfsizeY
                       BLACK};

    int obsPosition = random(0, 4);  //determines which edge the obstacle will come from
    int diagonalVelocity = random((int)-obstacle.dX, (int)obstacle.dX + 1);
    
    //based on which edge, initial position (x,y) and velocity (dX, dY) are set
    switch (obsPosition) {
      case 0:
        //left side
        obstacle.x = 0 - obstacle.hsX;
        obstacle.y = random(obstacle.hsY * 2, tft->height() - obstacle.hsY * 2);
        obstacle.dY = diagonalVelocity;
        break;
      case 1:
        //top side
        obstacle.x = random(obstacle.hsX * 2, tft->width() - obstacle.hsX * 2);
        obstacle.y = 0 - obstacle.hsY;
        (obstacle.dY) = (obstacle.dX);
        obstacle.dX = diagonalVelocity;
        break;
      case 2:
        //right side 
        obstacle.x = tft->width() + obstacle.hsX;
        obstacle.y = random(obstacle.hsY * 2, tft->height() - obstacle.hsY * 2);
        (obstacle.dX) *= -1; //moves to the left
        obstacle.dY = diagonalVelocity;
        break;
      case 3:
        //bottom side
        obstacle.x = random(obstacle.hsX * 2, tft->width() - obstacle.hsX * 2);
        obstacle.y = tft->height() + obstacle.hsY;
        (obstacle.dY) = (obstacle.dX) * -1;
        obstacle.dX = diagonalVelocity;
        break;
    }
    obstacle.priorX = obstacle.x;
    obstacle.priorY = obstacle.y;

    oVector.push_back(obstacle);

    genCounter = 0;
  }
  else if ((oVector.size() < levels[lvl].maxObstacles) && (genCounter < levels[lvl].obsGenRate)) {
    genCounter++;
  }
}

//moves all obstacles based on their movement speed
void moveObstacles(Elegoo_TFTLCD * tft) {
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
      if ((oVector[j].priorX > tft->width() + oVector[j].hsX)
          || (oVector[j].priorX < (-1 * oVector[j].hsX))
          || (oVector[j].priorY > tft->height() + oVector[j].hsY)
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

//checks collision between player and any obstacle or the boss
bool collisionDetect(Player* p) {
  for (int i = 0; i < oVector.size(); i++) {
    if (rectVsRect(p, oVector[i])) {
      return true;
    }
  }
  //checks if boss collision
  if (p->x + p->halfSize > boss.x - boss.halfSize &&
    p->x - p->halfSize < boss.x + boss.halfSize &&
    p->y + p->halfSize > boss.y - boss.halfSize &&
    p->y - p->halfSize < boss.y + boss.halfSize) {
      return true;
    }
  return false;
}
