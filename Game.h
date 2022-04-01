#pragma once

//#include "settings.hpp"
//#include "level_details.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <map>
#include <ctime>

class State;
class BitmapText;

class Game {
public:
	Game();
	~Game();

	sf::RenderWindow *getWindow();
	void changeState(State *newState);
	void exit();

	std::string gameName;
	sf::Vector2i gameSize;
	int defaultScale;
	sf::Vector2u screenSize();
	int scale;
	sf::Vector2f screenOffset;
	float screenShakeTime = 0;
	sf::Color bgColor = sf::Color::Black;

	sf::Vector2f getCursorPosition();

	// Sprite for the cursor
	// Todo: this should be optional
	sf::Sprite cursorSprite;

	// Game variables
	bool fullscreen = false;

	void setFullscreen(bool fullscreen);

	// Plays a sound that continues even if the state changes
	void playGlobalSound(std::string filename);

private:
	sf::RenderWindow *window;

	State *state = nullptr;
	State *nextState = nullptr;
	bool exiting = false;

	// A sound that exists across states
	sf::Sound globalSound;
};
