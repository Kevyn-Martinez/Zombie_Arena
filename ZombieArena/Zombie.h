#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

class Zombie {
private:
	
	//How fast is each zombie type?
	const float BLOATER_SPEED = 40,
				CHASER_SPEED =  80,
				CRAWLER_SPEED = 20;
	
	//How tough is each zombie type
	const float BLOATER_HEALTH = 5,
				CHASER_HEALTH  = 1,
				CRAWLER_HEALTH = 3;

	//Make each zombie vary its speed slightly
	const int MAX_VARRIANCE = 30,
			  OFFSET        = 101 - MAX_VARRIANCE;

	//Where is this zombie?
	Vector2f m_Position;

	//A sprite for the zombie
	Sprite m_Sprite;

	//How fast can this one run/crawl?
	float m_Speed;

	//How much health has it got?
	float m_Health;

	//Is it still alive?
	bool m_Alive;

	//Public Prototypes go here
public:

	//Handle when a bullet hits a zomibe
	bool hit();

	//Find out if the zombie is alive
	bool isAlive();

	//Spawn a new Zombie
	void spawn(float startX, float startY, int type, int seed);

	//Return a rectangle that is the position in the world
	FloatRect getPosition();

	//Get a copy of the sprite to draw
	Sprite getSprite();

	//Update the zombie each frame
	void update(float elapsedTime, Vector2f playerLocation);
};