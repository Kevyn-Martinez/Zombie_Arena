#include <iostream>
#include <sstream>
#include <fstream>
#include "SFML/Graphics.hpp"
#include "SFML/Audio.hpp"
#include "Player.h"
#include "ZombieArena.h"
#include "TextureHolder.h"
#include "Bullet.h"
#include "Pickup.h"

using namespace sf;


int main() {

    TextureHolder holder;
    //The game will always be in one of four states
    enum class State { PAUSED, LEVELING_UP, GAME_OVER, PLAYING };

    //Start with the game over state
    State state = State::GAME_OVER;

    //Get the screen resolution and create an SFML window
    Vector2f resolution;
    resolution.x = VideoMode::getDesktopMode().width;
    resolution.y = VideoMode::getDesktopMode().height;

    RenderWindow window(VideoMode(resolution.x, resolution.y), "Zombie Arena", Style::Fullscreen);

    //Create an SFML View for the main action
    View mainView(FloatRect(0, 0, resolution.x, resolution.y));

    //Here is our clock for timing everything
    Clock clock;
    
    //How long has the PLAYING state been active
    Time gameTimeTotal;

    //Where is the mouse in relation to the world coordinates
    Vector2f mouseWorldPosition;
    Vector2i mouseScreenPosition; // Where is the mouse in relation to the screen coordinates

    //Create instance of the player class
    Player player;

    //The boundaries of the arena
    IntRect arena;

    //Create background
    VertexArray background;
    
    //Load texture for the background vertex array
    Texture textureBackground = TextureHolder::GetTexture("graphics/background_sheet.png");

    int numZombies,
        numZombiesAlive;
    Zombie* zombies = nullptr;

    //100 bullets should do
    Bullet bullets[100];
    int currentBullet = 0,
        bulletsSpare  = 24,
        bulletsInClip = 6,
        clipSize      = 6;
    float fireRate    = 1;
    Time lastPressed; // When was the fire button last pressed?

    //Hide the mouse pointer and replace with crosshair
    window.setMouseCursorVisible(false);
    Sprite spriteCrosshair;
    Texture textureCrosshair = TextureHolder::GetTexture("graphics/crosshair.png");

    spriteCrosshair.setTexture(textureCrosshair);
    spriteCrosshair.setOrigin(25, 25);

    //Create a couple of pickups
    Pickup healthPickup(1);
    Pickup ammoPickup(2);

    //About the game
    int score     = 0,
        highScore = 0;

    //For the home/game over screen
    Sprite spriteGameOver;
    Texture textureGameOver = TextureHolder::GetTexture("graphics/background.png");
    spriteGameOver.setTexture(textureGameOver);
    spriteGameOver.setPosition(0, 0);

    //Create a view for the HUD
    View hudView(FloatRect(0, 0, resolution.x, resolution.y));

    //Create a sprite for the ammo icon
    Sprite spriteAmmoIcon;
    Texture textureAmmoIcon = TextureHolder::GetTexture("graphics/ammo_icon.png");
    spriteAmmoIcon.setTexture(textureAmmoIcon);
    spriteAmmoIcon.setPosition(28, 620);

    //Load the font
    Font font;
    font.loadFromFile("fonts/zombiecontrol.ttf");

    //Pause
    Text pausedText;
    pausedText.setFont(font);
    pausedText.setCharacterSize(85);
    pausedText.setFillColor(Color::White);
    pausedText.setPosition(350, 200);
    pausedText.setString("Press Enter \nto continue");

    //Game Over
    Text gameOverText;
    gameOverText.setFont(font);
    gameOverText.setCharacterSize(80);
    gameOverText.setFillColor(Color::White);
    gameOverText.setPosition(280, 540);
    gameOverText.setString("Press Enter to play");

    //Leveling up
    Text levelUpText;
    levelUpText.setFont(font);
    levelUpText.setCharacterSize(60);
    levelUpText.setFillColor(Color::White);
    levelUpText.setPosition(80, 150);
    std::stringstream levelUpStream;
    levelUpStream << "1- Increased fire rate"
                  << "\n2- Increased clip sized(next reload)"
                  << "\n3- Increased max health"
                  << "\n4- Increased run speed"
                  << "\n5- More and better health pickups"
                  << "\n6- More and better ammo pickups";
    levelUpText.setString(levelUpStream.str());

    //Ammo
    Text ammoText;
    ammoText.setFont(font);
    ammoText.setCharacterSize(50);
    ammoText.setFillColor(Color::White);
    ammoText.setPosition(116, 620);

    //Score
    Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(50);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(20, 0);

    //load the high score from a tex file
    std::ifstream inputFile("gamedata/scores.txt");
    if (inputFile.is_open()) {
        inputFile >> highScore;
        inputFile.close();
    }

    //High Score
    Text highScoreText;
    highScoreText.setFont(font);
    highScoreText.setCharacterSize(50);
    highScoreText.setFillColor(Color::White);
    highScoreText.setPosition(980, 0);
    std::stringstream s;
    s << "High Score: " << highScore;
    highScoreText.setString(s.str());

    //Zombies remaining
    Text zombiesRemainingText;
    zombiesRemainingText.setFont(font);
    zombiesRemainingText.setCharacterSize(50);
    zombiesRemainingText.setFillColor(Color::White);
    zombiesRemainingText.setPosition(975, 620);
    zombiesRemainingText.setString("Zombies: 100");

    //Wave Number
    int wave = 0;
    Text waveNumberText;
    waveNumberText.setFont(font);
    waveNumberText.setCharacterSize(50);
    waveNumberText.setFillColor(Color::White);
    waveNumberText.setPosition(750, 620);
    waveNumberText.setString("Wave: 0");

    //Health bar
    RectangleShape healthBar;
    healthBar.setFillColor(Color::Red);
    healthBar.setPosition(300, 620);

    //When did we last update the HUD?
    int framesSinceLastHUDUpdate = 0;

    //How Often (in frames) should we update the hud
    int fpsMeasurementFrameInterval = 1000;

    //Prepare the hit sound
    SoundBuffer hitBuffer;
    hitBuffer.loadFromFile("sound/hit.wav");
    Sound hit;
    hit.setBuffer(hitBuffer);

    //Prepare the splot sound
    SoundBuffer splatBufffer;
    splatBufffer.loadFromFile("sound/splat.wave");
    Sound splat;
    splat.setBuffer(splatBufffer);

    //Prepare the shoot sound
    SoundBuffer shootBuffer;
    shootBuffer.loadFromFile("sound/shoot.wav");
    Sound shoot;
    shoot.setBuffer(shootBuffer);

    //Prepare the reload sound
    SoundBuffer reloadBuffer;
    reloadBuffer.loadFromFile("sound/reload.wav");
    Sound reload;
    reload.setBuffer(reloadBuffer);

    //Prepare the failed sound
    SoundBuffer reloadFailedBuffer;
    reloadFailedBuffer.loadFromFile("sound/reload_failed.wav");
    Sound reloadFailed;
    reloadFailed.setBuffer(reloadFailedBuffer);

    //Prepare the powerup sound
    SoundBuffer powerupBuffer;
    powerupBuffer.loadFromFile("sound/powerup.wav");
    Sound powerup;
    powerup.setBuffer(powerupBuffer);

    //Prepare the pickup sound
    SoundBuffer pickupBuffer;
    pickupBuffer.loadFromFile("sound/pickup.wav");
    Sound pickup;
    pickup.setBuffer(pickupBuffer);

    //The game main loop
    while (window.isOpen()) {

        // Handle Input

        //Handle events
        Event event;
        while (window.pollEvent(event)) {
            
            if (event.type == Event::KeyPressed) {
                //Pause the game while playing
                if (event.key.code == Keyboard::Return && state == State::PLAYING) {
                    state = State::PAUSED;
                }
                else if (event.key.code == Keyboard::Return && state == State::PAUSED) { //Restart while paused
                    state = State::PLAYING;
                    clock.restart(); //Reset the clock so there isnt a frame jump!
                }
                else if (event.key.code == Keyboard::Return && state == State::GAME_OVER) { //Start a new game while in game over state
                    state = State::LEVELING_UP;
                    wave = 0;
                    score = 0;

                    //Prepare the gun and ammo for next game
                    currentBullet = 0;
                    bulletsSpare  = 24;
                    bulletsInClip = 6;
                    clipSize      = 6;
                    fireRate      = 1;

                    //Reset the player's stats
                    player.resetPlayerStats();
                }
                if (state == State::PLAYING){

                    //Reloading
                    if (event.key.code == ::Keyboard::R) {
                        if (bulletsSpare >= clipSize) {

                            //Plenty of bullets . Reload .
                            bulletsInClip = clipSize;
                            bulletsSpare -= clipSize;
                            reload.play();
                        }
                        else if (bulletsSpare > 0) {
                            //only few bullets lef
                            bulletsInClip = bulletsSpare;
                            bulletsSpare = 0;
                            reload.play();
                        }
                        else {
                            //More here soon?!
                            reloadFailed.play();
                        }
                    }
                
                }
            }
        } // End event polling

        //Handle player quitting
        if (Keyboard::isKeyPressed(Keyboard::Escape)) {
            window.close();
        }

        //Handle controls while playing
        if (state == State::PLAYING) {

            //Handle the pressing and releasing of the WASD keys
            if (Keyboard::isKeyPressed(Keyboard::W)) {
                player.moveUp();
            }
            else {
                player.stopUp();
            }

            if(Keyboard::isKeyPressed(Keyboard::S)){
                player.moveDown();
            }
            else {
                player.stopDown();
            }
            if (Keyboard::isKeyPressed(Keyboard::A)){
                player.moveLeft();
            }
            else {
                player.stopLeft();
            }
            if (Keyboard::isKeyPressed(Keyboard::D)){
                player.moveRight();
            }
            else {
                player.stopRight();
            }

            //Fire a bullet
            if (Mouse::isButtonPressed(Mouse::Left)) {
                if (gameTimeTotal.asMilliseconds() - lastPressed.asMilliseconds() > 1000 / fireRate && bulletsInClip > 0) {

                    //Pass he center of the player and the center of the cross-hair to the shoot function
                    bullets[currentBullet].shoot(player.getCenter().x, player.getCenter().y, mouseWorldPosition.x, mouseWorldPosition.y);

                    currentBullet++;
                    if (currentBullet > 99) {
                        currentBullet = 0;
                    }
                    lastPressed = gameTimeTotal;

                    shoot.play();

                    bulletsInClip--;
                }
            } //End of Bullet firing

        }//End WASD handling while playing

        //Hande te player levelling up state
        if (state == State::LEVELING_UP) {
            if (event.key.code == Keyboard::Num1) {

                //Increase fire rate
                fireRate++;

                state = State::PLAYING;
            }

            if (event.key.code == Keyboard::Num2) {
                
                //Increase clip  size
                clipSize += clipSize;

                state = State::PLAYING;
            }

            if (event.key.code == Keyboard::Num3) {

                //Increase health
                player.upgradeHealth();

                state = State::PLAYING;
            }

            if (event.key.code == Keyboard::Num4) {

                //Increase speed
                player.upgradeSpeed();
                state = State::PLAYING;
            }

            if (event.key.code == Keyboard::Num5) {
                
                //Upgrade Health pickup
                healthPickup.upgrade();
                state = State::PLAYING;
            }

            if (event.key.code == Keyboard::Num6) {

                //Upgrade Ammo pickup
                ammoPickup.upgrade();
                state = State::PLAYING;
            }

            if (state == State::PLAYING) {

                //Increase the number of wave
                wave++;

                //Prepare the level we will modify the next 2 lines later
                arena.width = 500 * wave;
                arena.height = 500 * wave;
                arena.left = 0;
                arena.top = 0;


                //Pass the vertex array by reference to the createBackground function
                int tileSize = createBackground(background, arena);

                //Spawn the player in the middle of the arena
                player.spawn(arena, resolution, tileSize);

                //Configure the pickups
                healthPickup.setArena(arena);
                ammoPickup.setArena(arena);

                //Create a horde of zomibes
                numZombies = 5 *  wave;

                //Delete the previously allocated memory (if it exists)
                delete[] zombies;
                zombies = createHorde(numZombies, arena);
                numZombiesAlive = numZombies;

                //Play the powerup sound
                powerup.play();

                //Reset the clock so there isn't a frame jump
                clock.restart();
            }

        } //End Leveling state


        //UPDATE THE FRAME
        if (state == State::PLAYING) {

            //Update the delta time
            Time dt = clock.restart();
            gameTimeTotal += dt; //Update the total game time
            float dtAsSeconds = dt.asSeconds(); //Make a decimal fraction of 1 from the delta time

            //Where is the mouse pointer
            mouseScreenPosition = Mouse::getPosition();

            //Convert mouse position to world coordinates of mainView
            mouseWorldPosition = window.mapPixelToCoords(Mouse::getPosition(), mainView);

            //Set the crosshair to the mouse world location
            spriteCrosshair.setPosition(mouseWorldPosition);

            //Update the player
            player.update(dtAsSeconds, Mouse::getPosition());

            //Make a note of the players new position
            Vector2f playerPosition(player.getCenter());

            //Make the view center around the player
            mainView.setCenter(player.getCenter());

            //Loop through each Zombie and update them
            for (int i = 0; i < numZombies; i++) {
                if (zombies[i].isAlive()) {
                    zombies[i].update(dt.asSeconds(), playerPosition);
                }
            } //end of zombie for loop

            //Update any bullets that are in flight
            for (int i = 0; i < 100; i++) {
                if (bullets[i].isInFlight()) {
                    bullets[i].update(dtAsSeconds);
                } // end of if
            } //end of bullet for loop

            //Update the pickups
            healthPickup.update(dtAsSeconds);
            ammoPickup.update(dtAsSeconds);

            //Collision detection Have any zombies been shot?
            for (int i = 0; i < 100; i++) {
                for (int j = 0; j < numZombies; j++) {

                    if (bullets[i].isInFlight() && zombies[j].isAlive()) {

                        if (bullets[i].getPosition().intersects(zombies[j].getPosition())){

                            //Stop the bullet
                            bullets[i].stop();

                            //Register the hit and see if it was a kill
                            if (zombies[j].hit()) {
                                //Not just a hit but a kill too
                                score += 10;
                                if (score >= highScore) {
                                    highScore = score;
                                }
                                
                                numZombiesAlive--;

                                //When all the zombies are dead (again)
                                if (numZombiesAlive == 0) {
                                    state = State::LEVELING_UP;
                                }
                            }

                            //Play a sound
                            splat.play();
                        }
                    }
                }
            } //End of zombie being shot

            //Have any zombies touched the player
            for (int i = 0; i < numZombies; i++) {

                if (player.getPosition().intersects(zombies[i].getPosition()) && zombies[i].isAlive()) {

                    if (player.hit(gameTimeTotal)) {
                        //more later
                        hit.play();
                    }

                    if (player.getHealth() <= 0) {
                        state = State::GAME_OVER;
                        std::ofstream outputFile("gamedata/scores.txt");
                        outputFile << highScore;
                        outputFile.close();
                    }
                }
            } // End of player being hit by zombie

            //Has the player touched a health pickup
            if (player.getPosition().intersects(healthPickup.getPosition()) && healthPickup.isSpawned()) {
                player.increaseHealthLevel(healthPickup.gotIt());
                
                //Play sound
                pickup.play();
            }

            //Has the playertouched ammo pickup
            if (player.getPosition().intersects(ammoPickup.getPosition()) && ammoPickup.isSpawned()) {
                bulletsSpare += ammoPickup.gotIt();

                //Play reload sound effect
                reload.play();
            }

            //Size up the health bar
            healthBar.setSize(Vector2f(player.getHealth() * 3, 70));

            //Increment the number of frames since the lat HUD calculation
            framesSinceLastHUDUpdate++;

            //Calculate FPS every fpsMeasurementFrameInterval frames
            if (framesSinceLastHUDUpdate > fpsMeasurementFrameInterval) {

                //Update game HUD text
                std::stringstream ssAmmo,
                                  ssScore,
                                  ssHighScore,
                                  ssWave,
                                  ssZombiesAlive;

                //Update the ammo text
                ssAmmo << bulletsInClip << "/" << bulletsSpare;
                ammoText.setString(ssAmmo.str());

                //Update the score text
                ssScore << "Score: " << score;
                scoreText.setString(ssScore.str());

                //Update the high score text
                ssHighScore << "High Score: " << highScore;
                highScoreText.setString(ssHighScore.str());

                //Update the wave
                ssWave << "Wave: " << wave;
                waveNumberText.setString(ssWave.str());

                //Update the Number of Zumbies alive text
                ssZombiesAlive << "Zombies: " << numZombiesAlive;
                zombiesRemainingText.setString(ssZombiesAlive.str());

                framesSinceLastHUDUpdate = 0;
            } // End of HUD update

        } // end of Update Frame IF statement

        //Draw the Scene

        if (state == State::PLAYING) {
            window.clear();

            //Set the mainView to be Displayed in the window and draw everything related to it
            window.setView(mainView);

            //Draw the background
            window.draw(background, &textureBackground);

            //Draw the zomibes
            for (int i = 0; i < numZombies; i++) {
                window.draw(zombies[i].getSprite());
            }
            
            //Draw the bullets being shot
            for (int i = 0; i < 100; i++) {
                if (bullets[i].isInFlight()) {
                    window.draw(bullets[i].getShape());
                }
            }

            //Draw the player
            window.draw(player.getSprite());

            //drwa the pickups if curently spawn
            if (ammoPickup.isSpawned()) {
                window.draw(ammoPickup.getSprite());
            }
            if (healthPickup.isSpawned()) {
                window.draw(healthPickup.getSprite());
            }

            //Draw the crosshair
            window.draw(spriteCrosshair);

            //Switch to the HUD view
            window.setView(hudView);

            //Draw all the HUD elements
            window.draw(spriteAmmoIcon);
            window.draw(ammoText);
            window.draw(scoreText);
            window.draw(highScoreText);
            window.draw(healthBar);
            window.draw(waveNumberText);
            window.draw(zombiesRemainingText);
        }

        if (state == State::LEVELING_UP) {
            
            window.draw(spriteGameOver);
            window.draw(levelUpText);
        }
        if (state == State::GAME_OVER) {
            window.draw(spriteGameOver);
            window.draw(gameOverText);
            window.draw(scoreText);
            window.draw(highScoreText);
        }
        if (state == State::PAUSED) {

            window.draw(pausedText);
        }

        window.display();
         
    } // end of while loop OR END GAME LOOP

    delete[] zombies; // DELTE the previously allocated memory if existst
    return 0;
}