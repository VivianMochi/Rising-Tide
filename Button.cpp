#include "Button.h"

#include "State.h"
#include "ResourceManager.h"
#include "ColorManager.h"
#include "BitmapText.h"

Button::Button(std::string text) {
	this->text = text;
}

void Button::init() {
	buttonSprite.setTexture(rm::loadTexture("Resource/Image/Button.png"));
	buttonSprite.setTextureRect(sf::IntRect(0, 0, 55, 14));
}

void Button::update(sf::Time elapsed) {
	// Press animation
	pressTime -= elapsed.asSeconds();
	if (pressTime < 0) {
		pressTime = 0;
	}
	if (!enabled) {
		buttonSprite.setTextureRect(sf::IntRect(0, 28, 55, 14));
	}
	else if (pressTime > 0) {
		buttonSprite.setTextureRect(sf::IntRect(0, 14, 55, 14));
	}
	else {
		buttonSprite.setTextureRect(sf::IntRect(0, 0, 55, 14));
	}
}

bool Button::clickPosition(sf::Vector2f position) {
	if (enabled) {
		sf::FloatRect hitBox(getPosition(), sf::Vector2f(55, 14));
		if (hitBox.contains(position)) {
			pressTime = 0.1;
			return true;
		}
	}
	return false;
}

void Button::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(getPosition());

	target.draw(buttonSprite, states);

	BitmapText textSprite;
	textSprite.setTexture(rm::loadTexture("Resource/Image/Font.png"));
	if (!enabled) {
		textSprite.setColor(cm::getPaletteColor(1));
	}
	else {
		textSprite.setColor(cm::getTextColor());
	}
	textSprite.setText(text);
	textSprite.setPosition(55 / 2 - textSprite.getWidth() / 2, (pressTime > 0 || !enabled ? 4 : 2));
	target.draw(textSprite, states);
}
