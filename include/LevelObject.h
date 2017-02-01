#ifndef LEVEL_OBJECT_H
#define LEVEL_OBJECT_H

#include <string>
#include <vector>
#include <SFML\Graphics.hpp>

extern const int DEFAULT_RES_X;
extern const int DEFAULT_RES_Y;

class LevelObject
{
private:
	std::string name;
	std::vector<sf::Texture*> textures;
	int currentTexture;
	int step;
	float gravity;

	sf::RectangleShape glow;
	sf::Sprite model;
	sf::Vector2f velocity;

	bool moveable;
	bool solid;
	float pullPower;

public:
	// create a new level object at a given position with a given scale
	// the texture of the object has to be loaded already
	LevelObject(std::string name, float left, float top,
		const sf::Vector2f& scale, std::vector<sf::Texture*> textures,
		bool moveable, bool solid, float pullPower = 0, float rescale = 1);

	std::string GetName() const {return name;}

	bool IsMoveable() const {return moveable;}

	bool IsSolid() const {return solid;}

	float GetGravity() const {return gravity;}

	float GetPullPower() const {return pullPower;}

	sf::Sprite& GetModel() {return model;}

	// Return a transparent vertical lane
	sf::RectangleShape& GetGlow() {return glow;}

	const sf::Vector2f &GetVelocity() const {return velocity;}

	void SetVelocity(float x, float y) {velocity.x = x; velocity.y = y;}

	// Move the level object (and if there is, the glow around it)
	// Testing whether the object can be moved or not is not up to this function
	void MoveObject(float offsetX, float offsetY);

	// if an object has texture a, b, c,
	// change the texture in the a pattern like a -> b -> c -> b -> a
	// if the object only has one texture, the function will return
	void ChangeTexture();

	friend std::ostream& operator << (std::ostream& out, const LevelObject& obj);
};

#endif
