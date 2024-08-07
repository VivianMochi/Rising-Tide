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

	// Ensure we set to fullscreen if needed
	if (!saveData[settingWindowed]) {
		adjustSetting(settingWindowed, 0);
	}
	// Also do the cursor
	if (!saveData[settingCursor]) {
		adjustSetting(settingCursor, 0);
	}

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

	buttonShellA = std::make_shared<Button>("Drain", rm::loadTexture("Resource/Image/Button38.png"), sf::IntRect(0, 0, 38, 16));
	buttonShellA->showText = true;
	buttons.initButton(buttonShellA);

	buttonShellB = std::make_shared<Button>("$", rm::loadTexture("Resource/Image/Button38.png"), sf::IntRect(0, 0, 38, 16));
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

	initEntity(grid);
	grid.setPosition(GRID_LEFT, getGame()->gameSize.y + 4);
	grid.water = &water;

	water = Water(100, 140);
	initEntity(water);

	initEntity(waterBar);

	aquarium = std::make_shared<Aquarium>();
	initEntity(*aquarium);

	shop.aquarium = aquarium;
	initEntity(shop);

	initEntity(settings);

	// Load sprites
	leftPane.setTexture(rm::loadTexture("Resource/Image/LeftPane.png"));
	leftPane.setPosition(-71, 0);
	rightPane.setTexture(rm::loadTexture("Resource/Image/RightPane.png"));
	rightPane.setPosition(getGame()->gameSize.x, 0);
	paletteSelect.setTexture(rm::loadTexture("Resource/Image/PalettePane.png"));
	paletteSelect.setPosition(leftPane.getPosition() + sf::Vector2f(2, PALETTE_SELECT_TOP));
	resultsPane.setTexture(rm::loadTexture("Resource/Image/ResultsPane.png"));
	resultsPane.setPosition(-60, 17);

	splashLogo.setTexture(rm::loadTexture("Resource/Image/SplashLogo.png"));
	splashLogo.setPosition(sf::Vector2f(getGame()->gameSize / 2) + sf::Vector2f(-44, -9));

	initEntity(title);
	title.setPosition(0, 135);

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
	soundShopJingle.setBuffer(rm::loadSoundBuffer("Resource/Sound/ShopJingle.wav"));
	soundPerfect.setBuffer(rm::loadSoundBuffer("Resource/Sound/Perfect.wav"));
	soundDeposit.setBuffer(rm::loadSoundBuffer("Resource/Sound/Deposit.wav"));
	soundDepositEnd.setBuffer(rm::loadSoundBuffer("Resource/Sound/DepositEnd.wav"));
	soundDepositEndBest.setBuffer(rm::loadSoundBuffer("Resource/Sound/DepositEndBest.wav"));

	// Load music
	std::string song = "Tide";
	musicBase.openFromFile("Resource/Music/" + song + "0.ogg");
	musicBase.setLoop(true);
	musicActive.openFromFile("Resource/Music/" + song + "1.ogg");
	musicActive.setLoop(true);
	musicBeat.openFromFile("Resource/Music/" + song + "2.ogg");
	musicBeat.setLoop(true);
	musicWarning.openFromFile("Resource/Music/" + song + "3.ogg");
	musicWarning.setLoop(true);
	musicShop.openFromFile("Resource/Music/Shop.ogg");
	musicShop.setLoop(true);
	musicAmbience.openFromFile("Resource/Music/OceanAmbience.ogg");
	musicAmbience.setVolume(0);
	musicAmbience.setLoop(true);

	// Start ambience
	musicAmbience.play();
}

void PlayState::gotEvent(sf::Event event) {
	if (event.type == sf::Event::MouseButtonPressed) {
		// Any mouse press cancels the ongoing action
		heldAction = noAction;

		if (event.mouseButton.button == sf::Mouse::Left) {
			// Sequence skipping
			if (phase == fade) {
				startIntro();
				introTime = 1.3;
			}
			else if (phase == intro) {
				if (introTime > 1.7) {
					introTime = 1.3;
				}
				else {
					endIntro();
					cameraY = -25;
					menuPaneY = 0;
				}
			}

			// Check buttons
			std::string clickedButton = buttons.clickPosition(getGame()->getCursorPosition());

			if (settingsOpen) {
				// Settings pane takes priority over all
				clickedButton = settings.clickPosition(getGame()->getCursorPosition());
				if (clickedButton == "Settings") {
					toggleSettingsTab();
					soundSelect.play();
				}
				else if (clickedButton.substr(0, 8) == "setting-") {
					if (clickedButton.substr(0, 13) == "setting-music") {
						// Adjusting music volume
						saveData[settingMusic] = std::stoi(clickedButton.substr(13));
						save();
					}
					else if (clickedButton.substr(0, 13) == "setting-sound") {
						// Adjusting sound volume
						saveData[settingSound] = std::stoi(clickedButton.substr(13));
						save();
					}
					else {
						// Adjusting a saveData entry directly
						if (saveData[clickedButton] == 0) {
							saveData[clickedButton] = 1;
							save();
						}
						else {
							saveData[clickedButton] = 0;
							save();
						}
						adjustSetting(clickedButton, saveData[clickedButton]);
					}
					soundSelect.play();
				}
				else if (getGame()->getCursorPosition().y < 81) {
					toggleSettingsTab();
				}
			}
			else if (phase == menu) {
				if (clickedButton == "Start") {
					soundStart.play();
					phase = playing;
					loadLevel(0);
				}
				else if (clickedButton == "Shop") {
					phase = shopping;
					shop.setActive(true);
					aquarium->setActive(true);
					soundShopJingle.play();
					musicShop.stop();
					shopMusicDelayTime = 1.71;
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
				else if (clickedButton == "Settings") {
					toggleSettingsTab();
					soundSelect.play();
				}
			}
			else if (phase == playing) {
				if (clickedButton == "Submit") {
					// Only allow submission if a flag has been placed, to protect from accidental press
					// Todo: This isn't intuitive and could lead to confusion
					if (flags < levelFlagCount) {
						submit();
						soundSelect.play();
					}
					else {
						soundError.play();
					}
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
							water.turbulence = 1;
						}
						else {
							soundError.play();
						}
					}
				}
				else if (clickedButton == "Menu") {
					// Todo: This maintains score but will generate a new level, seems like holdover from demo code... needs a fix
					// Should probably keep as-is, to have a resume button on main menu
					goToMenu();
					soundSelect.play();
				}
				else if (clickedButton == "Settings") {
					toggleSettingsTab();
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
				else {
					// Left click on board
					doDigLogic();
					if (heldAction == noAction) {
						heldAction = digAction;
						actionStartGridSquare = getGridSquareFromCursorPosition();
						actionLastGridSquare = actionStartGridSquare;
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
				else if (clickedButton == "Settings") {
					toggleSettingsTab();
					soundSelect.play();
				}
			}
		}
		else if (event.mouseButton.button == sf::Mouse::Right) {
			if (settingsOpen) {
				toggleSettingsTab();
			}
			else if (phase == playing) {
				// Right click in game
				bool flagResult = doFlagLogic();
				if (heldAction == noAction) {
					heldAction = flagAction;
					actionStartGridSquare = getGridSquareFromCursorPosition();
					actionLastGridSquare = actionStartGridSquare;
					heldActionIsRemoval = !flagResult;
				}
			}
		}
		else if (event.mouseButton.button == sf::Mouse::Middle) {
			if (settingsOpen) {
				toggleSettingsTab();
			}
			else if (phase == playing) {
				// Middle click in game
				bool markResult = doMarkLogic();
				if (heldAction == noAction) {
					heldAction = markAction;
					actionStartGridSquare = getGridSquareFromCursorPosition();
					actionLastGridSquare = actionStartGridSquare;
					heldActionIsRemoval = !markResult;
				}
			}
		}
	}
	else if (event.type == sf::Event::MouseButtonReleased) {
		// Ensure any held actions are cleared
		if (event.mouseButton.button == sf::Mouse::Left && heldAction == digAction) {
			heldAction = noAction;
		}
		else if (event.mouseButton.button == sf::Mouse::Right && heldAction == flagAction) {
			heldAction = noAction;
		}
		else if (event.mouseButton.button == sf::Mouse::Middle && heldAction == markAction) {
			heldAction = noAction;
		}
	}
	else if (event.type == sf::Event::MouseWheelMoved) {
		if (phase == shopping) {
			shop.desiredScroll -= event.mouseWheel.delta * 26;
		}
	}
	else if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			if (settingsOpen) {
				toggleSettingsTab();
			}
			else if (phase == playing || phase == shopping) {
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
		}
	}
}

void PlayState::update(sf::Time elapsed) {
	// Update palette
	cm::updatePalette(elapsed);

	// Do active held actions
	if (phase == playing && heldAction != noAction) {
		sf::Vector2i thisGridSquare = getGridSquareFromCursorPosition();
		if (thisGridSquare != actionLastGridSquare) {
			if (heldAction == digAction) {
				doDigLogic();
			}
			else if (heldAction == flagAction) {
				doFlagLogic(false);
			}
			else if (heldAction == markAction) {
				doMarkLogic(false);
			}
			actionLastGridSquare = thisGridSquare;
		}
	}

	// Update timers
	if (phase == playing && levelTimeTicking) {
		float lastLevelTime = levelTime;
		levelTime += elapsed.asSeconds();
		if (options.realTime && std::floor(levelTime) > lastLevelTime) {
			int lastWaterLevel = waterBar.waterLevel;

			waterBar.increment();

			// Logic for water rising, copied from bottom
			if (waterBar.waterLevel > lastWaterLevel) {
				soundWater.play();

				if (waterBar.waterLevel >= 7) {
					flashTime = 0;
					alertFlashTime = 1;
				}

				// Make turbulence
				water.turbulence = 1;
			}
		}
	}
	if (phase == fade) {
		introTime -= elapsed.asSeconds() * 0.5;
		if (introTime <= 0) {
			startIntro();
		}
	}
	else if (phase == intro) {
		if (introTime > 1) {
			// First section, splash
			introTime -= elapsed.asSeconds() * 0.3;
		}
		else if (introTime > 0) {
			// Panning down to menu
			introTime -= elapsed.asSeconds() * 0.5;
		}
		else {
			endIntro();
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
	shopMusicDelayTime -= elapsed.asSeconds();
	if (shopMusicDelayTime < 0) {
		shopMusicDelayTime = 0;
	}

	// Update camera position
	float desiredY = 0;
	if (phase == fade) {
		desiredY = -135;
	}
	else if (phase == intro) {
		if (introTime > 1) {
			desiredY = -135;
		}
		else {
			desiredY = -135 * introTime + -25 * (1 - introTime);
		}
	}
	else if (phase == menu) {
		desiredY = -25;
	}
	approachNumber(cameraY, desiredY, elapsed.asSeconds() * 2);

	// Update main menu pane
	desiredY = -135;
	if (phase == fade) {
		desiredY = 135;
	}
	else if (phase == intro) {
		desiredY = std::min(135 * introTime, 135.0f);
	}
	else if (phase == menu) {
		desiredY = 0;
	}
	approachNumber(menuPaneY, desiredY, elapsed.asSeconds() * 5);

	// Do submission
	if (phase == submitting) {
		submitTime -= elapsed.asSeconds();
		if (submitTime <= 0) {
			if (submitPhase == revealingBoard) {
				std::string popped = grid.popSquare(poppingFlags);

				findItem(popped, poppingFlags);
				if (popped == "none") {
					if (!poppingFlags) {
						poppingFlags = true;
						submitTime = SUBMIT_INTERVAL_BREAK;
						resultsLines[0] = "$0/" + std::to_string(grid.totalJellies);
					}
					else {
						submitPhase = gradeResult;
						submitTime = SUBMIT_RESULTS_INTERVAL;

						// Do grade text
						if (jelliesScored == grid.totalJellies) {
							resultsLines[1] = "Perfect";
							//soundPerfect.play();

							// Perfect bonus can come later, classic mode has no bonus
							jellyMultiplier += 1.0;
							//bonusJellies = 10;
						}
						else if (jelliesScored >= std::floor(grid.totalJellies / 2.0)) {
							resultsLines[1] = "Nice";
						}
						else if (jelliesScored > 0) {
							resultsLines[1] = "Ok";
						}
						else {
							// Todo: this should probably be a fail state
							resultsLines[1] = "...";
						}
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
			else if (submitPhase == gradeResult) {
				submitPhase = multiplierResult;
				submitTime = SUBMIT_RESULTS_INTERVAL;

				// Calculate multiplier here

				// Skip if multiplier is 1.0
				if (jellyMultiplier == 1.0) {
					submitTime = 0;;
				}
				else {
					// Otherwise update display
					int whole = std::trunc(jellyMultiplier);
					float decimal = jellyMultiplier - whole;
					resultsLines[2] = "^" + std::to_string(whole) + "." + std::to_string(decimal).substr(2, 1);
				}
			}
			else if (submitPhase == multiplierResult) {
				submitPhase = totalResult;
				submitTime = SUBMIT_RESULTS_INTERVAL;

				// Calculate final score, always round up!
				finalScore = std::ceil((jelliesScored + bonusJellies) * jellyMultiplier);
				// Display score
				resultsLines[3] = "$" + std::to_string(finalScore);
			}
			else if (submitPhase == totalResult) {
				submitPhase = depositResult;
				submitTime = 0.85;
				soundDeposit.play();

				// Calculate run score, and prep score effect
				finalRunScore = score + finalScore;
				scoreChangeInterval = (0.85 + 0.1) / finalScore;
				scoreChangeTimer = 0;
				flashTime = 0;
				scoreFlashTime = 0.85;
			}
			else if (submitPhase == depositResult) {
				phase = results;
				buttonSubmit->text = "Next";
				soundDeposit.stop();

				// End any score effect and update best score
				score = finalRunScore;
				flashTime = 0;
				scoreFlashTime = 1;

				if (score > saveData[best]) {
					saveData[best] = score;
					bestFlashTime = 1;

					// Save best score
					save();

					soundDepositEndBest.play();
				}
				else {
					soundDepositEnd.play();
				}
			}
		}

		// Do score incrementing effect
		if (phase == submitting && submitPhase == depositResult) {
			scoreChangeTimer -= elapsed.asSeconds();
			if (scoreChangeTimer <= 0 && score < finalRunScore - 1) {
				scoreChangeTimer += scoreChangeInterval;
				score += 1;
			}
		}
	}

	const bool inGame = (phase == playing || phase == submitting || phase == results || phase == loss);

	// Update shop
	shop.update(elapsed);

	// Update aquarium
	aquarium->update(elapsed);

	// Update settings tab
	settings.update(elapsed);
	// Todo: visualize if hovering over water checkbox or palette select
	visualizeWater = settingsOpen;

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

	desiredX = -60;
	if (phase == submitting && poppingFlags || phase == results) {
		desiredX = 2;
	}
	resultsPane.move((sf::Vector2f(desiredX, 17) - resultsPane.getPosition()) * elapsed.asSeconds() * 8.0f);
	resultsPane.setColor(cm::getWaterColor());

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
	bool buttonsActive = phase != submitting && phase != results && phase != loss && settingsOpen == false;
	buttonStart->setPosition(93, menuPaneY + 80);
	buttonClassic->setPosition(-200, menuPaneY + 81);
	buttonSpeedy->setPosition(-200, menuPaneY + 97);
	buttonShop->setPosition(93, menuPaneY + 98);
	buttonExit->setPosition(93, menuPaneY + (DEBUG_DEMO_MODE ? 2000 : 116));
	buttonSubmit->setPosition(leftPane.getPosition() + sf::Vector2f(2, 88));
	buttonSubmit->enabled = phase != submitting && settingsOpen == false;
	buttonMenu->setPosition(leftPane.getPosition() + sf::Vector2f(2, 117));
	buttonMenu->enabled = buttonsActive;
	float settingsX = std::max(leftPane.getPosition().x + 45, 2.0f);
	float settingsY = phase == menu || phase == fade || phase == intro ? std::max(menuPaneY + 117, 117.0f) : 117;
	buttonSettings->setPosition(sf::Vector2f(settingsX, settingsY));
	buttonSettings->enabled = buttonsActive && phase != shopping;
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

	buttons.update(elapsed);

	// Update water
	float desiredWaterDisplayDepth = std::clamp(waterBar.waterLevel * 10, visualizeWater ? 70 : 0, 100) + 20;
	water.masterDepth += (desiredWaterDisplayDepth - water.masterDepth) * elapsed.asSeconds() * 2;
	water.update(elapsed);
	water.setPosition(grid.getPosition() + sf::Vector2f(0, -20));
	
	// Update music
	float musicVolumeModifier = std::pow(saveData[settingMusic] / 9.0f, 2);
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
	if (phase == shopping) {
		adjustMusicVolume(musicBase, 0, elapsed.asSeconds() * 3);
		adjustMusicVolume(musicActive, 0, elapsed.asSeconds() * 3);
		adjustMusicVolume(musicBeat, 0, elapsed.asSeconds() * 3);
		adjustMusicVolume(musicWarning, 0, elapsed.asSeconds() * 3);

		if (shopMusicDelayTime <= 0) {
			if (musicShop.getStatus() == sf::SoundSource::Status::Stopped) {
				musicShop.setVolume(volumeBase);
				musicShop.play();
			}
			adjustMusicVolume(musicShop, volumeBase, elapsed.asSeconds() * 3);
		}
	}
	else {
		adjustMusicVolume(musicBase, volumeBase, elapsed.asSeconds() * 3);
		adjustMusicVolume(musicActive, volumeActive, elapsed.asSeconds() * 3);
		adjustMusicVolume(musicBeat, volumeBeat, elapsed.asSeconds() * 3);
		adjustMusicVolume(musicWarning, volumeWarning, elapsed.asSeconds() * 3);

		adjustMusicVolume(musicShop, 0, elapsed.asSeconds() * 3);
		if (musicShop.getStatus() == sf::SoundSource::Status::Playing && musicShop.getVolume() < 1) {
			musicShop.stop();
		}
	}

	// Update sound effect volumes
	float soundVolumeModifier = std::pow(saveData[settingSound] / 9.0f, 2);
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
	soundShopJingle.setVolume(100 * soundVolumeModifier);
	soundPerfect.setVolume(100 * soundVolumeModifier);
	soundDeposit.setVolume(100 * soundVolumeModifier);
	soundDepositEnd.setVolume(100 * soundVolumeModifier);
	soundDepositEndBest.setVolume(100 * soundVolumeModifier);
	adjustMusicVolume(musicAmbience, phase == fade || phase == intro || phase == menu ? 100 * soundVolumeModifier : 0, elapsed.asSeconds());

	// Update background position
	title.setPosition(getGame()->gameSize.x / 2 - 103 / 2, menuPaneY + 18);
	sun.setPosition(141, std::round(-1 - cameraY * 0.9));
	sun.setColor(cm::getFlashColor());
	clouds.setPosition(0, std::round(-125 - cameraY * 0.9));
	clouds.setColor(cm::getCloudColor());
	dunes.setPosition(0, std::round(-25 - cameraY));
	dunes.setColor(cm::getSandColor());
	ocean.setPosition(0, std::round(40 - cameraY * 1.2));

	// Update title
	title.update(elapsed);

	// Update ocean
	ocean.update(elapsed);

	// Update splash logo
	if (phase == intro) {
		sf::Color logoColor = cm::getJellyColor();
		if (introTime > 1) {
			logoColor.a = std::clamp((1 - (introTime - 1)) * 3, 0.0f, 1.0f) * 255;
		}
		else {
			logoColor.a = std::clamp((introTime - 0.65f) * 3, 0.0f, 1.0f) * 255;
		}
		splashLogo.setColor(logoColor);
	}
}

void PlayState::render(sf::RenderWindow &window) {
	sf::RectangleShape sky(sf::Vector2f(getGame()->gameSize));
	sky.setFillColor(cm::getSkyColor());
	window.draw(sky);

	window.draw(sun);
	window.draw(clouds);
	window.draw(dunes);
	window.draw(ocean);
	window.draw(title);

	if (phase == fade) {
		sf::RectangleShape fadeSprite(sf::Vector2f(getGame()->gameSize));
		fadeSprite.setFillColor(sf::Color(255, 255, 255, introTime * 255));
		window.draw(fadeSprite);
	}
	else if (phase == intro) {
		window.draw(splashLogo);
	}

	BitmapText text;
	text.setTexture(rm::loadTexture("Resource/Image/Font.png"));
	text.setColor(cm::getUIColorDark());

	// Render left pane
	window.draw(leftPane);
	sf::Sprite leftDetails(rm::loadTexture("Resource/Image/JellyDetails.png"), sf::IntRect(0, 0, 60, 135));
	leftDetails.setColor(cm::getUIColorMediumDetail());
	leftDetails.setPosition(leftPane.getPosition());
	window.draw(leftDetails);
	
	if (saveData[shopPaletteSelect]) {
		//window.draw(paletteSelect);
	}
	ra::renderJelly(window, sf::RenderStates::Default, leftPane.getPosition() + sf::Vector2f(3, 33));
	ra::renderFlag(window, sf::RenderStates::Default, leftPane.getPosition() + sf::Vector2f(4, 50));
	ra::renderShell(window, sf::RenderStates::Default, leftPane.getPosition() + sf::Vector2f(3, 65));

	// Render right pane
	window.draw(rightPane);
	sf::Sprite rightDetails(rm::loadTexture("Resource/Image/JellyDetails.png"), sf::IntRect(60, 0, 60, 135));
	rightDetails.setColor(cm::getUIColorMediumDetail());
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
	text.setText(std::to_string(grid.hiddenJellies));
	text.setPosition(leftPane.getPosition() + sf::Vector2f(28 - text.getWidth() / 2, 34));
	window.draw(text);
	text.setText("/");
	text.setColor(cm::getUIColorMedium());
	text.setPosition(leftPane.getPosition() + sf::Vector2f(36, 34));
	window.draw(text);
	text.setText(std::to_string(grid.totalJellies));
	text.setPosition(leftPane.getPosition() + sf::Vector2f(48 - text.getWidth() / 2, 34));
	window.draw(text);

	// Flag count
	text.setText(std::to_string(flags));
	text.setPosition(leftPane.getPosition() + sf::Vector2f(28 - text.getWidth() / 2, 51));
	if (flagFlashTime > 0 && flashTime < 0.1) {
		text.setColor(cm::getFlashColor());
	}
	else {
		text.setColor(cm::getUIColorDark());
	}
	window.draw(text);
	text.setText("/");
	text.setColor(cm::getUIColorMedium());
	text.setPosition(leftPane.getPosition() + sf::Vector2f(36, 51));
	window.draw(text);
	text.setText(std::to_string(levelFlagCount));
	text.setPosition(leftPane.getPosition() + sf::Vector2f(48 - text.getWidth() / 2, 51));
	window.draw(text);

	// Shell markers for this level
	for (int shellMarkerIndex = 0; shellMarkerIndex < 3; shellMarkerIndex++) {
		text.setText("*");
		text.setPosition(leftPane.getPosition() + sf::Vector2f(23 + 8 * shellMarkerIndex, 66));
		text.setColor(cm::getUIColorMedium());
		if (shellsFound > shellMarkerIndex) {
			text.setText("#");
			text.move(-1, 0);
			text.setColor(cm::getUIColorDark());
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
		text.setColor(cm::getUIColorDark());
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
		text.setColor(score == 0 ? cm::getUIColorMedium() : cm::getUIColorDark());
	}
	window.draw(text);

	// Best score
	text.setText("$" + std::to_string(std::clamp(saveData[best], 0, 9999)));
	text.setPosition(rightPane.getPosition() + sf::Vector2f(50 - text.getWidth() / 2, 57));
	if (bestFlashTime > 0 && flashTime < 0.1) {
		text.setColor(cm::getFlashColor());
	}
	else {
		text.setColor(cm::getUIColorMedium());
	}
	window.draw(text);

	// Shell count
	text.setText("#" + std::to_string(std::clamp(shells, 0, 99)));
	text.setPosition(rightPane.getPosition() + sf::Vector2f(29 - text.getWidth() / 2, 87));
	if (shellFlashTime > 0 && flashTime < 0.1) {
		text.setColor(cm::getFlashColor());
	}
	else {
		text.setColor(shells == 0 ? cm::getUIColorMedium() : cm::getUIColorDark());
	}
	window.draw(text);

	// Ability costs (work in progress)
	text.setText("#1");
	text.setPosition(rightPane.getPosition() + sf::Vector2f(22 - text.getWidth() / 2, 102));
	text.setColor(cm::getUIColorBright());
	window.draw(text);

	// Palette name
	if (saveData[shopPaletteSelect]) {
		text.setText(cm::getPaletteName(cm::getCurrentPalette()));
		text.setPosition(paletteSelect.getPosition() + sf::Vector2f(27 - text.getWidth() / 2, 16));
		text.setColor(cm::getUIColorDark());
		window.draw(text);
	}

	window.draw(waterBar);
	window.draw(buttons);

	window.draw(grid);

	window.draw(water);
	grid.renderOverWater(window);

	// Render warning text
	sf::Color warningColor = cm::getUIColorMediumDetail();
	if (phase == playing && waterBar.waterLevel >= 7 && waterBar.waterLevel <= 9) {
		warningColor = cm::getUIColorDark();
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
		warningColor = cm::getUIColorDark();
	}
	// Render icons
	ra::renderIcon(window, sf::RenderStates::Default, leftPane.getPosition() + sf::Vector2f(59, 20), sf::Vector2f(1, 3), warningColor);
	ra::renderIcon(window, sf::RenderStates::Default, rightPane.getPosition() + sf::Vector2f(2, 20), sf::Vector2f(1, 3), warningColor);

	// Render game over text
	if (phase == loss) {
		text.setText("Game Over");
		text.setPosition(getGame()->gameSize.x / 2 - text.getWidth() / 2, grid.getPosition().y - 10);
		text.setColor(cm::getUIColorDark());
		window.draw(text);
	}

	// Render results pane
	window.draw(resultsPane);
	if (resultsPane.getPosition().x > -58) {
		// Render results numbers
		text.setColor(cm::getResultsTextColor(false));
		text.setText(resultsLines[0]);
		text.setPosition(resultsPane.getPosition() + sf::Vector2f(28 - text.getWidth() / 2, 16));
		window.draw(text);
		if (submitPhase >= gradeResult) {
			text.setText(resultsLines[1]);
			text.setPosition(resultsPane.getPosition() + sf::Vector2f(28 - text.getWidth() / 2, 29));
			window.draw(text);
		}
		if (submitPhase >= multiplierResult) {
			text.setText(resultsLines[2]);
			text.setPosition(resultsPane.getPosition() + sf::Vector2f(28 - text.getWidth() / 2, 42));
			window.draw(text);
		}
		if (submitPhase >= totalResult) {
			text.setText(resultsLines[3]);
			text.setPosition(resultsPane.getPosition() + sf::Vector2f(28 - text.getWidth() / 2, 55));
			text.setColor(cm::getResultsTextColor(true));
			window.draw(text);
		}
	}

	// Render the aquarium
	window.draw(*aquarium);

	// Render jelly shop
	window.draw(shop);

	// Render settings tab
	window.draw(settings);
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
	// Initial settings
	saveData[settingWindowed] = 1;
	saveData[settingCursor] = 1;
	saveData[settingShake] = 1;
	saveData[settingMusic] = 9;
	saveData[settingSound] = 9;

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

sf::Vector2i PlayState::getGridSquareFromCursorPosition() {
	sf::Vector2i output;
	output.x = std::floor((getGame()->getCursorPosition().x - grid.getPosition().x) / 10.0f);
	output.y = std::floor((getGame()->getCursorPosition().y - grid.getPosition().y) / 10.0f);
	return output;
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

	// Determine how many jellies and flags to prep
	int jellies = 10 + level;
	if (NEW_MODE && jellies > 20) {
		jellies = 20;
	}
	int extraFlags = 5;
	extraFlags -= level / 2;
	if (extraFlags < 0) {
		extraFlags = 0;
	}
	grid.generateGrid(jellies, 3);
	flags = grid.totalJellies + extraFlags;
	levelFlagCount = flags;
	shellsFound = 0;

	// Set up the water bar
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

	// Reset time
	levelTime = 0;
	levelTimeTicking = false;
}

void PlayState::findItem(std::string item, bool flagged) {
	if (item == "jelly") {
		if (flagged) {
			jelliesScored += 1;
			resultsLines[0] = "$" + std::to_string(jelliesScored) + "/" + std::to_string(grid.totalJellies);
			soundScore.play();
		}
		else {
			waterBar.flood(3);
			water.turbulence = 1;
			if (saveData[settingShake]) {
				getGame()->screenShakeTime = 0.05;
			}
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
	submitPhase = revealingBoard;
	poppingFlags = false;
	submitTime = SUBMIT_INTERVAL;

	// Reset scoring stuff
	jelliesScored = 0;
	bonusJellies = 0;
	jellyMultiplier = 1.0;
	finalScore = 0;
	resultsLines.clear();
	resultsLines.resize(4);
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

void PlayState::toggleSettingsTab() {
	settingsOpen = !settingsOpen;
	settings.open = settingsOpen;
}

void PlayState::adjustSetting(std::string setting, int newValue) {
	//std::cout << "Set " << setting << " to " << std::to_string(newValue) << "\n";
	
	if (setting == settingWindowed) {
		getGame()->setFullscreen(!newValue);
	}
	else if (setting == settingCursor) {
		// Toggle software cursor
		getGame()->customCursor = newValue;
		getGame()->getWindow()->setMouseCursorVisible(!newValue);
	}
	else if (setting == settingShake) {
		if (newValue) {
			getGame()->screenShakeTime = 0.05;
		}
	}
}

void PlayState::startIntro() {
	introTime = 2;
	phase = intro;

	// Set music volumes
	musicBase.setVolume(0);
	musicActive.setVolume(0);
	musicBeat.setVolume(0);
	musicWarning.setVolume(0);

	// Start music
	musicBase.play();
	musicActive.play();
	musicBeat.play();
	musicWarning.play();
}

void PlayState::endIntro() {
	introTime = 0;
	phase = menu;
}

void PlayState::doDigLogic() {
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

		// Logic for water rising
		if (waterBar.waterLevel > lastWaterLevel) {
			soundWater.play();

			if (waterBar.waterLevel >= 7) {
				flashTime = 0;
				alertFlashTime = 1;
			}

			// Make turbulence
			water.turbulence = 1;
		}

		playDigSound();

		levelTimeTicking = true;
	}
}

bool PlayState::doFlagLogic(bool toggle) {
	// Determine rules
	ActionRule rule = toggleState;
	if (!toggle) {
		rule = heldActionIsRemoval ? onlyRemove : onlyAdd;
	}
	if (flags == 0) {
		// If set to onlyAdd but have no remaining flags, cancel logic
		if (rule == onlyAdd) {
			return false;
		}
		// Otherwise just set rule to onlyRemove
		rule = onlyRemove;
	}

	// Do action
	int flagResult = grid.flagPosition(getGame()->getCursorPosition() - grid.getPosition(), rule);
	flags -= flagResult;
	
	// Sounds and effects
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

	// Result
	return flagResult != -1;
}

bool PlayState::doMarkLogic(bool toggle) {
	// Determine rules
	ActionRule rule = toggleState;
	if (!toggle) {
		rule = heldActionIsRemoval ? onlyRemove : onlyAdd;
	}

	// Do action
	int markResult = grid.markPosition(getGame()->getCursorPosition() - grid.getPosition(), rule);

	// Sounds and effects
	if (markResult != 0) {
		playDigSound();
	}

	// Result
	return markResult != -1;
}

