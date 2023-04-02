#pragma once

#include "State.h"
#include "Entity.h"
#include "Grid.h"
#include "Water.h"
#include "BitmapText.h"
#include "WaterBar.h"
#include "ButtonManager.h"
#include "ButtonBar.h"
#include "JellyShop.h"
#include "Aquarium.h"
#include <SFML/Audio.hpp>
#include <map>

// The minor version of the game
const int VERSION = 1;

const int GRID_LEFT = 70;
const float SUBMIT_INTERVAL = 0.1;
const float SUBMIT_INTERVAL_ITEM = 0.3;
const float SUBMIT_INTERVAL_BREAK = 1;
const float PALETTE_SELECT_TOP = 82;
const float PALETTE_BUTTONS_TOP = PALETTE_SELECT_TOP + 25;

const bool NEW_MODE = true;

const bool DEBUG_ENABLED = true;
const bool PALETTE_SELECT_DEBUG_ENABLED = false;
const bool DEBUG_MUSIC_DISABLED = false;
const bool DEBUG_DEMO_MODE = false;

// SaveData keys
// Total jellies
const std::string totalJellies = "jellies";
// The high score
const std::string best = "best";
// The selected palette (-1 is the code to link the palette to the current level)
const std::string selectedPalette = "palette";
// The number of palettes that have been unlocked
const std::string unlockedPalettes = "unlocked";
const std::string musicVolume = "music";
const std::string soundVolume = "sound";

class PlayState : public State {
public:
	virtual void init() override;
	virtual void gotEvent(sf::Event event) override;
	virtual void update(sf::Time elapsed) override;
	virtual void render(sf::RenderWindow &window) override;

	void initEntity(Entity &entity);

	void save();
	void load();

	enum {
		menu,
		setup,
		playing,
		submitting,
		results,
		loss,
		shopping,
	} phase = menu;

	enum {
		classic,
		speedy,
	} mode = classic;

	std::map<std::string, int> saveData;

private:
	std::shared_ptr<Button> createButton(std::string buttonText);
	void approachNumber(float &input, float desired, float factor);

	void adjustMusicVolume(sf::Music &music, float desiredVolume, float factor);
	void playDigSound();

	void loadLevel(int level);
	// Do the effect of digging up an item, unless flagged is true
	void findItem(std::string item, bool flagged = false);
	void submit();

	void goToMenu();

	float cameraY = -25;
	float menuPaneY = 0;
	float setupMenuPaneY = 135;

	float flashTime = 0;
	float flagFlashTime = 0;
	float scoreFlashTime = 0;
	float bestFlashTime = 0;
	float shellFlashTime = 0;
	float alertFlashTime = 0;

	float submitTime = 0;
	bool poppingFlags = false;

	// Level stats
	int level = -1;
	int jellies = 0;
	int flags = 0;
	std::string levelName = "";
	float levelTime = 0;
	bool levelTimeTicking = false;

	// Player stats
	int score = 0;
	int shells = 0;

	// Demo holdovers
	bool showTimer = true;
	bool showVolume = false;

	ButtonManager buttons;

	std::shared_ptr<Button> buttonStart;
	std::shared_ptr<Button> buttonClassic;
	std::shared_ptr<Button> buttonSpeedy;
	std::shared_ptr<Button> buttonShop;
	std::shared_ptr<Button> buttonExit;
	std::shared_ptr<Button> buttonSubmit;
	std::shared_ptr<Button> buttonShell;
	std::shared_ptr<Button> buttonMenu;
	std::shared_ptr<Button> buttonPaletteLeft;
	std::shared_ptr<Button> buttonPaletteLink;
	std::shared_ptr<Button> buttonPaletteRight;
	std::shared_ptr<ButtonBar> barMusic;
	std::shared_ptr<ButtonBar> barSound;

	Grid grid;
	Water water;

	JellyShop shop;

	std::shared_ptr<Aquarium> aquarium;

	sf::Sprite leftPane;
	sf::Sprite rightPane;
	sf::Sprite paletteSelect;
	WaterBar waterBar;
	
	sf::Sprite title;
	sf::Sprite sun;
	sf::Sprite clouds;
	sf::Sprite dunes;

	sf::Sound soundDig;
	sf::Sound soundFlag;
	sf::Sound soundUnflag;
	sf::Sound soundShellFind;
	sf::Sound soundShellUse;
	sf::Sound soundSelect;
	sf::Sound soundError;
	sf::Sound soundJelly;
	sf::Sound soundScore;
	sf::Sound soundStart;
	sf::Sound soundWater;

	sf::Music musicBase;
	sf::Music musicActive;
	sf::Music musicBeat;
	sf::Music musicWarning;
};

