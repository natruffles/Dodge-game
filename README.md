# Dodge-game
Obstacle dodging game created with an Arduino Mega, LCD TFT display, and a joystick.

v0.1 - Initial Commit 
* Initial hardware design created and game logic implemented (input, simulate, render frame-by-frame). 
* Player is a green square that can move around on the display in 8 directions. 
* Walls prevent the player from going off-screen. 
* Pause functionality included by pressing in the button on the joystick. The player has 3 seconds to get ready after unpausing. 
* No obstacles or collision detection added as of yet. 

v0.2 - Obstacles 
* Obstacle functionality added. 
* The user can customize the maximum number of obstacles on screen. 
* The obstacles have random size, speed, and spawn position. 
* Obstacles are removed from memory when they go off-screen. 

v0.3 - Collision Detection + Scoring 
* Collision detection added between all sides of every obstacle and all sides of the player. 
* Scoring system implemented, reflects seconds that player has survived. 
* Score is displayed in top left during gameplay and during the death screen. 
* Explosion effect displays upon death. 
* The user is given the option to replay the game at the death screen. 

v0.4 - Smoothness fix + Customizability 
* Now allows the user to decide the framerate for the player and obstacles (want obstacle framerate to be lower to avoid lag). 
* Obstacle position can be updated less frequently than player position to avoid stuttering. 
* Obstacles can now move in any direction rather than along one axis. 
* Accessibility: Countdown timer size has been increased.

v0.5 - Colors + Bugfix
* Color functionality added. 
* Silhouette - Player and obstacles are black, but both leave a multicolored trail, painting the screen many different random colors.
* This can be used to create level themes in the future. 
* Introduced an obstacle generation rate to prevent lag at the beginning of the game, obstacles generated every x frames. 
* Functions migrated from main .ino file to separate header files. Only "setup" and "loop" remain in main file.

v0.6 - Levels
* Level functionality added. 
* The user can select which level to play on both the startup screen and the game over screen. 
* Each level can have wildly different difficulties, as many aspects can be changed (such as player size, obstacle speed, number of obstacles, obstacle generation rate, etc.).
* Each level can also have its own color scheme.
* Many more levels will be added in the future.


