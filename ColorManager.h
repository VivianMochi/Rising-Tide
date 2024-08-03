#pragma once

#include <SFML/Graphics.hpp>

const std::vector<std::string> PALETTE_NAMES = {
	"Beach",
	"Wood",
	"Sunset",
	"Night",
	"Pastel",
	"Hills",
	"Magma",
	"Virtual",
	"Soda",
	"Retro",
};

namespace cm {
	void init();
	void selectPalette(int index, bool instant = false);
	int getCurrentPalette();
	void updatePalette(sf::Time elapsed);
	int getTotalPalettes();

	sf::Color getPaletteColor(int index);
	std::string getPaletteName(int index);

	sf::Color getSkyColor();
	sf::Color getWaterColor();
	sf::Color getUIColorDark();
	sf::Color getUIColorMedium();
	sf::Color getUIColorMediumDetail();
	sf::Color getUIColorBright();
	sf::Color getFlashColor();
	sf::Color getShellColor();
	sf::Color getWeedColor();
	sf::Color getJellyColor();
	sf::Color getFlagColor();
	sf::Color getUIColor();
	sf::Color getSandColor();
	sf::Color getCloudColor();
	sf::Color getResultsTextColor(bool dark);

	sf::Color adjustColor(sf::Color input, sf::Color desired, float amount);
	float adjustComponent(float input, float desired, float amount);
}

