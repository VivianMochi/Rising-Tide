#include "PlayState.h"

#include "ResourceManager.h"
#include "ColorManager.h"
#include "Entity.h"

void PlayState::init() {
	initEntity(grid);
	grid.setPosition(GRID_LEFT, getGame()->gameSize.y + 4);

	initEntity(water);

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
			if (phase == menu) {
				// Left click in menu
				soundClear1.play();
				phase = playing;
				loadLevel(0);
			}
			else if (phase == playing) {
				// Left click in game
				if (grid.digPosition(getGame()->getCursorPosition() - grid.getPosition())) {
					soundDig.setPitch(0.8 + std::rand() % 40 / 100.0f);
					soundDig.play();
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
		// DEBUG
		if (event.key.code == sf::Keyboard::Num1) {
			loadLevel(level);
		}
		else if (event.key.code == sf::Keyboard::Up) {
			water.masterDepth += 10;
		}
		else if (event.key.code == sf::Keyboard::Down) {
			water.masterDepth -= 10;
		}
		else if (event.key.code == sf::Keyboard::Escape) {
			phase = menu;
		}
	}
}

void PlayState::update(sf::Time elapsed) {
	// Update camera position
	float desiredY = 0;
	if (phase == menu) {
		desiredY = -25;
	}
	cameraY += (desiredY - cameraY) * elapsed.asSeconds() * 2;

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

	// Update water
	water.update(elapsed);
	water.setPosition(grid.getPosition() + sf::Vector2f(0, 100));

	// Update music
	float volumeActive = 0;
	float volumeBeat = 0;
	float volumeWarning = 0;
	if (phase == playing) {
		volumeActive = 100;
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

	// Render left pane
	window.draw(leftPane);
	BitmapText text;
	text.setTexture(rm::loadTexture("Resource/Image/Font.png"));
	text.setColor(cm::getTextColor());

	// Render section title
	text.setText(levelName);
	text.setPosition(leftPane.getPosition() + sf::Vector2f(4, 4));
	window.draw(text);

	// Jelly and flag counts
	text.setText(std::to_string(jellies));
	text.setPosition(leftPane.getPosition() + sf::Vector2f(20, 18));
	window.draw(text);
	text.setText(std::to_string(flags));
	text.setPosition(leftPane.getPosition() + sf::Vector2f(20, 32));
	window.draw(text);

	// Render right pane
	window.draw(rightPane);

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

	levelName = std::string("Section ") + char('A' + level);

	int extraFlags = 5;
	extraFlags -= level / 2;
	if (extraFlags < 0) {
		extraFlags = 0;
	}

	jellies = 10 + level;
	flags = jellies + extraFlags;
	water.masterDepth = 0;

	grid.generateGrid(jellies, 1);
}
