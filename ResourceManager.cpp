#include "ResourceManager.h"

#include <iostream>

// Ensure the containers are locked to this file's scope
namespace {
	std::map<std::string, sf::Texture> textures;
	std::map<std::string, sf::SoundBuffer> soundBuffers;
}

sf::Texture& rm::loadTexture(std::string filename) {
	if (textures.count(filename) == 0) {
		sf::Texture newTexture;
		if (newTexture.loadFromFile(filename)) {
			textures.insert(std::pair<std::string, sf::Texture>(filename, newTexture));
			// Debug
			//std::cout << "New texture created (" << filename << ")\n";
		}
		else {
			if (textures.count("BadTexture") == 0) {
				sf::Image badImage;
				badImage.create(2, 2, sf::Color::Black);
				badImage.setPixel(0, 0, sf::Color::Cyan);
				badImage.setPixel(1, 1, sf::Color::Cyan);
				newTexture.loadFromImage(badImage);
				newTexture.setRepeated(true);
				textures.insert(std::pair<std::string, sf::Texture>("BadTexture", newTexture));

				// Debug
				//std::cout << "Bad texture created (" << filename << ")\n";
			}
			else {
				// Debug
				//std::cout << "Bad texture loaded (" << filename << ")\n";
			}
			return textures["BadTexture"];
		}
	}
	else {
		// Debug
		//std::cout << "Existing texture loaded (" << filename << ")\n";
	}
	return textures[filename];
}

sf::SoundBuffer& rm::loadSoundBuffer(std::string filename) {
	if (soundBuffers.count(filename) == 0) {
		sf::SoundBuffer newSoundBuffer;
		if (newSoundBuffer.loadFromFile(filename)) {
			soundBuffers.insert(std::pair<std::string, sf::SoundBuffer>(filename, newSoundBuffer));
			// Debug
			//std::cout << "New sound buffer created (" << filename << ")\n";
		}
		else {
			// Debug
			//std::cout << "Cannot create sound buffer (" << filename << ")\n";
		}
	}
	else {
		// Debug
		//std::cout << "Existing sound buffer loaded (" << filename << ")\n";
	}
	return soundBuffers[filename];
}

void rm::clearCache() {
	textures.clear();
	soundBuffers.clear();
}