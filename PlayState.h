#pragma once

#include "State.h"
#include "Entity.h"
#include "Grid.h"
#include "Water.h"
#include "BitmapText.h"
#include "WaterBar.h"
#include "ButtonManager.h"
#include <SFML/Audio.hpp>

const int GRID_LEFT = 70;

class PlayState : public State {
public:
	virtual void init() override;
	virtual void gotEvent(sf::Event event) override;
	virtual void update(sf::Time elapsed) override;
	virtual void render(sf::RenderWindow &window) override;

	void initEntity(Entity &entity);

	enum {
		menu,
		playing
	} phase = menu;

private:
	void adjustMusicVolume(sf::Music &music, float desiredVolume, float factor);

	void loadLevel(int level);

	float cameraY = -25;
	float menuPaneY = 0;
	float flashTime = 0;
	float shellFlashTime = 0;

	// Level stats
	int level = -1;
	int jellies = 0;
	int flags = 0;
	std::string levelName = "";

	// Player stats
	int best = 0;
	int score = 0;
	int shells = 0;

	ButtonManager buttons;

	std::shared_ptr<Button> buttonStart;
	std::shared_ptr<Button> buttonExit;
	std::shared_ptr<Button> buttonSubmit;
	std::shared_ptr<Button> buttonShell;
	std::shared_ptr<Button> buttonMenu;

	Grid grid;
	Water water;

	sf::Sprite leftPane;
	sf::Sprite rightPane;
	WaterBar waterBar;

	sf::Sprite sun;
	sf::Sprite clouds;
	sf::Sprite dunes;
	sf::Sprite camp;

	sf::Sound soundDig;
	sf::Sound soundClear1;
	sf::Sound soundClear2;
	sf::Sound soundClear3;

	sf::Music musicBase;
	sf::Music musicActive;
	sf::Music musicBeat;
	sf::Music musicWarning;
};

