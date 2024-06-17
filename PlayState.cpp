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

	// Build buttons
	initEntity(buttons);

	buttonStart = createButton("Start");
	buttonClassic = createButton("Classic");
	buttonSpeedy = createButton("Speedy");
	buttonShop = createButton("Shop");
	buttonExit = createButton("Exit");
	buttonSubmit = createButton("Submit");

	buttonShellA = std::make_shared<Button>("Drain", rm::loadTexture("Resource/Image/SmallerButton.png"), sf::IntRect(0, 0, 38, 16));
	buttonShellA->showText = true;
	buttons.initButton(buttonShellA);

	buttonShellB = std::make_shared<Button>("$", rm::loadTexture("Resource/Image/SmallerButton.png"), sf::IntRect(0, 0, 38, 16));
	buttonShellB->showText = true;
	buttons.initButton(buttonShellB);
	
	buttonMenu = std::make_shared<Button>("Menu", rm::loadTexture("Resource/Image/MenuButton.png"), sf::IntRect(0, 0, 41, 16));
	buttons.initButton(buttonMenu);

	buttonSettings = std::make_shared<Button>("Settings", rm::loadTexture("Resource/Image/SettingsButton.png"), sf::IntRect(0, 0, 13, 16));
	buttons.initButton(buttonSettings);

	buttonPaletteLeft = std::make_shared<Button>("PaletteLeft", rm::loadTexture("Resource/Image/PaletteButtons.png"), sf::IntRect(0, 0, 19, 10));
	buttons.initButton(buttonPaletteLeft);
	buttonPaletteLink = std::make_shared<Button>("PaletteLink", rm::loadTexture("Resource/Image/PaletteButtons.png"), sf::IntRect(18, 0, 19, 10));
	buttons.initButton(buttonPaletteLink);
	buttonPaletteRight = std::make_shared<Button>("PaletteRight", rm::loadTexture("Resource/Image/PaletteButtons.png"), sf::IntRect(36, 0, 19, 10));
	buttons.initButton(buttonPaletteRight);

	barMusic = std::make_shared<ButtonBar>("Music", 5);
	initEntity(*barMusic);
	barMusic->selectSegment(saveData[musicVolume]);
	buttons.addButton(barMusic);
	barSound = std::make_shared<ButtonBar>("Sound", 5);
	initEntity(*barSound);
	barSound->selectSegment(saveData[soundVolume]);
	buttons.addButton(barSound);

	initEntity(grid);
	grid.setPosition(GRID_LEFT, getGame()->gameSize.y + 4);

	initEntity(water);

	initEntity(waterBar);

	aquarium = std::make_shared<Aquarium>();
	initEntity(*aquarium);

	shop.aquarium = aquarium;
	initEntity(shop);

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
	soundWater.setBuffer(rm::loadSoundBuffer("Resource/Sound/Water.wav"));
	soundClick.setBuffer(rm::loadSoundBuffer("Resource/Sound/Click.wav"));

	// Load music
	std::string song = "Tide";
	musicBase.openFromFile("Resource/Music/" + song + "0.ogg");
	musicBase.setVolume(100 * std::pow(saveData[musicVolume] / 4.0f, 2));
	musicBase.setLoop(true);

	musicActive.openFromFile("Resource/Music/" + song + "1.ogg");
	musicActive.setVolume(0);
	musicActive.setLoop(true);

	musicBeat.openFromFile("Resource/Music/" + song + "2.ogg");
	musicBeat.setVolume(0);
	musicBeat.setLoop(true);

	musicWarning.openFromFile("Resource/Music/" + song + "3.ogg");
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
					loadLevel(0);
				}
				else if (clickedButton == "Shop") {
					phase = shopping;
					shop.setActive(true);
					aquarium->setActive(true);
					soundSelect.play();
				}
				else if (clickedButton == "Exit") {
					getGame()->exit();
				}
				else if (clickedButton == "Classic") {
					options.digTime = true;
					options.realTime = false;
				}
				else if (clickedButton == "Speedy") {
					options.digTime = false;
					options.realTime = true;
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
							waterBar.drain();
							shells -= 1;
							flashTime = 0;
							shellFlashTime = 1;
							soundShellUse.play();
							soundSelect.play();
						}
						else {
							soundError.play();
						}
					}
				}
				else if (clickedButton == "Menu") {
					// Todo: This maintains score but will generate a new level, seems like holdover from demo code... needs a fix
					goToMenu();
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
				else if (clickedButton.substr(0, 5) == "Music") {
					int value = std::stoi(clickedButton.substr(5));
					saveData[musicVolume] = value;
					soundSelect.play();
					save();
				}
				else if (clickedButton.substr(0, 5) == "Sound") {
					int value = std::stoi(clickedButton.substr(5));
					saveData[soundVolume] = value;
					soundSelect.play();
					save();
				}
				else {
					// Left click on board
					std::string found = grid.digPosition(getGame()->getCursorPosition() - grid.getPosition());
					if (found != "none") {
						int lastWaterLevel = waterBar.waterLevel;

						// Digging on classic mode (except digging jellies) increments the water bar
						if (options.digTime && found != "jelly") {
							if (waterBar.activeBlocks <= 3) {
								soundClick.play();
							}
							waterBar.increment();
						}

						findItem(found);

						if (waterBar.waterLevel > lastWaterLevel) {
							soundWater.play();

							if (waterBar.waterLevel >= 7) {
								flashTime = 0;
								alertFlashTime = 1;
							}
						}

						playDigSound();

						levelTimeTicking = true;
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
			else if (phase == shopping) {
				if (clickedButton == "") {
					clickedButton = shop.clickPosition(getGame()->getCursorPosition());
				}

				if (clickedButton == "Menu") {
					shop.setActive(false);
					aquarium->setActive(false);
					goToMenu();
					soundSelect.play();
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
		else if (event.mouseButton.button == sf::Mouse::Middle) {
			if (phase == playing) {
				// Middle click in game
				bool markResult = grid.markPosition(getGame()->getCursorPosition() - grid.getPosition());
				playDigSound();
			}
		}
	}
	else if (event.type == sf::Event::MouseWheelMoved) {
		if (phase == shopping) {
			shop.desiredScroll -= event.mouseWheel.delta * 26;
		}
	}
	else if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			if (phase == playing || phase == shopping) {
				shop.setActive(false);
				aquarium->setActive(false);
				goToMenu();
			}
			else if (DEBUG_DEMO_MODE && phase == loss) {
				phase = playing;
				loadLevel(0);
				buttonSubmit->text = "Submit";

				score = 0;
				shells = 0;

				goToMenu();
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
			else if (event.key.code == sf::Keyboard::Up) {
				findItem("shell");
			}
			else if (event.key.code == sf::Keyboard::Z) {
				saveData[totalJellies] += sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ? 10 : 1;
				save();
			}

			// Demo keys
			if (event.key.code == sf::Keyboard::O) {
				showTimer = !showTimer;
			}
			else if (event.key.code == sf::Keyboard::P) {
				showVolume = !showVolume;
			}
		}
	}
}

void PlayState::update(sf::Time elapsed) {
	// Update palette
	cm::updatePalette(elapsed);

	// Update timers
	if (phase == playing && levelTimeTicking) {
		float lastLevelTime = levelTime;
		levelTime += elapsed.asSeconds();
		if (options.realTime && std::floor(levelTime) > lastLevelTime) {
			int lastWaterLevel = waterBar.waterLevel;

			waterBar.increment();

			if (waterBar.waterLevel > lastWaterLevel) {
				soundWater.play();

				if (waterBar.waterLevel >= 7) {
					flashTime = 0;
					alertFlashTime = 1;
				}
			}
		}
	}
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
	shellMarkerFlashTime -= elapsed.asSeconds();
	if (shellMarkerFlashTime < 0) {
		shellMarkerFlashTime = 0;
	}
	flagFlashTime -= elapsed.asSeconds();
	if (flagFlashTime < 0) {
		flagFlashTime = 0;
	}
	alertFlashTime -= elapsed.asSeconds();
	if (alertFlashTime < 0) {
		alertFlashTime = 0;
	}

	// Update camera position
	float desiredY = 0;
	if (phase == menu) {
		desiredY = -25;
	}
	approachNumber(cameraY, desiredY, elapsed.asSeconds() * 2);

	// Update main menu pane
	desiredY = -135;
	if (phase == menu) {
		desiredY = 0;
	}
	approachNumber(menuPaneY, desiredY, elapsed.asSeconds() * 5);

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

	const bool inGame = (phase == playing || phase == submitting || phase == results || phase == loss);

	// Update shop
	shop.update(elapsed);

	// Update aquarium
	aquarium->update(elapsed);

	// Update grid
	desiredY = getGame()->gameSize.y + 4;
	if (inGame) {
		desiredY = getGame()->gameSize.y - 104;
	}
	grid.move((sf::Vector2f(GRID_LEFT, desiredY) - grid.getPosition()) * elapsed.asSeconds() * 5.0f);
	grid.xray = DEBUG_ENABLED && sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
	grid.update(elapsed);

	// Update panes
	float desiredX = -71;
	if (inGame) {
		desiredX = 0;
	}
	leftPane.move((sf::Vector2f(desiredX, 0) - leftPane.getPosition()) * elapsed.asSeconds() * 5.0f);
	leftPane.setColor(cm::getUIColor());

	paletteSelect.setPosition(leftPane.getPosition() + sf::Vector2f(2, PALETTE_SELECT_TOP));
	paletteSelect.setColor(cm::getUIColor());

	desiredX = getGame()->gameSize.x;
	if (inGame) {
		desiredX = getGame()->gameSize.x - 71;
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
		saveData[totalJellies] += score;
		save();
	}

	// Update buttons
	bool buttonsActive = phase != submitting && phase != results && phase != loss;
	buttonStart->setPosition(93, menuPaneY + 81);
	buttonClassic->setPosition(20, menuPaneY + 81);
	buttonSpeedy->setPosition(20, menuPaneY + 97);
	buttonShop->setPosition(93, menuPaneY + 97);
	buttonExit->setPosition(93, menuPaneY + (DEBUG_DEMO_MODE ? 2000 : 113));
	buttonSubmit->setPosition(leftPane.getPosition() + sf::Vector2f(2, 88));
	buttonSubmit->enabled = phase != submitting;
	buttonMenu->setPosition(leftPane.getPosition() + sf::Vector2f(2, 117));
	buttonMenu->enabled = buttonsActive;
	buttonSettings->setPosition(leftPane.getPosition() + sf::Vector2f(45, 117));
	buttonSettings->enabled = buttonsActive;
	buttonShellA->setPosition(rightPane.getPosition() + sf::Vector2f(31, 99));
	buttonShellA->enabled = shells > 0 && buttonsActive;
	buttonShellB->setPosition(rightPane.getPosition() + sf::Vector2f(31, 117));
	buttonShellB->enabled = false;
	
	float paletteSelectLeft = saveData[shopPaletteSelect] ? 2 : -1000;
	bool atLeftEnd = cm::getCurrentPalette() == 0;
	bool atRightEnd = cm::getCurrentPalette() >= cm::getTotalPalettes() - 1 || (!PALETTE_SELECT_DEBUG_ENABLED && cm::getCurrentPalette() >= saveData[unlockedPalettes] - 1);
	buttonPaletteLeft->setPosition(leftPane.getPosition() + sf::Vector2f(paletteSelectLeft, PALETTE_BUTTONS_TOP));
	buttonPaletteLeft->enabled = !atLeftEnd && buttonsActive;
	buttonPaletteLink->setPosition(leftPane.getPosition() + sf::Vector2f(paletteSelectLeft + 18, PALETTE_BUTTONS_TOP));
	buttonPaletteLink->enabled = buttonsActive && saveData[selectedPalette] != -1;
	buttonPaletteRight->setPosition(leftPane.getPosition() + sf::Vector2f(paletteSelectLeft + 36, PALETTE_BUTTONS_TOP));
	buttonPaletteRight->enabled = !atRightEnd && buttonsActive;

	barMusic->setPosition(rightPane.getPosition() + sf::Vector2f(showVolume ? 25 : 1000, 80));
	barMusic->enabled = buttonsActive;
	barSound->setPosition(rightPane.getPosition() + sf::Vector2f(showVolume ? 25 : 1000, 92));
	barSound->enabled = buttonsActive;
	buttons.update(elapsed);

	// Update water
	water.masterDepth += (waterBar.waterLevel * 10 - water.masterDepth) * elapsed.asSeconds() * 2;
	water.update(elapsed);
	water.setPosition(grid.getPosition() + sf::Vector2f(0, 100));

	// Update music
	float musicVolumeModifier = std::pow(saveData[musicVolume] / 4.0f, 2);
	float volumeBase = 100 * musicVolumeModifier;
	float volumeActive = 0;
	float volumeBeat = 0;
	float volumeWarning = 0;
	if (phase == playing && musicVolumeModifier != 0) {
		if (waterBar.waterLevel >= 3) {
			volumeActive = 100 * musicVolumeModifier;
		}
		else if (waterBar.waterLevel >= 2) {
			volumeActive = 50 * musicVolumeModifier;
		}
		else if (waterBar.waterLevel >= 1) {
			volumeActive = 25 * musicVolumeModifier;
		}

		if (waterBar.waterLevel >= 6) {
			volumeBeat = 100 * musicVolumeModifier;
		}
		else if (waterBar.waterLevel >= 5) {
			volumeBeat = 50 * musicVolumeModifier;
		}
		else if (waterBar.waterLevel >= 4) {
			volumeBeat = 25 * musicVolumeModifier;
		}

		if (waterBar.waterLevel >= 9) {
			volumeWarning = 100 * musicVolumeModifier;
		}
		else if (waterBar.waterLevel >= 8) {
			volumeWarning = 50 * musicVolumeModifier;
		}
		else if (waterBar.waterLevel >= 7) {
			volumeWarning = 25 * musicVolumeModifier;
		}
	}
	adjustMusicVolume(musicBase, volumeBase, elapsed.asSeconds() * 3);
	adjustMusicVolume(musicActive, volumeActive, elapsed.asSeconds() * 3);
	adjustMusicVolume(musicBeat, volumeBeat, elapsed.asSeconds() * 3);
	adjustMusicVolume(musicWarning, volumeWarning, elapsed.asSeconds() * 3);

	// Update sound effect volumes
	float soundVolumeModifier = std::pow(saveData[soundVolume] / 4.0f, 2);
	soundDig.setVolume(100 * soundVolumeModifier);
	soundFlag.setVolume(100 * soundVolumeModifier);
	soundUnflag.setVolume(100 * soundVolumeModifier);
	soundShellFind.setVolume(100 * soundVolumeModifier);
	soundShellUse.setVolume(100 * soundVolumeModifier);
	soundSelect.setVolume(100 * soundVolumeModifier);
	soundError.setVolume(100 * soundVolumeModifier);
	soundJelly.setVolume(100 * soundVolumeModifier);
	soundScore.setVolume(100 * soundVolumeModifier);
	soundStart.setVolume(100 * soundVolumeModifier);
	soundWater.setVolume(100 * soundVolumeModifier);
	soundClick.setVolume(100 * soundVolumeModifier);

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
	sf::Sprite leftDetails(rm::loadTexture("Resource/Image/JellyDetails.png"), sf::IntRect(0, 0, 60, 135));
	leftDetails.setColor(cm::getDetailUIColor());
	leftDetails.setPosition(leftPane.getPosition());
	window.draw(leftDetails);
	
	if (saveData[shopPaletteSelect]) {
		//window.draw(paletteSelect);
	}
	ra::renderJelly(window, sf::RenderStates::Default, leftPane.getPosition() + sf::Vector2f(3, 33));
	ra::renderFlag(window, sf::RenderStates::Default, leftPane.getPosition() + sf::Vector2f(4, 50));
	ra::renderShell(window, sf::RenderStates::Default, leftPane.getPosition() + sf::Vector2f(3, 65));

	window.draw(rightPane);
	sf::Sprite rightDetails(rm::loadTexture("Resource/Image/JellyDetails.png"), sf::IntRect(60, 0, 60, 135));
	rightDetails.setColor(cm::getDetailUIColor());
	rightDetails.setPosition(rightPane.getPosition() + sf::Vector2f(11, 0));
	window.draw(rightDetails);

	// Render section title
	text.setText(levelName);
	text.setPosition(leftPane.getPosition() + sf::Vector2f(35 - text.getWidth() / 2, 5));
	window.draw(text);

	// Render right title, unsure what to use it for yet
	text.setText("stats");
	text.setPosition(rightPane.getPosition() + sf::Vector2f(36 - text.getWidth() / 2, 5));
	window.draw(text);

	// Jelly count
	text.setText(std::to_string(jellies));
	text.setPosition(leftPane.getPosition() + sf::Vector2f(28 - text.getWidth() / 2, 34));
	window.draw(text);
	text.setText("/");
	text.setColor(cm::getDisabledTextColor());
	text.setPosition(leftPane.getPosition() + sf::Vector2f(36, 34));
	window.draw(text);
	text.setText(std::to_string(levelJellyCount));
	text.setPosition(leftPane.getPosition() + sf::Vector2f(48 - text.getWidth() / 2, 34));
	window.draw(text);

	// Flag count
	text.setText(std::to_string(flags));
	text.setPosition(leftPane.getPosition() + sf::Vector2f(28 - text.getWidth() / 2, 51));
	if (flagFlashTime > 0 && flashTime < 0.1) {
		text.setColor(cm::getFlashColor());
	}
	else {
		text.setColor(cm::getTextColor());
	}
	window.draw(text);
	text.setText("/");
	text.setColor(cm::getDisabledTextColor());
	text.setPosition(leftPane.getPosition() + sf::Vector2f(36, 51));
	window.draw(text);
	text.setText(std::to_string(levelFlagCount));
	text.setPosition(leftPane.getPosition() + sf::Vector2f(48 - text.getWidth() / 2, 51));
	window.draw(text);

	// Shell markers for this level
	for (int shellMarkerIndex = 0; shellMarkerIndex < 3; shellMarkerIndex++) {
		text.setText("*");
		text.setPosition(leftPane.getPosition() + sf::Vector2f(23 + 8 * shellMarkerIndex, 66));
		text.setColor(cm::getDisabledTextColor());
		if (shellsFound > shellMarkerIndex) {
			text.setText("#");
			text.move(-1, 0);
			text.setColor(cm::getTextColor());
			if (shellsFound == shellMarkerIndex + 1 && shellMarkerFlashTime > 0 && flashTime < 0.1) {
				text.setColor(cm::getFlashColor());
			}
		}
		window.draw(text);
	}

	// Render level time
	if (showTimer) {
		//sf::Vector2f timerPosition = leftPane.getPosition() + sf::Vector2f(2, 60);
		sf::Vector2f timerPosition = rightPane.getPosition() + sf::Vector2f(14, 107);
		sf::Vector2f timerTextRight = timerPosition + sf::Vector2f(51, 2);
		std::string timeMinutes = std::to_string((int)(levelTime / 60));
		if (timeMinutes.size() == 1) {
			timeMinutes = "0" + timeMinutes;
		}
		std::string timeSeconds = std::to_string((int)(levelTime) % 60);
		if (timeSeconds.size() == 1) {
			timeSeconds = "0" + timeSeconds;
		}
		std::string timeCentiseconds = std::to_string((int)(levelTime * 100) % 100);
		if (timeCentiseconds.size() == 1) {
			timeCentiseconds = "0" + timeCentiseconds;
		}
		ra::renderUIBox(window, sf::RenderStates::Default, timerPosition, 55, 1);
		text.monowidth = true;
		text.setText(timeMinutes + ":");
		text.setPosition(timerTextRight - sf::Vector2f(31 + text.getWidth(), 0));
		text.setColor(cm::getTextColor());
		window.draw(text);

		text.setText(timeSeconds + ":");
		text.setPosition(timerTextRight - sf::Vector2f(14 + text.getWidth(), 0));
		window.draw(text);

		text.setText(timeCentiseconds);
		text.setPosition(timerTextRight - sf::Vector2f(text.getWidth(), 0));
		window.draw(text);
		text.monowidth = false;
	}

	// Current run score
	text.setText("$" + std::to_string(std::clamp(score, 0, 9999)));
	text.setPosition(rightPane.getPosition() + sf::Vector2f(50 - text.getWidth() / 2, 43));
	if (scoreFlashTime > 0 && flashTime < 0.1) {
		text.setColor(cm::getFlashColor());
	}
	else {
		text.setColor(score == 0 ? cm::getDisabledTextColor() : cm::getTextColor());
	}
	window.draw(text);

	// Best score
	text.setText("$" + std::to_string(std::clamp(saveData[best], 0, 9999)));
	text.setPosition(rightPane.getPosition() + sf::Vector2f(50 - text.getWidth() / 2, 57));
	if (bestFlashTime > 0 && flashTime < 0.1) {
		text.setColor(cm::getFlashColor());
	}
	else {
		text.setColor(cm::getDisabledTextColor());
	}
	window.draw(text);

	// Shell count
	text.setText("#" + std::to_string(std::clamp(shells, 0, 99)));
	text.setPosition(rightPane.getPosition() + sf::Vector2f(29 - text.getWidth() / 2, 87));
	if (shellFlashTime > 0 && flashTime < 0.1) {
		text.setColor(cm::getFlashColor());
	}
	else {
		text.setColor(shells == 0 ? cm::getDisabledTextColor() : cm::getTextColor());
	}
	window.draw(text);

	// Ability costs (work in progress)
	text.setText("#1");
	text.setPosition(rightPane.getPosition() + sf::Vector2f(22 - text.getWidth() / 2, 102));
	text.setColor(cm::getActiveUIElementColor());
	window.draw(text);

	// Palette name
	if (saveData[shopPaletteSelect]) {
		text.setText(cm::getPaletteName(cm::getCurrentPalette()));
		text.setPosition(paletteSelect.getPosition() + sf::Vector2f(27 - text.getWidth() / 2, 16));
		text.setColor(cm::getTextColor());
		window.draw(text);
	}

	window.draw(waterBar);
	window.draw(buttons);

	window.draw(grid);

	window.draw(water);

	// Render warning text
	sf::Color warningColor = cm::getDetailUIColor();
	if (phase == playing && waterBar.waterLevel >= 7 && waterBar.waterLevel <= 9) {
		warningColor = cm::getTextColor();
		if (alertFlashTime > 0 && flashTime < 0.1) {
			warningColor = cm::getFlashColor();
		}
		text.setText("Danger");
		if (waterBar.waterLevel == 8) {
			text.setText("Danger!");
		}
		if (waterBar.waterLevel == 9) {
			text.setText("Danger!!");
			if (options.digTime) {
				if (waterBar.activeBlocks == 1) {
					text.setText("1 dig left!");
				}
				else {
					text.setText(std::to_string(waterBar.activeBlocks) + " digs left!");
				}
			}
		}
		text.setPosition(getGame()->gameSize.x / 2 - text.getWidth() / 2, grid.getPosition().y - 10);
		text.setColor(warningColor);
		window.draw(text);
	}
	else if (phase == loss) {
		warningColor = cm::getTextColor();
	}
	// Render icons
	ra::renderIcon(window, sf::RenderStates::Default, leftPane.getPosition() + sf::Vector2f(59, 20), sf::Vector2f(1, 3), warningColor);
	ra::renderIcon(window, sf::RenderStates::Default, rightPane.getPosition() + sf::Vector2f(2, 20), sf::Vector2f(1, 3), warningColor);

	// Render game over text
	if (phase == loss) {
		text.setText("Game Over");
		text.setPosition(getGame()->gameSize.x / 2 - text.getWidth() / 2, grid.getPosition().y - 10);
		text.setColor(cm::getTextColor());
		window.draw(text);
	}

	// Render the aquarium
	window.draw(*aquarium);

	// Render jelly shop
	window.draw(shop);
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
	saveData[totalJellies] = 0;
	saveData[best] = 0;
	saveData[selectedPalette] = -1;
	saveData[unlockedPalettes] = 1;
	saveData[musicVolume] = 2;
	saveData[soundVolume] = 2;

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

std::shared_ptr<Button> PlayState::createButton(std::string buttonText) {
	std::shared_ptr<Button> output = std::make_shared<Button>(buttonText);
	buttons.initButton(output);
	return output;
}

void PlayState::approachNumber(float &input, float desired, float factor) {
	input += (desired - input) * factor;
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
	if (NEW_MODE && jellies > 20) {
		jellies = 20;
	}
	flags = jellies + extraFlags;

	int waterBlocks = 10;
	if (NEW_MODE) {
		waterBlocks = 10 - level / 3;
		if (waterBlocks < 5) {
			waterBlocks = 5;
		}
	}
	else {
		waterBlocks = 10 - level / 2;
		if (waterBlocks < 2) {
			waterBlocks = 2;
		}
	}
	waterBar.setMaxBlocks(waterBlocks);
	waterBar.resetSystem();

	// Todo: ensure the jelly count is correct! Generation can fail to place all jellies!
	//grid.generateGrid(jellies, 2 + std::rand() % 3);
	grid.generateGrid(jellies, 3);

	levelJellyCount = jellies;
	levelFlagCount = flags;

	shellsFound = 0;
	
	// Reset time
	levelTime = 0;
	levelTimeTicking = false;
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
			//waterBar.resetSystem(true); // This resets the water block progress on a jelly dig
			getGame()->screenShakeTime = 0.05;
			soundJelly.play();
		}
	}
	else if (item == "shell") {
		shells += 1;
		shellsFound += 1;
		flashTime = 0;
		shellFlashTime = 1;
		shellMarkerFlashTime = 1;
		soundShellFind.play();
	}
}

void PlayState::submit() {
	phase = submitting;
	poppingFlags = false;
	submitTime = SUBMIT_INTERVAL;
}

void PlayState::goToMenu() {
	phase = menu;

	if (DEBUG_DEMO_MODE) {
		score = 0;
		shells = 0;

		saveData[selectedPalette] = -1;
		cm::selectPalette(0);
	}
}

