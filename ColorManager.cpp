#include "ColorManager.h"

#include <vector>

namespace {
	std::vector<sf::Color> palette;
	std::vector<sf::Color> desiredPalette;
	sf::Image paletteSheet;
	int currentPalette = 0;
}

void cm::init() {
	paletteSheet.loadFromFile("Resource/Image/Palettes.png");
	palette.resize(paletteSheet.getSize().x);
	desiredPalette.resize(paletteSheet.getSize().x);

	selectPalette(0, true);
}

void cm::selectPalette(int index, bool instant) {
	if (index < 0) {
		index = 0;
	}
	while (index >= getTotalPalettes()) {
		index -= getTotalPalettes();
	}
	currentPalette = index;

	for (int i = 0; i < desiredPalette.size(); i++) {
		desiredPalette[i] = paletteSheet.getPixel(i, index);
	}

	if (instant) {
		palette = desiredPalette;
	}
}

int cm::getCurrentPalette() {
	return currentPalette;
}

void cm::updatePalette(sf::Time elapsed) {
	for (int i = 0; i < palette.size(); i++) {
		palette[i] = adjustColor(palette[i], desiredPalette[i], elapsed.asSeconds() * 200);
	}
}

int cm::getTotalPalettes() {
	return paletteSheet.getSize().y;
}

sf::Color cm::getPaletteColor(int index) {
	if (index >= 0 && index < palette.size()) {
		return palette[index];
	}
	else if (!palette.empty()) {
		return palette[0];
	}
	return sf::Color();
}

std::string cm::getPaletteName(int index) {
	if (index >= 0 && index < getTotalPalettes() && index < PALETTE_NAMES.size()) {
		return PALETTE_NAMES[index];
	}
	return "ERROR";
}

sf::Color cm::getSkyColor() {
	return getPaletteColor(3);
}

sf::Color cm::getWaterColor() {
	return getPaletteColor(7);
}

// Todo: rename getUIColorDark
sf::Color cm::getTextColor() {
	sf::Color output = getUIColor();
	int strength = 23;
	output.r *= strength / 100.0f;
	output.g *= strength / 100.0f;
	output.b *= strength / 100.0f;
	return output;
}

// Todo: rename getUIColorMedium
sf::Color cm::getDisabledTextColor() {
	sf::Color output = getUIColor();
	int strength = 57;
	output.r *= strength / 100.0f;
	output.g *= strength / 100.0f;
	output.b *= strength / 100.0f;
	return output;
}

// Todo: rename getUIColorBright
sf::Color cm::getActiveUIElementColor() {
	sf::Color output = getUIColor();
	int strength = 73;
	output.r *= strength / 100.0f;
	output.g *= strength / 100.0f;
	output.b *= strength / 100.0f;
	return output;
}

// Todo: should probably rename this too
sf::Color cm::getDetailUIColor() {
	sf::Color output = getUIColor();
	int strength = 64;
	output.r *= strength / 100.0f;
	output.g *= strength / 100.0f;
	output.b *= strength / 100.0f;
	return output;
}

sf::Color cm::getFlashColor() {
	return getPaletteColor(1);
}

sf::Color cm::getShellColor() {
	return getPaletteColor(4);
}

sf::Color cm::getWeedColor() {
	return getPaletteColor(5);
}

sf::Color cm::getJellyColor() {
	return getPaletteColor(6);
}

sf::Color cm::getFlagColor() {
	return getShellColor();
}

sf::Color cm::getUIColor() {
	return getSandColor();
}

sf::Color cm::getSandColor() {
	return getPaletteColor(0);
}

sf::Color cm::getCloudColor() {
	return getPaletteColor(2);
}

sf::Color cm::adjustColor(sf::Color input, sf::Color desired, float amount) {
	float r = adjustComponent(input.r, desired.r, amount);
	float g = adjustComponent(input.g, desired.g, amount);
	float b = adjustComponent(input.b, desired.b, amount);
	return sf::Color(r, g, b);
}

float cm::adjustComponent(float input, float desired, float amount) {
	if (input < desired) {
		input += amount;
		if (input > desired) {
			input = desired;
		}
	}
	else if (input > desired) {
		input -= amount;
		if (input < desired) {
			input = desired;
		}
	}
	return input;
}
