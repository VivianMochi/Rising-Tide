#include "Game.h"

#include "State.h"
#include "ResourceManager.h"
#include "PlayState.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <filesystem>

Game::Game() {
	// Define game info
	gameName = "LD50";
	gameSize = { 240, 135 };
	defaultScale = 4;

	// Create game window
	setFullscreen(false);

	// Show the welcome message
	std::cout << "Debug messages are printed here:\n";

	// Initial state
	changeState(new PlayState());

	// Set the cursor sprite
	cursorSprite.setTexture(rm::loadTexture("Resource/Image/Cursor.png"));

	// Start the clock
	sf::Clock clock;

	while (window->isOpen()) {
		// State
		if (nextState) {
			// Load up the new state
			if (state)
				delete state;
			state = nextState;
			nextState = nullptr;
			state->setGame(this);
			state->init();

			// Restart the clock to account for load time
			//clock.restart();
		}

		// Event
		sf::Event event;
		while (window->pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				exit();
			}
			else if (event.type == sf::Event::Resized) {
				window->setView(sf::View(sf::FloatRect(0, 0, event.size.width, event.size.height)));
			}
			else if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::F11) {
					setFullscreen(!fullscreen);
				}
			}

			if (state) {
				state->gotEvent(event);
			}
		}

		// Update
		sf::Time elapsed = clock.restart();
		// If the game freezes, make sure no time passes
		if (elapsed.asMilliseconds() >= 250) {
			elapsed = sf::Time::Zero;
		}
		if (screenShakeTime > 0) {
			screenShakeTime -= elapsed.asSeconds();
		}

		// Set the cursor position
		cursorSprite.setPosition(std::round(getCursorPosition().x), std::round(getCursorPosition().y));

		if (state) {
			state->update(elapsed);
		}

		// Render
		window->clear(bgColor);
		if (state) {
			state->render(*window);
		}
		// Draw the cursor
		window->draw(cursorSprite);

		// Pixelate
		sf::Texture postTexture;
		postTexture.create(window->getSize().x, window->getSize().y);
		postTexture.update(*window);

		sf::RectangleShape overlay;
		overlay.setFillColor(bgColor);
		overlay.setSize(sf::Vector2f(window->getSize()));
		window->draw(overlay);

		sf::Sprite postSprite(postTexture, sf::IntRect(0, 0, gameSize.x, gameSize.y));
		scale = std::min(window->getSize().x / gameSize.x, window->getSize().y / gameSize.y);
		postSprite.setScale(scale, scale);
		postSprite.setPosition(window->getSize().x / 2 - postSprite.getGlobalBounds().width / 2, window->getSize().y / 2 - postSprite.getGlobalBounds().height / 2);
		screenOffset = postSprite.getPosition();
		if (screenShakeTime > 0) {
			postSprite.move((std::rand() % 3 - 1) * scale, (std::rand() % 3 - 1) * scale);
		}
		window->draw(postSprite);

		window->display();

		// Exit
		if (exiting) {
			window->close();
		}
	}
}


Game::~Game() {
	if (state)
		delete state;
	if (nextState)
		delete nextState;

	delete window;
}

sf::RenderWindow *Game::getWindow() {
	return window;
}

void Game::changeState(State *newState) {
	if (nextState)
		delete nextState;
	nextState = newState;
}

void Game::exit() {
	exiting = true;
}

sf::Vector2u Game::screenSize() {
	return window->getSize();
}

sf::Vector2f Game::getCursorPosition() {
	return (sf::Vector2f(sf::Mouse::getPosition(*window)) - screenOffset) / float(scale);
}

void Game::setFullscreen(bool fullscreen) {
	this->fullscreen = fullscreen;
	if (window) {
		delete window;
	}
	if (fullscreen) {
		window = new sf::RenderWindow(sf::VideoMode(1920, 1080), gameName, sf::Style::Fullscreen);
		//window = new sf::RenderWindow(sf::VideoMode(2560, 1440), gameName, sf::Style::Fullscreen);
	}
	else {
		window = new sf::RenderWindow(sf::VideoMode(gameSize.x * defaultScale, gameSize.y * defaultScale), gameName);
	}
	window->setFramerateLimit(60);
	window->setKeyRepeatEnabled(false);
	scale = std::min(window->getSize().x / gameSize.x, window->getSize().y / gameSize.y);

	// Hide the OS cursor
	window->setMouseCursorVisible(false);
}

void Game::playGlobalSound(std::string filename) {
	globalSound.setBuffer(rm::loadSoundBuffer(filename));
	globalSound.play();
}
