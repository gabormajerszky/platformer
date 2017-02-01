#include "Player.h"

void Player::AdjustAnimation()
{
	// The player isn't moving horizontally
	if (!velocity.x)
		animationNumber = 0;

	// The player is moving to the right
	if (velocity.x > 0)
	{
		if (animationType == "walkRight")
		{
			++animationNumber;
			animationNumber %= walkRight.size();
		}
		else if (animationType == "walkLeft")
		{
			animationType = "walkRight";
			animationNumber = 0;
		}

		if (animationNumber == 1)
			(*sounds)["step1"].play();
		if (animationNumber == 5)
			(*sounds)["step1"].play();
	}

	// The player is moving to the left
	if (velocity.x < 0)
	{
		if (animationType == "walkLeft")
		{
			++animationNumber;
			animationNumber %= walkLeft.size();
		}
		else if (animationType == "walkRight")
		{
			animationType = "walkLeft";
			animationNumber = 0;
		}

		if (animationNumber == 1)
			(*sounds)["step1"].play();
		if (animationNumber == 5)
			(*sounds)["step1"].play();
	}

	// The player is in the air
	if (velocity.y)
		(velocity.x) ? animationNumber = 1 : animationNumber = 0;
}

void Player::SetTexture()
{
    if (animationType == "walkRight")
    {
        model.setTexture(walkRight[animationNumber]);
    }
    else if (animationType == "walkLeft")
    {
        model.setTexture(walkLeft[animationNumber]);
    }
    else {
        throw std::runtime_error("Unexpected animation type " + animationType + "\n");
    }
        
}

bool Player::HorizontalCollision(float xOffset)
{
	// We detect a collision by creating a "ghost model",
	// which represents the object in the next iteration, and check
	// whether it intersects any of the walls or other objects
	// Also, since we'll stop the object before a collision,
	// we'll adjust the position of the player slightly to get rid of this gap

	const float precision = 0.99f;

	sf::Sprite ghostModel = model;
	ghostModel.move(xOffset, 0);

	sf::FloatRect bounds = model.getGlobalBounds();
	sf::FloatRect ghostBounds = ghostModel.getGlobalBounds();

	if (xOffset < 0)
	{
		if (ghostBounds.left < 0)
		{
			model.setPosition(0, bounds.top);
			return true;
		}

		std::vector<LevelObject> &platforms = currentLevel -> GetObjects();
		for (size_t i = 0; i < platforms.size(); ++i)
		{
			sf::FloatRect objectBounds = platforms[i].GetModel().getGlobalBounds();

			if (ghostBounds.intersects(objectBounds) && platforms[i].IsSolid())
			{
				currentLevel -> MoveObject(i, xOffset, 0);
				objectBounds = platforms[i].GetModel().getGlobalBounds();
				float gap = objectBounds.left + objectBounds.width - bounds.left;
				model.move(gap * precision, 0);
				return true;
			}
		}

		return false;
	}

	else if (xOffset > 0)
	{
		if (ghostBounds.left + ghostBounds.width > DEFAULT_RES_X * scale.x)
		{
			model.setPosition(DEFAULT_RES_X * scale.x - bounds.width, bounds.top);
			return true;
		}
		
		std::vector<LevelObject> &platforms = currentLevel -> GetObjects();
		for (size_t i = 0; i < platforms.size(); ++i)
		{
			sf::FloatRect objectBounds = platforms[i].GetModel().getGlobalBounds();

			if (ghostBounds.intersects(objectBounds) && platforms[i].IsSolid())
			{
				currentLevel -> MoveObject(i, xOffset, 0);
				objectBounds = platforms[i].GetModel().getGlobalBounds();
				float gap = objectBounds.left - (bounds.left + bounds.width);
				model.move(gap * precision, 0);
				return true;
			}
		}

		return false;
	}

	// there is no movement
	else
		return false;
}

bool Player::VerticalCollision(float yOffset)
{
	// we detect a collision by creating a "ghost model",
	// which represents the object in the next iteration, and check
	// whether it intersects any of the walls or other objects
	// also, since we'll stop the object before a collision,
	// we'll adjust the position of the player slightly to get rid of this gap

	const float precision = 0.99f;

	sf::Sprite ghostModel = model;
	ghostModel.move(0, yOffset);

	sf::FloatRect bounds = model.getGlobalBounds();
	sf::FloatRect ghostBounds = ghostModel.getGlobalBounds();

	if (yOffset < 0)
	{
		if (ghostBounds.top < 0)
		{
			model.setPosition(bounds.left, 0);
			return true;
		}
		
		std::vector<LevelObject> &platforms = currentLevel -> GetObjects();
		for (size_t i = 0; i < platforms.size(); ++i)
		{
			sf::FloatRect objectBounds = platforms[i].GetModel().getGlobalBounds();

			if (ghostBounds.intersects(objectBounds) && platforms[i].IsSolid())
			{
				float gap = objectBounds.top + objectBounds.height - bounds.top;
				model.move(0, gap * precision);
				return true;
			}
		}

		return false;
	}

	else if (yOffset > 0)
	{
		if (ghostBounds.top + ghostBounds.height > DEFAULT_RES_Y * scale.y)
		{
			// If we want to detect collisions with the bottom of the screen
			// model.setPosition(bounds.left, DEFAULT_RES_Y * scale.y - bounds.height);
			// return true;
		}

		// After the player left the screen downwards
		if (ghostBounds.top > DEFAULT_RES_Y * scale.y)
			alive = false;
		
		std::vector<LevelObject> &platforms = currentLevel -> GetObjects();
		for (size_t i = 0; i < platforms.size(); ++i)
		{
			sf::FloatRect objectBounds = platforms[i].GetModel().getGlobalBounds();

			if (ghostBounds.intersects(objectBounds) && platforms[i].IsSolid())
			{
				float gap = objectBounds.top - (bounds.top + bounds.height);
				model.move(0, gap * precision);
				return true;
			}
		}

		return false;
	}

	// there is no movement
	else
		return false;
}

void Player::AddGravityBoxPull()
{
	std::vector<LevelObject> &platforms = currentLevel -> GetObjects();
	for (size_t i = 0; i < platforms.size(); ++i)
	{
		if (platforms[i].GetName() == "gravitybox")
		{
			sf::FloatRect bounds = model.getGlobalBounds();
			sf::FloatRect objectBounds = platforms[i].GetModel().getGlobalBounds();
			float xCenter = bounds.left + bounds.width / 2;

			// we're in the vertical lane of the pull
			if (xCenter > objectBounds.left && xCenter < objectBounds.left + objectBounds.width)
			{
				float pullPower = platforms[i].GetPullPower();

				// the gravity box is under us
				if (bounds.top < objectBounds.top)
					velocity.y += pullPower * scale.y;

				// the gravity box is above us
				if (bounds.top > objectBounds.top)
					velocity.y -= pullPower * scale.y;
			}
		}
	}
}

Player::Player(const sf::Vector2f& scale, Level* level, std::map<std::string, sf::Sound>* sounds) :
	playerSpeed(2.5),
	jumpSpeed(5),
	gravity(0.10f),
	alive(true),
	onGround(false),
	velocity(sf::Vector2f(0, 0)),
	scale(scale),
	animationDelay(sf::milliseconds(25)),
	animationType("walkRight"),
	animationNumber(0)
{
	currentLevel = level;
	this -> sounds = sounds;

	for (int i = 0; i <= 7; ++i)
	{
		sf::Texture texture;
		std::string fileName;

		fileName = "res/obj_Walk00" + std::to_string(i) + ".png";
		texture.loadFromFile(fileName, sf::IntRect(41, 48, 48, 75));
		texture.setSmooth(true);
		walkRight.push_back(texture);

		fileName = "res/obj_Walk10" + std::to_string(i) + ".png";
		texture.loadFromFile(fileName, sf::IntRect(41, 48, 48, 75));
		texture.setSmooth(true);
		walkLeft.push_back(texture);
	}
	
	// Load the default texture of the player
	// (looking right, standing still)
	SetTexture();

	// Set up scale and player position
	model.setScale(scale.x, scale.y);
	PlaceToSpawn();
}

void Player::PlaceToSpawn()
{
	float x = currentLevel->GetSpawn().x;
	float y = currentLevel->GetSpawn().y;
	Place(x, y);
}

void Player::MoveLeft()
{
	velocity.x = -playerSpeed * scale.x;
}

void Player::MoveRight()
{
	velocity.x = playerSpeed * scale.x;
}

void Player::Jump()
{
	if (velocity.y)
		return;

	velocity.y = -jumpSpeed * scale.y;
}

void Player::UpdatePosition()
{
	// Add pulling forces by gravity boxes
	AddGravityBoxPull();

	// Update the position horizontally
	if (!HorizontalCollision(velocity.x))
		model.move(velocity.x, 0);

	// Update the player's texture
	if (animationTimer.getElapsedTime() > animationDelay)
	{
		animationTimer.restart();
		AdjustAnimation();
		SetTexture();
	}
	
	velocity.x = 0;

	// Update the position vertically
	velocity.y += gravity * scale.y;

	if (!VerticalCollision(velocity.y))
	{
		onGround = false;
		model.move(0, velocity.y);
	}
	else
	{
		velocity.y = 0;

		// Play the landing sounds only once
		if (!onGround)
		{
			(*sounds)["step1"].play();
			onGround = true;
		}
	}

	// Respawn the player if necessary
	if (!alive)
	{
		alive = true;
		PlaceToSpawn();
	}
}

bool Player::IsPlayerOnExit()
{
	std::vector<LevelObject> &platforms = currentLevel -> GetObjects();
	for (size_t i = 0; i < platforms.size(); ++i)
	{
		if (platforms[i].GetName() == "exitdoor")
		{
			sf::FloatRect bounds = model.getGlobalBounds();
			sf::FloatRect objectBounds = platforms[i].GetModel().getGlobalBounds();
			float yCenter = bounds.top + bounds.height / 2;

			if (bounds.left > objectBounds.left &&
				bounds.left + bounds.width < objectBounds.left + objectBounds.width &&
				yCenter > objectBounds.top &&
				yCenter < objectBounds.top + objectBounds.height)
			{
				return true;
			}

		}
	}
	return false;
}

std::ostream& operator<<(std::ostream& out, const Player player)
{
	out << "Player Speed (constant): " << player.playerSpeed << std::endl;
	out << "Jump Speed (constant): " << player.jumpSpeed << std::endl;
	out << "Gravity (constant): " << player.gravity << std::endl;
	out << "Velocity: " << player.velocity.x << ", " << player.velocity.y << std::endl;
	out << "Scale: " << player.scale.x << ", " << player.scale.y << std::endl;
	out << "Position (top left corner): " << player.model.getGlobalBounds().left
		<< ", " << player.model.getGlobalBounds().top << std::endl;
	out << "Size: " << player.model.getGlobalBounds().width
		<< " x " << player.model.getGlobalBounds().height << std::endl;
	return out;
}
