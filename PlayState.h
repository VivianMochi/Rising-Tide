#pragma once

#include "State.h"
#include "SFML/Audio.hpp"

class PlayState : public State {
public:
	virtual void init() override;
	virtual void gotEvent(sf::Event event) override;
	virtual void update(sf::Time elapsed) override;
	virtual void render(sf::RenderWindow &window) override;

	enum {
		menu,
		playing
	} phase = menu;

private:
	void adjustMusicVolume(sf::Music &music, float desiredVolume, float factor);

	float cameraY = -25;

	sf::Sprite sun;
	sf::Sprite clouds;
	sf::Sprite dunes;

	sf::Sound clear1;
	sf::Sound clear2;
	sf::Sound clear3;

	sf::Music musicBase;
	sf::Music musicActive;
	sf::Music musicBeat;
	sf::Music musicWarning;
};

