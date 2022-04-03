#include "PlayState.h"

#include "ResourceManager.h"
#include "ColorManager.h"
#include "Entity.h"

void PlayState::init() {
	initEntity(grid);
	grid.setPosition(GRID_LEFT, getGame()->gameSize.y + 4);

	initEntity(water);

	// Load sprites
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
				soundClear1.play();
			}
			phase = playing;
			if (grid.digPosition(getGame()->getCursorPosition() - grid.getPosition())) {
				soundDig.setPitch(0.8 + std::rand() % 40 / 100.0f);
				soundDig.play();
			}
		}
		else if (event.mouseButton.button == sf::Mouse::Right) {
			if (phase == playing) {
				if (grid.flagPosition(getGame()->getCursorPosition() - grid.getPosition(), false)) {
					soundClear1.play();
				}
			}
			
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		// DEBUG
		if (event.key.code == sf::Keyboard::Num1) {
			grid.generateGrid(10, 1);
		}
		else if (event.key.code == sf::Keyboard::Up) {
			water.masterDepth += 10;
		}
		else if (event.key.code == sf::Keyboard::Down) {
			water.masterDepth -= 10;
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
