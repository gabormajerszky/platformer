#include "Level.h"

std::vector<sf::Texture> Level::backgroundTextures;
std::vector<sf::Texture> Level::textures;

void Level::UpdateGravityBox()
{
	if (animationTimer.getElapsedTime() > animationDelay)
	{
		animationTimer.restart();
		for (size_t i = 0; i < levelObjects.size(); ++i)
			if (levelObjects[i].GetName() == "gravitybox")
				levelObjects[i].ChangeTexture();
	}
}

void Level::LoadAllTextures()
{
	// NOTE: the loadFromFile method is quite costly,
	// it takes about 250ms to load an image

	sf::Texture background01;
	background01.loadFromFile("res/background01.png");
	backgroundTextures.push_back(background01);

	sf::Texture groundSurface;
	groundSurface.loadFromFile("res/groundsurface.png");
	textures.push_back(groundSurface);

	sf::Texture ground;
	ground.loadFromFile("res/groundblock.png");
	ground.setSmooth(true);
	textures.push_back(ground);

	sf::Texture gravitybox_a;
	gravitybox_a.loadFromFile("res/gravitybox_a.png");
	textures.push_back(gravitybox_a);

	sf::Texture gravitybox_b;
	gravitybox_b.loadFromFile("res/gravitybox_b.png");
	textures.push_back(gravitybox_b);

	sf::Texture gravitybox_c;
	gravitybox_c.loadFromFile("res/gravitybox_c.png");
	textures.push_back(gravitybox_c);

	sf::Texture exitDoor;
	exitDoor.loadFromFile("res/exitdoor.png");
	textures.push_back(exitDoor);
}

Level::Level(const sf::Vector2f &scale, float spawnX, float spawnY) :
	scale(scale), animationDelay(sf::milliseconds(100)), spawn(spawnX, spawnY)
{
}

void Level::SetBackground(int index)
{
	background.setTexture(backgroundTextures[index]);
}

float Level::GetRescale(ObjectName name)
{
	switch (name)
	{
	case GROUNDSURFACE:
		return 0.25f;

	case GROUNDBLOCK:
		return 0.25f;

	case GRAVITYBOX:
		return 0.125f;

	case EXITDOOR:
		return 1;

	default:
		return 1;
	}
}

void Level::PlaceObject(float left, float top, ObjectName name)
{
	if (name == GROUNDSURFACE)
	{
		std::vector<sf::Texture*> v;
		v.push_back(&textures[name]);
		LevelObject obj("groundsurface", left, top, scale, v, false, true, 0, GetRescale(name));
		levelObjects.push_back(obj);
	}

	if (name == GROUNDBLOCK)
	{
		std::vector<sf::Texture*> v;
		v.push_back(&textures[name]);
		LevelObject obj("groundblock", left, top, scale, v, false, true, 0, GetRescale(name));
		levelObjects.push_back(obj);
	}

	else if (name == GRAVITYBOX)
	{
		std::vector<sf::Texture*> v;
		v.push_back(&textures[name]);
		v.push_back(&textures[name + 1]);
		v.push_back(&textures[name + 2]);
		LevelObject obj("gravitybox", left, top, scale, v, true, true, 0.125, GetRescale(name));
		levelObjects.push_back(obj);
	}

	else if (name == EXITDOOR)
	{
		std::vector<sf::Texture*> v;
		v.push_back(&textures[name]);
		LevelObject obj("exitdoor", left, top, scale, v, false, false, 0, GetRescale(name));
		levelObjects.push_back(obj);
	}
}

void Level::FillObject(float left, float top, float right, float bottom, ObjectName name)
{
	float width = textures[name].getSize().x * GetRescale(name);
	float height = textures[name].getSize().y * GetRescale(name);

	for (float x = left; x <= right; x += width)
		for (float y = top; y <= bottom; y += height)
			PlaceObject(x, y, name);
}

bool Level::MoveObject(int index, float xOffset, float yOffset)
{
	if (!levelObjects[index].IsMoveable())
		return false;

	sf::Sprite &model = levelObjects[index].GetModel();
	sf::Sprite ghostModel = model;

	// detect collisions with this "ghost model"
	ghostModel.move(xOffset, yOffset);

	sf::FloatRect &bounds = model.getGlobalBounds();
	sf::FloatRect &ghostBounds = ghostModel.getGlobalBounds();

	// check for collisions with every level object

	for (size_t i = 0; i < levelObjects.size(); ++i)
	{
		sf::FloatRect &objectBounds = levelObjects[i].GetModel().getGlobalBounds();
		if (i != index && ghostBounds.intersects(objectBounds))
		{
			// Adjust the object's position,
			// so it touches the other object it collides with

			float xDist = 0;
			float yDist = 0;

			if (xOffset < 0)
				xDist = bounds.left - (objectBounds.left + objectBounds.width);
			if (xOffset > 0)
				xDist = objectBounds.left - (bounds.left + bounds.width);
			if (yOffset < 0)
				yDist = bounds.top - (objectBounds.top + objectBounds.height);
			if (yOffset > 0)
				yDist = objectBounds.top - (bounds.top + bounds.height);

			// The two objects aren't touching at this point,
			// so we need to determine how much we have to move

			float xTime;
			float yTime;

			(xOffset == 0) ? xTime = 0 : xTime = xDist / xOffset;
			(yOffset == 0) ? yTime = 0 : yTime = yDist / yOffset;

			// xOffset AND yOffset need to be non-positive to have a collision,
			// that's why we need the greatest of the two times

			float collisionTime = std::max(xTime, yTime);
			const float precision = 0.99f;

			// Align the moving object with the other object
			model.move(precision * collisionTime * xOffset, precision * collisionTime * yOffset);
			levelObjects[index].SetVelocity(0, 0);

			return false;
		}
	}

	// Detect collisions with walls

	if (ghostBounds.left < 0)
	{
		model.setPosition(0, bounds.top);
		levelObjects[index].SetVelocity(0, 0);
		return false;
	}

	if (ghostBounds.left + ghostBounds.width > DEFAULT_RES_X * scale.x)
	{
		model.setPosition(DEFAULT_RES_X * scale.x - bounds.width, bounds.top);
		levelObjects[index].SetVelocity(0, 0);
		return false;
	}

	if (ghostBounds.top < 0)
	{
		model.setPosition(bounds.left, 0);
		levelObjects[index].SetVelocity(0, 0);
		return false;
	}

	if (ghostBounds.top + ghostBounds.height > DEFAULT_RES_Y * scale.y)
	{
		model.setPosition(bounds.left, DEFAULT_RES_Y * scale.y - bounds.height);
		levelObjects[index].SetVelocity(0, 0);
		return false;
	}

	// Finally, if we aren't going to hit anything,
	// we can move the object
	levelObjects[index].MoveObject(xOffset, yOffset);
	return true;
}

void Level::UpdateObjects()
{
	// Animate gravity boxes
	UpdateGravityBox();

	// Update the position's of all level objects if necessary
	for (size_t i = 0; i < levelObjects.size(); ++i)
	{
		if (levelObjects[i].IsMoveable())
		{
			float yVelocity = levelObjects[i].GetVelocity().y + levelObjects[i].GetGravity();
			levelObjects[i].SetVelocity(0, yVelocity);
			MoveObject(i, 0, yVelocity);
		}
	}
}
