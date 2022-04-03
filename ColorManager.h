#pragma once

#include <SFML/Graphics.hpp>

namespace cm {
	void init();
	void selectPalette(int index, bool instant = false);
	int getCurrentPalette();
	void updatePalette(sf::Time elapsed);

	sf::Color getPaletteColor(int index);

	sf::Color getSkyColor();
	sf::Color getWaterColor();
	sf::Color getTextColor();
	sf::Color getFlashColor();

	sf::Color adjustColor(sf::Color input, sf::Color desired, float amount);
	float adjustComponent(float input, float desired, float amount);
}

