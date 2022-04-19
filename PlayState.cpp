#include "PlayState.h"

#include "ResourceManager.h"
#include "ColorManager.h"
#include "Entity.h"
#include "RenderAssist.h"
#include <fstream>

#include <iostream>

void PlayState::init() {
	// Try to load save
	load();

	// Load the saved palette, or select a random unlocked palette
	if (saveData[selectedPalette] == -1) {
		cm::selectPalette(std::rand() % saveData[unlockedPalettes], true);
	}
	else {
		cm::selectPalette(saveData[selectedPalette], true);
	}

	initEntity(buttons);

	// Build buttons
	// This can definitely be cleaned up
	buttonStart = std::make_shared<Button>("Start");
	initEntity(*buttonStart);
	buttons.addButton(buttonStart);
	buttonExit = std::make_shared<Button>("Exit");
	initEntity(*buttonExit);
	buttons.addButton(buttonExit);
	buttonSubmit = std::make_shared<Button>("Submit");
	initEntity(*buttonSubmit);
	buttons.addButton(buttonSubmit);
	buttonShell = std::make_shared<Button>("Drain");
	initEntity(*buttonShell);
	buttons.addButton(buttonShell);
	buttonMenu = std::make_shared<Button>("Menu");
	initEntity(*buttonMenu);
	buttons.addButton(buttonMenu);
	if (PALETTE_SELECT_ENABLED) {
		buttonPaletteLeft = std::make_shared<Button>("PaletteLeft", rm::loadTexture("Resource/Image/PaletteButtons.png"), sf::IntRect(0, 0, 19, 10));
		initEntity(*buttonPaletteLeft);
		buttons.addButton(buttonPaletteLeft);
		buttonPaletteLink = std::make_shared<Button>("PaletteLink", rm::loadTexture("Resource/Image/PaletteButtons.png"), sf::IntRect(18, 0, 19, 10));
		initEntity(*buttonPaletteLink);
		buttons.addButton(buttonPaletteLink);
		buttonPaletteRight = std::make_shared<Button>("PaletteRight", rm::loadTexture("Resource/Image/PaletteButtons.png"), sf::IntRect(36, 0, 19, 10));
		initEntity(*buttonPaletteRight);
		buttons.addButton(buttonPaletteRight);
	}
	buttonMute = std::make_shared<Button>("Mute", rm::loadTexture("Resource/Image/MuteButton.png"), sf::IntRect(saveData[muted] ? 11 : 0, 0, 12, 14));
	initEntity(*buttonMute);
	buttons.addButton(buttonMute);

	initEntity(grid);
	grid.setPosition(GRID_LEFT, getGame()->gameSize.y + 4);

	initEntity(water);

	initEntity(waterBar);

	// Load sprites
	leftPane.setTexture(rm::loadTexture("Resource/Image/LeftPane.png"));
	leftPane.setPosition(-71, 0);
	rightPane.setTexture(rm::loadTexture("Resource/Image/RightPane.png"));
	rightPane.setPosition(getGame()->gameSize.x, 0);
	paletteSelect.setTexture(rm::loadTexture("Resource/Image/PalettePane.png"));
	paletteSelect.setPosition(leftPane.getPosition() + sf::Vector2f(2, PALETTE_SELECT_TOP));

	title.setTexture(rm::loadTexture("Resource/Image/Title.png"));
	sun.setTexture(rm::loadTexture("Resource/Image/Sun.png"));
	clouds.setTexture(rm::loadTexture("Resource/Image/Clouds.png"));
	dunes.setTexture(rm::loadTexture("Resource/Image/Dunes.png"));
	
	// Load sounds
	soundDig.setBuffer(rm::loadSoundBuffer("Resource/Sound/Dig.wav"));
	soundFlag.setBuffer(rm::loadSoundBuffer("Resource/Sound/Flag.wav"));
	soundUnflag.setBuffer(rm::loadSoundBuffer("Resource/Sound/Unflag.wav"));
	soundShellFind.setBuffer(rm::loadSoundBuffer("Resource/Sound/ShellFind.wav"));
	soundShellUse.setBuffer(rm::loadSoundBuffer("Resource/Sound/ShellUse.wav"));
	soundSelect.setBuffer(rm::loadSoundBuffer("Resource/Sound/Select.wav"));
	soundError.setBuffer(rm::loadSoundBuffer("Resource/Sound/Error.wav"));
	soundJelly.setBuffer(rm::loadSoundBuffer("Resource/Sound/Jelly.wav"));
	soundScore.setBuffer(rm::loadSoundBuffer("Resource/Sound/Score.wav"));
	soundStart.setBuffer(rm::loadSoundBuffer("Resource/Sound/Start.wav"));

	// Load music
	musicBase.openFromFile("Resource/Music/MusicBase.ogg");
	if (saveData[muted] == 1) {
		musicBase.setVolume(0);
	}
	musicBase.setLoop(true);

	musicActive.openFromFile("Resource/Music/MusicActive.ogg");
	musicActive.setVolume(0);
	musicActive.setLoop(true);

	musicBeat.openFromFile("Resource/Music/MusicBeat.ogg");
	musicBeat.setVolume(0);
	musicBeat.setLoop(true);

	musicWarning.openFromFile("Resource/Music/MusicWarning.ogg");
	musicWarning.setVolume(0);
	musicWarning.setLoop(true);

	if (!DEBUG_MUSIC_DISABLED) {
		musicBase.play();
		musicActive.play();
		musicBeat.play();
		musicWarning.play();
	}
}

void PlayState::gotEvent(sf::Event event) {
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			// Check buttons
			std::string clickedButton = buttons.clickPosition(getGame()->getCursorPosition());

			if (phase == menu) {
				if (clickedButton == "Start") {
					soundStart.play();
					phase = playing;
					if (level == -1) {
						loadLevel(0);
					}
				}
				else if (clickedButton == "Exit") {
					getGame()->exit();
				}
			}
			else if (phase == playing) {
				if (clickedButton == "Submit") {
					submit();
					soundSelect.play();
				}
				else if (clickedButton == "Drain") {
					if (shells > 0) {
						if (waterBar.waterLevel > 0) {
							shells -= 1;
							flashTime = 0;
							shellFlashTime = 1;
							soundShellUse.play();
							soundSelect.play();
						}
						else {
							soundError.play();
						}
						waterBar.drain();
					}
				}
				else if (clickedButton == "Menu") {
					phase = menu;
					soundSelect.play();
				}
				else if (clickedButton == "PaletteLeft") {
					saveData[selectedPalette] = cm::getCurrentPalette() - 1;
					if (saveData[selectedPalette] < 0) {
						saveData[selectedPalette] = 0;
					}
					cm::selectPalette(saveData[selectedPalette]);
					soundSelect.play();
					save();
				}
				else if (clickedButton == "PaletteLink") {
					saveData[selectedPalette] = -1;
					cm::selectPalette(level);
					soundSelect.play();
					save();
				}
				else if (clickedButton == "PaletteRight") {
					saveData[selectedPalette] = cm::getCurrentPalette() + 1;
					cm::selectPalette(saveData[selectedPalette]);
					soundSelect.play();
					save();
				}
				else if (clickedButton == "Mute") {
					saveData[muted] = !saveData[muted];
					buttonMute->baseRect = sf::IntRect(saveData[muted] ? 11 : 0, 0, 12, 14);
					soundSelect.play();
					save();
				}
				else {
					// Left click on board
					std::string found = grid.digPosition(getGame()->getCursorPosition() - grid.getPosition());
					if (found != "none") {
						waterBar.increment();

						findItem(found);

						playDigSound();
					}
				}
			}
			else if (phase == results) {
				if (clickedButton == "Next") {
					phase = playing;
					loadLevel(level + 1);
					buttonSubmit->text = "Submit";
					soundSelect.play();
				}
			}
			else if (phase == loss) {
				if (clickedButton == "Restart") {
					phase = playing;
					loadLevel(0);
					buttonSubmit->text = "Submit";
					soundSelect.play();

					score = 0;
					shells = 0;
				}
			}
		}
		else if (event.mouseButton.button == sf::Mouse::Right) {
			if (phase == playing) {
				// Right click in game
				int flagResult = grid.flagPosition(getGame()->getCursorPosition() - grid.getPosition(), flags == 0);
				flags -= flagResult;
				if (flagResult != 0) {
					if (flagResult > 0) {
						soundFlag.play();
					}
					else {
						soundUnflag.play();
					}
					flashTime = 0;
					flagFlashTime = 0.5;
				}
			}
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			if (phase == playing) {
				phase = menu;
			}
		}

		if (DEBUG_ENABLED) {
			if (event.key.code == sf::Keyboard::Num1) {
				int oldPalette = cm::getCurrentPalette();
				cm::init();
				cm::selectPalette(oldPalette, true);
			}
			else if (event.key.code == sf::Keyboard::Right) {
				loadLevel(level + 1);
			}
			else if (event.key.code == sf::Keyboard::Left) {
				if (level > 0) {
					loadLevel(level - 1);
				}
			}
		}
	}
}

void PlayState::update(sf::Time elapsed) {
	// Update palette
	cm::updatePalette(elapsed);

	// Update timers
	flashTime += elapsed.asSeconds();
	if (flashTime >= 0.2) {
		flashTime = 0;
		
		// This is a hacky way to get the water bar to constantly flash upon losing
		if (phase == loss) {
			waterBar.flood();
		}
	}
	bestFlashTime -= elapsed.asSeconds();
	if (bestFlashTime < 0) {
		bestFlashTime = 0;
	}
	scoreFlashTime -= elapsed.asSeconds();
	if (scoreFlashTime < 0) {
		scoreFlashTime = 0;
	}
	shellFlashTime -= elapsed.asSeconds();
	if (shellFlashTime < 0) {
		shellFlashTime = 0;
	}
	flagFlashTime -= elapsed.asSeconds();
	if (flagFlashTime < 0) {
		flagFlashTime = 0;
	}

	// Update camera position
	float desiredY = 0;
	if (phase == menu) {
		desiredY = -25;
	}
	cameraY += (desiredY - cameraY) * elapsed.asSeconds() * 2;

	// Update main menu pane
	desiredY = -135;
	if (phase == menu) {
		desiredY = 0;
	}
	menuPaneY += (desiredY - menuPaneY) * elapsed.asSeconds() * 5;

	// Do submission
	if (phase == submitting) {
		submitTime -= elapsed.asSeconds();
		if (submitTime <= 0) {
			std::string popped = grid.popSquare(poppingFlags);

			findItem(popped, poppingFlags);
			if (popped == "none") {
				if (!poppingFlags) {
					poppingFlags = true;
					submitTime = SUBMIT_INTERVAL_BREAK;
				}
				else {
					phase = results;
					buttonSubmit->text = "Next";
				}
			}
			else if (popped == "jelly" || popped == "shell") {
				submitTime = SUBMIT_INTERVAL_ITEM;
				playDigSound();
			}
			else {
				submitTime = SUBMIT_INTERVAL;
				playDigSound();
			}
		}
	}

	// Update grid
	desiredY = getGame()->gameSize.y - 104;
	if (phase == menu) {
		desiredY = getGame()->gameSize.y + 4;
	}
	grid.move((sf::Vector2f(GRID_LEFT, desiredY) - grid.getPosition()) * elapsed.asSeconds() * 5.0f);
	grid.update(elapsed);

	// Update panes
	float desiredX = 0;
	if (phase == menu) {
		desiredX = -71;
	}
	leftPane.move((sf::Vector2f(desiredX, 0) - leftPane.getPosition()) * elapsed.asSeconds() * 5.0f);
	leftPane.setColor(cm::getUIColor());

	paletteSelect.setPosition(leftPane.getPosition() + sf::Vector2f(2, PALETTE_SELECT_TOP));
	paletteSelect.setColor(cm::getUIColor());

	desiredX = getGame()->gameSize.x - 71;
	if (phase == menu) {
		desiredX = getGame()->gameSize.x;
	}
	rightPane.move((sf::Vector2f(desiredX, 0) - rightPane.getPosition()) * elapsed.asSeconds() * 5.0f);
	rightPane.setColor(cm::getUIColor());

	// Update water bar
	waterBar.update(elapsed);
	waterBar.setPosition(rightPane.getPosition());
	// Check for game loss
	// Theoretically, this should happen whenever the water level goes up,
	// but that can happen in a few places so I'm just putting it here
	if ((phase == playing || phase == submitting) && waterBar.waterLevel >= 10) {
		phase = loss;
		buttonSubmit->text = "Restart";
	}

	// Update buttons
	bool buttonsActive = phase != submitting && phase != results && phase != loss;
	buttonStart->setPosition(getGame()->gameSize.x / 2 - 55 / 2, menuPaneY + 87);
	buttonExit->setPosition(getGame()->gameSize.x / 2 - 55 / 2, menuPaneY + 105);
	buttonSubmit->setPosition(leftPane.getPosition() + sf::Vector2f(2, 44));
	buttonSubmit->enabled = phase != submitting;
	buttonMenu->setPosition(leftPane.getPosition() + sf::Vector2f(2, 119));
	buttonMenu->enabled = buttonsActive;
	buttonShell->setPosition(rightPane.getPosition() + sf::Vector2f(14, 44));
	buttonShell->enabled = shells > 0 && buttonsActive;
	if (PALETTE_SELECT_ENABLED) {
		bool atLeftEnd = cm::getCurrentPalette() == 0;
		bool atRightEnd = cm::getCurrentPalette() >= cm::getTotalPalettes() - 1 || (!PALETTE_SELECT_DEBUG_ENABLED && cm::getCurrentPalette() >= saveData[unlockedPalettes] - 1);
		buttonPaletteLeft->setPosition(leftPane.getPosition() + sf::Vector2f(2, PALETTE_BUTTONS_TOP));
		buttonPaletteLeft->enabled = !atLeftEnd && buttonsActive;
		buttonPaletteLink->setPosition(leftPane.getPosition() + sf::Vector2f(20, PALETTE_BUTTONS_TOP));
		buttonPaletteLink->enabled = buttonsActive && saveData[selectedPalette] != -1;
		buttonPaletteRight->setPosition(leftPane.getPosition() + sf::Vector2f(38, PALETTE_BUTTONS_TOP));
		buttonPaletteRight->enabled = !atRightEnd && buttonsActive;
	}
	buttonMute->setPosition(rightPane.getPosition() + sf::Vector2f(57, 105));
	buttonMute->enabled = buttonsActive;
	buttons.update(elapsed);

	// Update water
	water.masterDepth += (waterBar.waterLevel * 10 - water.masterDepth) * elapsed.asSeconds() * 2;
	water.update(elapsed);
	water.setPosition(grid.getPosition() + sf::Vector2f(0, 100));

	// Update music
	float volumeBase = 100;
	float volumeActive = 0;
	float volumeBeat = 0;
	float volumeWarning = 0;
	if (saveData[muted] == 1) {
		volumeBase = 0;
		volumeActive = 0;
		volumeBeat = 0;
		volumeWarning = 0;
	}
	else if (phase == playing) {
		if (waterBar.waterLevel >= 3) {
			volumeActive = 100;
		}
		else if (waterBar.waterLevel >= 2) {
			volumeActive = 50;
		}
		else if (waterBar.waterLevel >= 1) {
			volumeActive = 25;
		}

		if (waterBar.waterLevel >= 6) {
			volumeBeat = 100;
		}
		else if (waterBar.waterLevel >= 5) {
			volumeBeat = 50;
		}
		else if (waterBar.waterLevel >= 4) {
			volumeBeat = 25;
		}

		if (waterBar.waterLevel >= 9) {
			volumeWarning = 100;
		}
		else if (waterBar.waterLevel >= 8) {
			volumeWarning = 50;
		}
		else if (waterBar.waterLevel >= 7) {
			volumeWarning = 25;
		}
	}
	adjustMusicVolume(musicBase, volumeBase, elapsed.asSeconds() * 3);
	adjustMusicVolume(musicActive, volumeActive, elapsed.asSeconds() * 3);
	adjustMusicVolume(musicBeat, volumeBeat, elapsed.asSeconds() * 3);
	adjustMusicVolume(musicWarning, volumeWarning, elapsed.asSeconds() * 3);

	// Update background position
	title.setPosition(getGame()->gameSize.x / 2 - 80 / 2, menuPaneY + 32);
	title.setColor(cm::getWaterColor());
	sun.setPosition(141, -1 - cameraY * 0.9);
	sun.setColor(cm::getFlashColor());
	clouds.setPosition(0, -25 - cameraY * 0.9);
	clouds.setColor(cm::getCloudColor());
	dunes.setPosition(0, -25 - cameraY);
	dunes.setColor(cm::getSandColor());
}

void PlayState::render(sf::RenderWindow &window) {
	sf::RectangleShape sky(sf::Vector2f(getGame()->gameSize));
	sky.setFillColor(cm::getSkyColor());
	window.draw(sky);

	window.draw(sun);
	window.draw(clouds);
	window.draw(dunes);
	window.draw(title);

	BitmapText text;
	text.setTexture(rm::loadTexture("Resource/Image/Font.png"));
	text.setColor(cm::getTextColor());

	// Render panes
	window.draw(leftPane);
	if (PALETTE_SELECT_ENABLED) {
		window.draw(paletteSelect);
	}
	ra::renderJelly(window, sf::RenderStates::Default, leftPane.getPosition() + sf::Vector2f(3, 17));
	ra::renderFlag(window, sf::RenderStates::Default, leftPane.getPosition() + sf::Vector2f(3, 31));
	window.draw(rightPane);
	ra::renderJelly(window, sf::RenderStates::Default, rightPane.getPosition() + sf::Vector2f(15, 17));
	ra::renderShell(window, sf::RenderStates::Default, rightPane.getPosition() + sf::Vector2f(15, 31));

	// Render section title
	text.setText(levelName);
	text.setPosition(leftPane.getPosition() + sf::Vector2f(35 - text.getWidth() / 2, 4));
	window.draw(text);

	// Jelly and flag counts
	text.setText(std::to_string(jellies));
	text.setPosition(leftPane.getPosition() + sf::Vector2f(20, 18));
	window.draw(text);
	text.setText(std::to_string(flags));
	text.setPosition(leftPane.getPosition() + sf::Vector2f(20, 32));
	if (flagFlashTime > 0 && flashTime < 0.1) {
		text.setColor(cm::getFlashColor());
	}
	else {
		text.setColor(cm::getTextColor());
	}
	window.draw(text);

	// Score counts
	text.setText(std::to_string(saveData[best]));
	text.setPosition(rightPane.getPosition() + sf::Vector2f(40, 4));
	if (bestFlashTime > 0 && flashTime < 0.1) {
		text.setColor(cm::getFlashColor());
	}
	else {
		text.setColor(cm::getTextColor());
	}
	window.draw(text);

	text.setText(std::to_string(score));
	text.setPosition(rightPane.getPosition() + sf::Vector2f(32, 18));
	if (scoreFlashTime > 0 && flashTime < 0.1) {
		text.setColor(cm::getFlashColor());
	}
	else {
		text.setColor(cm::getTextColor());
	}
	window.draw(text);

	text.setText(std::to_string(shells));
	text.setPosition(rightPane.getPosition() + sf::Vector2f(32, 32));
	if (shellFlashTime > 0 && flashTime < 0.1) {
		text.setColor(cm::getFlashColor());
	}
	else {
		text.setColor(cm::getTextColor());
	}
	window.draw(text);

	// Palette name
	if (PALETTE_SELECT_ENABLED) {
		text.setText(cm::getPaletteName(cm::getCurrentPalette()));
		text.setPosition(paletteSelect.getPosition() + sf::Vector2f(27 - text.getWidth() / 2, 16));
		text.setColor(cm::getTextColor());
		window.draw(text);
	}

	window.draw(waterBar);
	window.draw(buttons);

	window.draw(grid);

	window.draw(water);
}

void PlayState::initEntity(Entity &entity) {
	entity.setState(this);
	entity.init();
}

void PlayState::save() {
	std::ofstream saveFile("Save.txt");
	saveFile << "version " << VERSION << "\n";
	for (auto entry : saveData) {
		saveFile << entry.first << " " << entry.second << "\n";
	}
	saveFile << "end";
	saveFile.close();
}

void PlayState::load() {
	// Clear any old save data and initialize default values
	saveData.clear();
	saveData[best] = 0;
	saveData[selectedPalette] = -1;
	saveData[unlockedPalettes] = 1;
	saveData[muted] = 0;

	std::ifstream saveFile("Save.txt");
	if (saveFile.is_open()) {
		std::cout << "    Save data found:\n";
		std::string token;
		saveFile >> token;

		int version = 0;
		if (token == "version") {
			saveFile >> version;
			saveFile >> token;
		}
		std::cout << "      Save version is 1." << version << ((version < VERSION) ? " (older)\n" : "\n");

		if (version == 0) {
			saveData[best] = std::stoi(token);
			std::cout << "      Loaded high score: " << saveData[best] << "\n";
		}
		else if (version == 1) {
			while (token != "end") {
				saveFile >> saveData[token];
				std::cout << "      Loaded stat " << token << ": " << saveData[token] << "\n";
				saveFile >> token;
			}
		}
		else {
			std::cout << "      Save version not recognized! (Currently running version 1." << VERSION << ")\n";
		}
	}
	else {
		std::cout << "    No save data found";
	}
	saveFile.close();
}

void PlayState::adjustMusicVolume(sf::Music &music, float desiredVolume, float factor) {
	float volume = music.getVolume();
	volume += (desiredVolume - volume) * factor;
	music.setVolume(volume);
}

void PlayState::playDigSound() {
	soundDig.setPitch(0.8 + std::rand() % 40 / 100.0f);
	soundDig.play();
}

void PlayState::loadLevel(int level) {
	this->level = level;

	// Unlock and change the palette
	if (level + 1 > saveData[unlockedPalettes]) {
		saveData[unlockedPalettes] = level + 1;
		if (saveData[unlockedPalettes] > cm::getTotalPalettes()) {
			saveData[unlockedPalettes] = cm::getTotalPalettes();
		}

		// Save unlocked palettes
		save();
	}
	if (saveData[selectedPalette] == -1) {
		cm::selectPalette(level);
	}

	if (level >= 26) {
		levelName = std::string("Section ?");
	}
	else {
		levelName = std::string("Section ") + char('A' + level);
	}

	int extraFlags = 5;
	extraFlags -= level / 2;
	if (extraFlags < 0) {
		extraFlags = 0;
	}

	jellies = 10 + level;
	flags = jellies + extraFlags;

	int waterBlocks = 10 - level / 2;
	if (waterBlocks < 2) {
		waterBlocks = 2;
	}
	waterBar.setMaxBlocks(waterBlocks);
	waterBar.resetSystem();

	grid.generateGrid(jellies, 2 + std::rand() % 3);
}

void PlayState::findItem(std::string item, bool flagged) {
	if (item == "jelly") {
		if (flagged) {
			score += 1;
			if (score > saveData[best]) {
				saveData[best] = score;
				bestFlashTime = 1;

				// Save best score
				save();
			}
			flashTime = 0;
			scoreFlashTime = 1;
			soundScore.play();
		}
		else {
			waterBar.flood(3);
			getGame()->screenShakeTime = 0.05;
			soundJelly.play();
		}
	}
	else if (item == "shell") {
		shells += 1;
		flashTime = 0;
		shellFlashTime = 1;
		soundShellFind.play();
	}
}

void PlayState::submit() {
	phase = submitting;
	poppingFlags = false;
	submitTime = SUBMIT_INTERVAL;
}

