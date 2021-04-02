#pragma once
#include "SFML\Graphics.hpp"

using namespace sf;

class Bullet {
private:

	//Where is the bullet
	Vector2f m_Position;

	//What each bullet looks like
	RectangleShape m_BulletShape;

	//Is this bullet currently whizzing through the air
	bool m_InFlight = false;

	//How fast does a bullet travel?
	float m_BulletSpeed = 1000;

	//What fraction of 1 pixel does the bullet travel, horizontally and vertically each frame? these values will derived from m_BulletSpeed
	float m_BulletDistanceX,
		m_BulletDistanceY;

	//Some boundaries so the bullet doesnt fly forever
	float m_MaxX,
		  m_MinX,
		  m_MaxY,
		  m_MinY;

	//Public function prototypes go here
public:

	//The Constructor
	Bullet();

	//Stop the bullet
	void stop();

	//Returns the value of m_InFlight
	bool isInFlight();

	//Launch a new bullet
	void shoot(float startX, float startY, float xTarget, float yTarget);

	//Return the calling code where the bullet is in the world
	FloatRect getPosition();

	//RReturn the sahpe of the bullet
	RectangleShape getShape();

	//Update the bullet each frame
	void update(float elapsedTime);
};