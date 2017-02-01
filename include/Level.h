#ifndef LEVEL_H
#define LEVEL_H

#include "LevelObject.h"

extern const int DEFAULT_RES_X;
extern const int DEFAULT_RES_Y;

class Level
{
private:
	static std::vector<sf::Texture> backgroundTextures;
	static std::vector<sf::Texture> textures;

	sf::Sprite background;
	std::vector<LevelObject> levelObjects;

	sf::Vector2f scale;
	sf::Vector2f spawn;

	sf::Clock animationTimer;
	sf::Time animationDelay;
	
	// Animate gravity boxes by changing their image
	void UpdateGravityBox();

public:
	// Load every texture required for the game
	static void LoadAllTextures();

	enum ObjectName
	{
		GROUNDSURFACE,
		GROUNDBLOCK,
		GRAVITYBOX,
		GRAVITYBOX_B,
		GRAVITYBOX_C,
		EXITDOOR
	};

	Level() {}

	// Level constructor
	// The scale of the game has to be provided
	Level(const sf::Vector2f &scale, float spawnX = 0, float spawnY = 0);

	// Return the sprite of the level background
	const sf::Sprite& GetBackground() {return background;}

	// Return all level objects in a vector
	std::vector<LevelObject> &GetObjects() {return levelObjects;}

	// Return the player's spawn position
	sf::Vector2f& GetSpawn() {return spawn;}

	// Set up a background for the level
	void SetBackground(int index);

	// Return the scale of a given type of level object
	float GetRescale(ObjectName name);

	// Initialize and place an object on the level
	// Helper function to create levels
	void PlaceObject(float left, float top, ObjectName name);

	// Fill a rectangle with the given type of objects
	void FillObject(float left, float top, float right, float bottom, ObjectName name);	

	// Move a level object if it's moveable and
	// not colliding with anything else
	// Return whether the movement happened
	bool MoveObject(int index, float offsetX, float offsetY);

	// Update every level object's position
	void UpdateObjects();
};

#endif
