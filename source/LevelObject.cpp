#include "LevelObject.h"

LevelObject::LevelObject(std::string name, float left, float top,
						 const sf::Vector2f& scale, std::vector<sf::Texture*> textures,
						 bool moveable, bool solid, float pullPower, float rescale)
						 : step(1), velocity(0, 0), gravity(0.1f)
{
	this -> name = name;
	this -> moveable = moveable;
	this -> textures = textures;
	this -> solid = solid;
	this -> pullPower = pullPower;
	currentTexture = 0;

	model.setTexture(*textures[currentTexture]);
	model.setPosition(left * scale.x, top * scale.y);
	model.setScale(scale.x * rescale, scale.y * rescale);

	float width = model.getGlobalBounds().width;
	float height = DEFAULT_RES_Y * scale.y;

	if (name == "gravitybox")
	{
		glow.setSize(sf::Vector2f(width, height));
		glow.setFillColor(sf::Color(200, 100, 100, 50));
		glow.setPosition(model.getGlobalBounds().left, 0);
	}
}

void LevelObject::MoveObject(float offsetX, float offsetY)
{
	model.move(offsetX, offsetY);
	glow.move(offsetX, 0);
}

void LevelObject::ChangeTexture()
{
	size_t textureCount = textures.size();
	if (textureCount == 1)
		return;

	if (currentTexture + 1 == textureCount)
		step = -1;
	if (currentTexture == 0)
		step = 1;

	currentTexture += step;
	model.setTexture(*textures[currentTexture]);
}

std::ostream& operator << (std::ostream& out, const LevelObject& obj)
{
	float left = obj.model.getGlobalBounds().left;
	float top = obj.model.getGlobalBounds().top;
	float right = left + obj.model.getGlobalBounds().width;
	float bottom = top + obj.model.getGlobalBounds().height;

	std::string moveable;
	(obj.moveable) ? moveable = "Yes" : moveable = "No";
	out << "Position x: " << left << " - " << right << std::endl;
	out << "Position y: " << top << " - " << bottom << std::endl;
	out << "Width x Height: " << right - left << " x " << bottom - top << std::endl;
	out << "Moveable: " << moveable << std::endl;

	return out;
}
