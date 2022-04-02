#include "PlayState.h"

#include "ResourceManager.h"
#include "ColorManager.h"

void PlayState::init() {
	// Load sprites
	sun.setTexture(rm::loadTexture("Resource/Image/Sun.png"));
	clouds.setTexture(rm::loadTexture("Resource/Image/Clouds.png"));
	dunes.setTexture(rm::loadTexture("Resource/Image/Dunes.png"));
	
	// Load sounds
	clear1.setBuffer(rm::loadSoundBuffer("Resource/Sound/Clear1.wav"));
	clear2.setBuffer(rm::loadSoundBuffer("Resource/Sound/Clear2.wav"));
	clear3.setBuffer(rm::loadSoundBuffer("Resource/Sound/Clear3.wav"));

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
			phase = playing;
			clear1.play();
		}
		else if (event.mouseButton.button == sf::Mouse::Right) {
			phase = menu;
			clear3.play();
		}
	}
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::F11) {
			//setFullscreen(!fullscreen);
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
}

void PlayState::render(sf::RenderWindow &window) {
	sf::RectangleShape sky(sf::Vector2f(getGame()->gameSize));
	sky.setFillColor(cm::getSkyColor());
	window.draw(sky);

	window.draw(sun);
	window.draw(clouds);
	window.draw(dunes);
}

void PlayState::adjustMusicVolume(sf::Music &music, float desiredVolume, float factor) {
	float volume = music.getVolume();
	volume += (desiredVolume - volume) * factor;
	music.setVolume(volume);
}
