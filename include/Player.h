#ifndef PLAYER_H
#define PLAYER_H

#include "Level.h"
#include <SFML/Audio.hpp>

extern const int DEFAULT_RES_X;
extern const int DEFAULT_RES_Y;

class Player
{
private:
	float playerSpeed;
	float jumpSpeed;
	float gravity;

	bool alive;
	bool onGround;

	sf::Vector2f velocity;
	sf::Vector2f scale;
	sf::Sprite model;

	Level* currentLevel;

	// A map pointer to the game sounds
	std::map<std::string, sf::Sound>* sounds;

	std::vector<sf::Texture> walkRight;
	std::vector<sf::Texture> walkLeft;

	// Inside clock for changing the animation
	sf::Clock animationTimer;

	// Amount of time between switching images
	sf::Time animationDelay;

	// Type of animation, for example walking left, jumping, etc.
	std::string animationType;

	// Index of the texture in the corresponding vector
	int animationNumber;

	// Set animationType and animationNumber to proper values,
	// based on the what the player's doing
	// Also play step sounds
	void AdjustAnimation();

	// Set the character's texture based on what animation it's in
	void SetTexture();

	// Return whether the player model would collide horizontally
	// with any of the platforms, or the walls in the next iteration
	bool HorizontalCollision(float xOffset);

	// Return whether the player model would collide vertically
	// with any of the platforms, or the walls in the next iteration
	bool VerticalCollision(float yOffset);

	// Add pulling forces by gravity boxes
	void AddGravityBoxPull();

public:
	// Default constructor
	Player() : playerSpeed(0), jumpSpeed(0), gravity(0) {}

	// Constructor
	Player(const sf::Vector2f& scale, Level* level, std::map<std::string, sf::Sound>* sounds);

	// Return the player's model as a sprite
	sf::Sprite GetModel() {return model;}

	// Set the level seen by the player to a given one
	void SetLevel(Level* nextLevel) {currentLevel = nextLevel;}

	// Place the player at a given position
	void Place(float x, float y) {model.setPosition(x * scale.x, y * scale.y);}

	// Place the player to the spawn position of the level
	void PlaceToSpawn();

	// These three functions only add some
	// vertical / horizontal velocity to the player
	// the actual movement happens in UpdatePosition
	void MoveLeft();
	void MoveRight();
	void Jump();

	// Update the player's position horizontally and vertically
	// The two axes are handled separately
	void UpdatePosition();

	// Return if the player is at the exit
	bool IsPlayerOnExit();

	// Return the value of gravity
	float GetGravity() {return gravity;}

	// Print information about the player
	friend std::ostream& operator<<(std::ostream &out, const Player player);
};

#endif
