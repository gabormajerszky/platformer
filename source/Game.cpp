#include "Game.h"

Game::Game(int resX, int resY, bool fullScreen, int levelIndex, Glow glow)
	: fullScreen(fullScreen), levelIndex(levelIndex - 1), gameOver(false), glow(glow)
{
	if (fullScreen)
		window.create(sf::VideoMode(resX, resY), "platformer", sf::Style::Fullscreen);
	else
		window.create(sf::VideoMode(resX, resY), "platformer", sf::Style::Titlebar | sf::Style::Close);

	window.setMouseCursorVisible(false);
	window.setKeyRepeatEnabled(false);

	// We need to store the resolution based on window.getSize(),
	// because when we use VideoMode's constructor, if the requested resolution
	// is not applicable, it'll still create the window with a different resolution

	this -> resX = window.getSize().x;
	this -> resY = window.getSize().y;

	scale.x = static_cast<float>(this -> resX) / DEFAULT_RES_X;
	scale.y = static_cast<float>(this -> resY) / DEFAULT_RES_Y;

	Level::LoadAllTextures();
	LoadLevels();
	LoadSounds();
	Menu();
}

void Game::Menu()
{
	// Load the texture for the background
	sf::Texture backgroundTexture;
	backgroundTexture.loadFromFile("res/xneg.png");
	sf::Sprite background(backgroundTexture);
	float bgLeft = (DEFAULT_RES_X - background.getGlobalBounds().width) / 2;
	float bgTop = (DEFAULT_RES_Y - background.getGlobalBounds().height) / 2;
	background.setScale(scale);
	background.setPosition(bgLeft * scale.x, bgTop * scale.y);

	// Load the texture for the custom cursor
	sf::Texture cursorTexture;
	cursorTexture.loadFromFile("res/RPG_GUI_moonmod_cursor.png");
	sf::Sprite cursor(cursorTexture);
	cursor.setScale(scale);

	// The x coordinate of the left side of the buttons
	int buttonLeft = 880;
	// The y coordinate of the top of the first button
	int buttonTop = 334;
	// Gap between buttons
	float gap = 150;

	sf::Vector2f playButtonPos(buttonLeft * scale.x, buttonTop * scale.y);
	sf::Vector2f optionsButtonPos(buttonLeft * scale.x, (buttonTop + gap) * scale.y);
	sf::Vector2f exitButtonPos(buttonLeft * scale.x, (buttonTop + 2 * gap) * scale.y);

	sf::Texture playTexture;
	playTexture.loadFromFile("res/pebble_buttons_play.png");
	sf::Sprite playButton(playTexture);
	playButton.setScale(scale);
	playButton.setPosition(playButtonPos);

	//sf::Texture optionsTexture;
	//optionsTexture.loadFromFile("res/pebble_buttons_menu.png");
	//sf::Sprite optionsButton(optionsTexture);
	//optionsButton.setScale(scale);
	//optionsButton.setPosition(optionsButtonPos);

	sf::Texture exitTexture;
	exitTexture.loadFromFile("res/pebble_buttons_exit.png");
	sf::Sprite exitButton(exitTexture);
	exitButton.setScale(scale);
	exitButton.setPosition(exitButtonPos);

	while (window.isOpen())
	{
		sf::Event event;

		// Event handling is done in a loop, because
		// multiple events can be fired simultaneously
		// and pollEvent puts the first in the queue to event
		// while returning whether or not there are events left

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				return;

			if (event.type == sf::Event::MouseButtonPressed)
			{
				float x = static_cast<float>(event.mouseButton.x);
				float y = static_cast<float>(event.mouseButton.y);

				if (playButton.getGlobalBounds().contains(x, y))
					Play();

                /*
				if (optionsButton.getGlobalBounds().contains(x, y))
					Options();
                    */

				if (exitButton.getGlobalBounds().contains(x, y))
					window.close();
			}

			if (event.type == sf::Event::MouseMoved)
			{
				float x = static_cast<float>(event.mouseMove.x);
				float y = static_cast<float>(event.mouseMove.y);
				cursor.setPosition(x, y);
			}
		}

		window.clear();
		window.draw(background);
		window.draw(playButton);
		// window.draw(optionsButton);
		window.draw(exitButton);
		window.draw(cursor);
		window.display();
	}
}

void Game::Play()
{
	player = Player(scale, &levels[levelIndex], &sounds);

	sf::Clock movementTimer;
	sf::Clock themeTimer;

	sf::Time delay(sf::milliseconds(10));
	sf::Time fpsMeasureTime(sf::milliseconds(1000));
	sf::Time themeDelay(sf::seconds(9));

	// Play the main theme in the beginning
	sounds["maintheme"].play();

	while (true)
	{
		// movementTimer assures that when the user keeps a movement key pressed,
		// we trigger the event in certain periods of time
		// we still need to check for regular keypresses, and trigger the movement event once,
		// so the player object reacts instantly (as fast as the main loop is)

		sf::Event event;

		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
			{
				return;
			}
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
			{
				player.Jump();
			}
		}

		if (movementTimer.getElapsedTime() > delay)
		{
			movementTimer.restart();

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
				player.MoveLeft();
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
				player.MoveRight();

			player.UpdatePosition();
			levels[levelIndex].UpdateObjects();
		}

        window.clear();
		DrawLevel();
		window.draw(player.GetModel());
		window.display();
		
		// Check if the player reached the end of the level
		if (player.IsPlayerOnExit())
		{
			NextLevel();

			// Check if it's the last level
			if (gameOver)
			{
				// End screen
				return;
			}

			// place the player at their spawn location
			player.PlaceToSpawn();
		}

		// Play the main theme periodically
		if (themeTimer.getElapsedTime() > themeDelay)
		{
			themeTimer.restart();
			sounds["maintheme"].play();
		}
	}
}

void Game::Options()
{
	// Options Menu...
}

void Game::LoadLevels()
{
	// LEVEL 1
	levels.push_back(Level(scale, 100, 830));
	// Background
	levels[0].SetBackground(0);
	// Ground
	levels[0].FillObject(-60, 930, 760, 930, Level::GROUNDBLOCK);
	levels[0].FillObject(1060, 930, 1920, 930, Level::GROUNDBLOCK);
	// Middle platform
	levels[0].FillObject(660, 530, 1160, 530, Level::GROUNDBLOCK);
	// Top left platform
	levels[0].FillObject(-50, 100, 450, 100, Level::GROUNDBLOCK);
	// Top right platform
	levels[0].FillObject(1370, 100, 1920, 100, Level::GROUNDBLOCK);
	// Gravity boxes
	levels[0].PlaceObject(1370, 50, Level::GRAVITYBOX);
	levels[0].PlaceObject(1470, 50, Level::GRAVITYBOX);
	levels[0].PlaceObject(1570, 50, Level::GRAVITYBOX);
	levels[0].PlaceObject(1670, 50, Level::GRAVITYBOX);
	levels[0].PlaceObject(1770, 50, Level::GRAVITYBOX);
	levels[0].PlaceObject(1870, 50, Level::GRAVITYBOX);
	// Exit
	levels[0].PlaceObject(925, 430, Level::EXITDOOR);

	
	// LEVEL 2
	levels.push_back(Level(scale, 925, 430));
	// Background
	levels[1].SetBackground(0);
	// Ground
	levels[1].FillObject(0, 900, 1920, 900, Level::GROUNDBLOCK);
	// Middle platform ground
	levels[1].FillObject(360, 530, 1160, 530, Level::GROUNDBLOCK);
	// Middle ceiling
	levels[1].FillObject(360, 380, 1160, 380, Level::GROUNDBLOCK);
	// Wall on the right
	levels[1].PlaceObject(1160, 430, Level::GROUNDBLOCK);
	levels[1].PlaceObject(1160, 480, Level::GROUNDBLOCK);
	// Gravity box
	levels[1].PlaceObject(660, 480, Level::GRAVITYBOX);
	// Exit
	levels[1].PlaceObject(100, 800, Level::EXITDOOR);

	// LEVEL 3
	levels.push_back(Level(scale, 100, 800));
	// Background
	levels[2].SetBackground(0);
	// Ground
	levels[2].FillObject(0, 900, 1920, 900, Level::GROUNDBLOCK);
	// First platform
	levels[2].FillObject(320, 100, 720, 100, Level::GROUNDBLOCK);
	// Second platform
	levels[2].FillObject(870, 350, 1270, 350, Level::GROUNDBLOCK);
	// Guarding wall for the gravity box
	levels[2].PlaceObject(770, 325, Level::GROUNDBLOCK);
	// Third platform
	levels[2].FillObject(1320, 600, 1920, 600, Level::GROUNDBLOCK);
	// Wall
	levels[2].FillObject(1270, 0, 1270, 300, Level::GROUNDBLOCK);
	// Gravity boxes
	levels[2].PlaceObject(400, 25, Level::GRAVITYBOX);
	levels[2].PlaceObject(650, 25, Level::GRAVITYBOX);
	levels[2].PlaceObject(900, 275, Level::GRAVITYBOX);
	// Exit
	levels[2].PlaceObject(1770, 500, Level::EXITDOOR);
}

void Game::LoadSounds()
{
	std::string soundNames[] =
	{
		"bonus",
		"changelevel",
		"deathsound",
		"fallen",
		"fallen2",
		"simpleeffect",
		"stepsound",
		"successound",
		"maintheme",
		"step1",
		"step2",
		"landing"
	};

	int soundCount = sizeof(soundNames) / sizeof(soundNames[0]);

	for (int i = 0; i < soundCount; ++i)
	{
		soundBuffers[soundNames[i]] = sf::SoundBuffer();
		soundBuffers[soundNames[i]].loadFromFile("res/" + soundNames[i] + ".wav");
		sounds[soundNames[i]].setBuffer(soundBuffers[soundNames[i]]);
	}

	// Sounds used so far are listed here
	sounds["changelevel"].setVolume(10);
	sounds["step1"].setVolume(10);
	sounds["maintheme"].setVolume(10);
}

void Game::NextLevel()
{
	sounds["changelevel"].play();

	// Check if it's the last level
	if (levelIndex + 1 == levels.size())
	{
		gameOver = true;
		return;
	}

	++levelIndex;
	player.SetLevel(&levels[levelIndex]);
}

void Game::DrawLevel()
{
	// Draw the background
	window.draw(levels[levelIndex].GetBackground());

	// Draw level objects
	for (size_t i = 0; i < levels[levelIndex].GetObjects().size(); ++i)
	{
		LevelObject &object = levels[levelIndex].GetObjects()[i];
		window.draw(object.GetModel());

		if (object.GetName() == "gravitybox")
			DrawGlow(object);
	}
}

void Game::DrawGlow(LevelObject& object)
{
	if (glow == Game::Glow::ALWAYS_OFF)
		return;

	if (glow == Game::Glow::FIRST_MAP_ONLY)
		if (levelIndex != 0)
			return;

	window.draw(object.GetGlow());
}
