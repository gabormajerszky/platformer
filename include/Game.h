#ifndef GAME_H
#define GAME_H

extern const int DEFAULT_RES_X;
extern const int DEFAULT_RES_Y;

#include "Player.h"
#include "Level.h"

class Game
{
	// Brought the public part in front, because we need the enum declaration
public:
	enum class Glow
	{
		ALWAYS_OFF,
		FIRST_MAP_ONLY,
		ALWAYS_ON
	};

	// Game constructor with default values of full HD fullscreen
	// if the provided resolution can't be displayed on a given monitor,
	// an optimal resolution will be chosed by SFML
	Game(int resX = DEFAULT_RES_X, int resY = DEFAULT_RES_Y, bool fullScreen = true,
		int levelIndex = 1, Glow glow = Glow::FIRST_MAP_ONLY);

private:
	int resX;
	int resY;
	bool fullScreen;
	bool gameOver;

	Player player;
	
	int levelIndex;
	std::vector<Level> levels;
	std::map<std::string, sf::SoundBuffer> soundBuffers;
	std::map<std::string, sf::Sound> sounds;

	sf::RenderWindow window;
	sf::Vector2f scale;

	Glow glow;

	void Menu();
	void Play();
	void Options();

	// Load every level (from source code)
	void LoadLevels();

	// Load every sound
	void LoadSounds();

	// Change to the next level
	void NextLevel();

	// Draw every platform on the screen
	void DrawLevel();

	// Draw glows of gravity boxes, if needed
	void DrawGlow(LevelObject& object);
};

#endif
