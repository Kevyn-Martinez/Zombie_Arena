#pragma once
#include "SFML/Graphics.hpp"

using namespace sf;

class Player {
private:
	const float START_SPEED = 200,
		        START_HEALTH = 100;

	//where is the player
	Vector2f m_Position;

	//of course we will need a sprite
	Sprite m_Sprite;

	//And texture, watch this space!!
	Texture m_Texture;

	//What is the screen resolution
	Vector2f m_Resolution;

	//What size is the current arena
	IntRect m_Arena;

	//How big is each tile of the arena
	int m_TileSize;

	//Which directions is the player currently moving in
	bool m_UpPressed,
		 m_DownPressed,
		 m_LeftPressed,
		 m_RightPressed;

	//How much health has the player got?
	int m_Health;

	//What is the maximumm health the player can have
	int m_MaxHealth;

	//When was the player last hit
	Time m_LastHit;

	//Speed in pixels per second
	float m_Speed;

	//All our public functions will come next

public:
	Player();
	
	void spawn(IntRect arena, Vector2f resolution, int tileSize);

	//Call this at the end of every game
	void resetPlayerStats();

	//Handle the player getting hit by a zombie
	bool hit(Time timeHit);

	//How long ago was the player last hit
	Time getLastHitTime();

	//Where is the player
	FloatRect getPosition();

	//Where is the center of the player
	Vector2f getCenter();

	//Which angle is the player facing
	float getRotation();

	//Send a copy of the sprite to main
	Sprite getSprite();

	//The next four functions move the player
	void moveLeft(),
		 moveRight(),
		 moveUp(),
		 moveDown();

	//Stop the player moving in a specific direction
	void stopLeft(),
		 stopRight(),
		 stopDown(),
		 stopUp();

	//We will call this function once every frame
	void update(float elapsedTime, Vector2i mousePosition);

	//Give player a speed boost
	void upgradeSpeed();

	//Give player some health
	void upgradeHealth();

	//Increase the maximum amount of health the player can have
	void increaseHealthLevel(int amount);

	//How much Health has the player currently got?
	int getHealth();
};