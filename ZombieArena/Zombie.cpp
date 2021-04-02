#include "Zombie.h"
#include "TextureHolder.h"
#include "cstdlib"
#include "ctime"

using namespace std;

void Zombie::spawn(float startX, float startY, int type, int seed) {

	switch (type) {
	case 0: //Bloater
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/bloader.png"));

		m_Speed = 40;
		m_Health = 5;
		break;

	case 1: //Chaser
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/chaser.png"));

		m_Speed = 70;
		m_Health = 1;
		break;

	case 2: //Crawler
		m_Sprite = Sprite(TextureHolder::GetTexture("graphics/crawler.png"));

		m_Speed = 20;
		m_Health = 3;
		break;
	}

	//Modify the speed to make the zombie unique every zombie is unique. Create a speed modifier
	srand((int)time(0) * seed);
	float modifier = (rand() % MAX_VARRIANCE) + OFFSET; // Somewhere between 80 and 100

	//Express as a fraction of 1
	modifier /= 100; // Now equals between 0.7 and 1
	m_Speed *= modifier;

	//Initialize its location
	m_Position.x = startX;
	m_Position.y = startY;

	//Set its origin to its center
	m_Sprite.setOrigin(25, 25);

	//Set its position
	m_Sprite.setPosition(m_Position);
}


bool Zombie::hit() {

	m_Health--;
	if (m_Health <= 0) {
		m_Alive = false;
		m_Sprite.setTexture(TextureHolder::GetTexture("graphics/blood.png"));

		return true;
	}

	//injured but not dead yet
	return false;
}

bool Zombie::isAlive() {
	return m_Alive;
}

FloatRect Zombie::getPosition() {
	return m_Sprite.getGlobalBounds();
}

Sprite Zombie::getSprite() {
	return m_Sprite;
}

void Zombie::update(float elapsedTime, Vector2f playerLocation) {
	
	float playerX = playerLocation.x,
		  playerY = playerLocation.y;

	//Update the zombie position variables
	if (playerX > m_Position.x) {
		m_Position.x = m_Position.x + m_Speed * elapsedTime;
	}

	if (playerY > m_Position.y) {
		m_Position.y = m_Position.y + m_Speed * elapsedTime;
	}

	if (playerX < m_Position.x) {
		m_Position.x = m_Position.x - m_Speed * elapsedTime;
	}

	if (playerY < m_Position.y) {
		m_Position.y = m_Position.y - m_Speed * elapsedTime;
	}

	//Move the sprite
	m_Sprite.setPosition(m_Position);

	//Face the sprite in the correction direction
	float angle = (atan2(playerY - m_Position.y, playerX - m_Position.x) * 180) / 3.141;

	m_Sprite.setRotation(angle);
}