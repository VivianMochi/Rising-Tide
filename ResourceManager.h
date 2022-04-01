#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <map>

namespace rm {
	sf::Texture& loadTexture(std::string filename);
	sf::SoundBuffer& loadSoundBuffer(std::string filename);
	void clearCache();
};
