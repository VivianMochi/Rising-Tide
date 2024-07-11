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
#include "SettingsTab.h"
#include "Title.h"
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

struct ModeOptions {
	bool digTime = true;
	bool realTime = false;
};

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
		playing,
		submitting,
		results,
		loss,
		shopping,
	} phase = menu;
	bool settingsOpen = false;

	ModeOptions options;

	std::map<std::string, int> saveData;

private:
	std::shared_ptr<Button> createButton(std::string buttonText);
	void approachNumber(float &input, float desired, float factor);
	sf::Vector2i getGridSquareFromCursorPosition();

	void adjustMusicVolume(sf::Music &music, float desiredVolume, float factor);
	void playDigSound();

	void loadLevel(int level);
	// Do the effect of digging up an item, unless flagged is true
	void findItem(std::string item, bool flagged = false);
	void submit();

	void goToMenu();
	void toggleSettingsTab();
	void adjustSetting(std::string setting, int newValue);

	// Action logic
	// Attempts to dig the square under the cursor
	void doDigLogic();
	// Attempts to flag the square under the cursor
	// If toggle is false, will use heldActionIsRemoval to determine which rule to use
	// Returns false if a removal happened
	bool doFlagLogic(bool toggle = true);
	// Attempts to mark the square under the cursor
	// If toggle is false, will use heldActionIsRemoval to determine which rule to use
	// Returns false if a removal happened
	bool doMarkLogic(bool toggle = true);

	float cameraY = -25;
	float menuPaneY = 0;

	float flashTime = 0;
	float flagFlashTime = 0;
	float scoreFlashTime = 0;
	float bestFlashTime = 0;
	float shellFlashTime = 0;
	float shellMarkerFlashTime = 0;
	float alertFlashTime = 0;
	float shopMusicDelayTime = 0;

	bool visualizeWater = false;

	float submitTime = 0;
	bool poppingFlags = false;

	ActionType heldAction = noAction;
	bool heldActionIsRemoval = false;
	sf::Vector2i actionStartGridSquare;
	sf::Vector2i actionLastGridSquare;

	// Level stats
	int level = -1;
	int flags = 0;
	int levelFlagCount = 0;
	int shellsFound = 0;
	std::string levelName = "";
	float levelTime = 0;
	bool levelTimeTicking = false;

	// Player stats
	int score = 0;
	int shells = 0;

	// Demo holdovers
	bool showTimer = false;

	ButtonManager buttons;

	std::shared_ptr<Button> buttonStart;
	std::shared_ptr<Button> buttonClassic;
	std::shared_ptr<Button> buttonSpeedy;
	std::shared_ptr<Button> buttonShop;
	std::shared_ptr<Button> buttonExit;
	std::shared_ptr<Button> buttonSubmit;
	std::shared_ptr<Button> buttonShellA;
	std::shared_ptr<Button> buttonShellB;
	std::shared_ptr<Button> buttonMenu;
	std::shared_ptr<Button> buttonSettings;
	std::shared_ptr<Button> buttonPaletteLeft;
	std::shared_ptr<Button> buttonPaletteLink;
	std::shared_ptr<Button> buttonPaletteRight;

	Grid grid;
	Water water;

	JellyShop shop;

	std::shared_ptr<Aquarium> aquarium;

	SettingsTab settings;

	sf::Sprite leftPane;
	sf::Sprite rightPane;
	sf::Sprite paletteSelect;
	WaterBar waterBar;
	
	Title title;
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
	sf::Sound soundClick;
	sf::Sound soundShopJingle;

	sf::Music musicBase;
	sf::Music musicActive;
	sf::Music musicBeat;
	sf::Music musicWarning;
	sf::Music musicShop;
};

