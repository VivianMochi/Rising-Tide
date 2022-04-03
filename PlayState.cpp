#include "PlayState.h"

#include "ResourceManager.h"
#include "ColorManager.h"
#include "Entity.h"

void PlayState::init() {
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

	initEntity(grid);
	grid.setPosition(GRID_LEFT, getGame()->gameSize.y + 4);

	initEntity(water);

	initEntity(waterBar);

	// Load sprites
	leftPane.setTexture(rm::loadTexture("Resource/Image/LeftPane.png"));
	leftPane.setPosition(-71, 0);
	rightPane.setTexture(rm::loadTexture("Resource/Image/RightPane.png"));
	rightPane.setPosition(getGame()->gameSize.x, 0);

	sun.setTexture(rm::loadTexture("Resource/Image/Sun.png"));
	clouds.setTexture(rm::loadTexture("Resource/Image/Clouds.png"));
	dunes.setTexture(rm::loadTexture("Resource/Image/Dunes.png"));
	camp.setTexture(rm::loadTexture("Resource/Image/Camp.png"));
	
	// Load sounds
	soundDig.setBuffer(rm::loadSoundBuffer("Resource/Sound/Dig.wav"));
	soundClear1.setBuffer(rm::loadSoundBuffer("Resource/Sound/Clear1.wav"));
	soundClear2.setBuffer(rm::loadSoundBuffer("Resource/Sound/Clear2.wav"));
	soundClear3.setBuffer(rm::loadSoundBuffer("Resource/Sound/Clear3.wav"));

	// Load music
	musicBase.openFromFile("Resource/Music/MusicBase.ogg");
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

	musicBase.play();
	musicActive.play();
	musicBeat.play();
	musicWarning.play();
}

void PlayState::gotEvent(sf::Event event) {
	if (event.type == sf::Event::MouseButtonPressed) {
		if (event.mouseButton.button == sf::Mouse::Left) {
			// Check buttons
			std::string clickedButton = buttons.clickPosition(getGame()->getCursorPosition());

			if (phase == menu) {
				if (clickedButton == "Start") {
					soundClear1.play();
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

				}
				else if (clickedButton == "Drain") {
					if (shells > 0) {
						if (waterBar.waterLevel > 1) {
							shells -= 1;
							flashTime = 0;
							shellFlashTime = 1;
						}
						waterBar.drain();
					}
				}
				else if (clickedButton == "Menu") {
					phase = menu;
				}
				else {
					// Left click on board
					std::string found = grid.digPosition(getGame()->getCursorPosition() - grid.getPosition());
					if (found != "none") {
						waterBar.increment();

						if (found == "jelly") {
							waterBar.flood(3);
						}
						else if (found == "shell") {
							shells += 1;
						}

						soundDig.setPitch(0.8 + std::rand() % 40 / 100.0f);
						soundDig.play();
					}
				}
			}
		}
		else if (event.mouseButton.button == sf::Mouse::Right) {
			if (phase == playing) {
				// Right click in game
				int flagResult = grid.flagPosition(getGame()->getCursorPosition() - grid.getPosition(), flags == 0);
				flags -= flagResult;
				if (flagResult) {
					soundClear1.play();
				}
			}
			
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Escape) {
			phase = menu;
		}

		// DEBUG
		if (event.key.code == sf::Keyboard::Num1) {
			loadLevel(level);
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

void PlayState::update(sf::Time elapsed) {
	// Update palette
	cm::updatePalette(elapsed);

	// Update timers
	flashTime += elapsed.asSeconds();
	if (flashTime >= 0.2) {
		flashTime = 0;
	}
	shellFlashTime -= elapsed.asSeconds();
	if (shellFlashTime < 0) {
		shellFlashTime = 0;
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

	desiredX = getGame()->gameSize.x - 71;
	if (phase == menu) {
		desiredX = getGame()->gameSize.x;
	}
	rightPane.move((sf::Vector2f(desiredX, 0) - rightPane.getPosition()) * elapsed.asSeconds() * 5.0f);

	// Update water bar
	waterBar.update(elapsed);
	waterBar.setPosition(rightPane.getPosition());

	// Update buttons
	buttonStart->setPosition(getGame()->gameSize.x / 2 - 55 / 2, menuPaneY + 87);
	buttonExit->setPosition(getGame()->gameSize.x / 2 - 55 / 2, menuPaneY + 105);
	buttonSubmit->setPosition(leftPane.getPosition() + sf::Vector2f(2, 44));
	buttonMenu->setPosition(leftPane.getPosition() + sf::Vector2f(2, 119));
	buttonShell->setPosition(rightPane.getPosition() + sf::Vector2f(14, 44));
	buttonShell->enabled = shells > 0;
	buttons.update(elapsed);

	// Update water
	water.masterDepth = waterBar.waterLevel * 10;
	water.update(elapsed);
	water.setPosition(grid.getPosition() + sf::Vector2f(0, 100));

	// Update music
	float volumeActive = 0;
	float volumeBeat = 0;
	float volumeWarning = 0;
	if (phase == playing) {
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
	adjustMusicVolume(musicActive, volumeActive, elapsed.asSeconds());
	adjustMusicVolume(musicBeat, volumeBeat, elapsed.asSeconds());
	adjustMusicVolume(musicWarning, volumeWarning, elapsed.asSeconds());

	// Update background position
	sun.setPosition(141, -1 - cameraY * 0.9);
	clouds.setPosition(0, -25 - cameraY * 0.9);
	dunes.setPosition(0, -25 - cameraY);
	camp.setPosition(0, -4 -cameraY);
}

void PlayState::render(sf::RenderWindow &window) {
	sf::RectangleShape sky(sf::Vector2f(getGame()->gameSize));
	sky.setFillColor(cm::getSkyColor());
	window.draw(sky);

	window.draw(sun);
	window.draw(clouds);
	window.draw(dunes);
	//window.draw(camp);

	BitmapText text;
	text.setTexture(rm::loadTexture("Resource/Image/Font.png"));
	text.setColor(cm::getTextColor());

	// Render panes
	window.draw(leftPane);
	window.draw(rightPane);

	// Render section title
	text.setText(levelName);
	text.setPosition(leftPane.getPosition() + sf::Vector2f(36 - text.getWidth() / 2, 4));
	window.draw(text);

	// Jelly and flag counts
	text.setText(std::to_string(jellies));
	text.setPosition(leftPane.getPosition() + sf::Vector2f(20, 18));
	window.draw(text);
	text.setText(std::to_string(flags));
	text.setPosition(leftPane.getPosition() + sf::Vector2f(20, 32));
	window.draw(text);

	// Score counts
	text.setText(std::to_string(best));
	text.setPosition(rightPane.getPosition() + sf::Vector2f(40, 4));
	window.draw(text);
	text.setText(std::to_string(score));
	text.setPosition(rightPane.getPosition() + sf::Vector2f(32, 18));
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

	window.draw(waterBar);
	window.draw(buttons);

	window.draw(grid);

	window.draw(water);
}

void PlayState::initEntity(Entity &entity) {
	entity.setState(this);
	entity.init();
}

void PlayState::adjustMusicVolume(sf::Music &music, float desiredVolume, float factor) {
	float volume = music.getVolume();
	volume += (desiredVolume - volume) * factor;
	music.setVolume(volume);
}

void PlayState::loadLevel(int level) {
	this->level = level;

	cm::selectPalette(level);

	levelName = std::string("Section ") + char('A' + level);

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
