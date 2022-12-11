#include "BitmapText.h"

#include <cctype>

BitmapText::BitmapText() {

}

BitmapText::BitmapText(const sf::Texture &texture, std::string text) {
	setTexture(texture);
	setText(text);
}

BitmapText::~BitmapText() {

}

void BitmapText::setTexture(const sf::Texture &texture) {
	this->texture = &texture;
}

void BitmapText::setText(std::string text) {
	this->text = text;
}

void BitmapText::setColor(sf::Color color) {
	this->color = color;
}

std::string BitmapText::getText() const {
	return this->text;
}

int BitmapText::getWidth() {
	int width = 0;
	for (char character : text) {
		width += getGlyphWidth(character) + 1;
	}
	return width - 1;
}

void BitmapText::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	sf::Vector2f currentPosition = getPosition();
	if (texture) {
		sf::Sprite glyph(*texture);
		glyph.setColor(color);
		for (int i = 0; i < text.length(); i++) {
			glyph.setPosition(currentPosition);
			char character = std::toupper(text[i]);
			// This is super hacky
			if (character == '1' && monowidth) {
				glyph.move(1, 0);
			}
			if (character < '@') {
				glyph.setTextureRect(sf::IntRect(8 * (character - ' '), 0, 8, 8));
			}
			else {
				glyph.setTextureRect(sf::IntRect(8 * (character - '@'), 8, 8, 8));
			}
			target.draw(glyph, states);
			if (character == '\n') {
				currentPosition.x = getPosition().x;
				currentPosition.y += 10;
			}
			else if (character == '\t') {
				currentPosition.x = getPosition().x + (int)(currentPosition.x - getPosition().x) / 28 * 28 + 28;
			}
			else {
				currentPosition.x += getGlyphWidth(character) + 1;
			}
		}
	}
}

int BitmapText::getGlyphWidth(char character) const {
	character = std::toupper(character);

	if (character == '\n') {
		return 0;
	}
	else if (character == '\t') {
		return 0;
	}
	else if (character == '!' || character == '.' || character == ':' || character == ';' || character == '*' || character == '\'') {
		return 2;
	}
	else if (character == '(' || character == ')' || character == '[' || character == ']') {
		return 3;
	}
	else if ((character == '1' && !monowidth) || character == 'I' || character == '/' || character == '\\' || character == ' ') {
		return 4;
	}
	else if (character == 'M' || character == 'W') {
		return 7;
	}
	else {
		return 6;
	}
}