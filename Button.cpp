#include "Button.h"

#include "State.h"
#include "ResourceManager.h"
#include "ColorManager.h"
#include "BitmapText.h"

Button::Button(std::string text) {
	this->text = text;
	buttonSprite.setTexture(rm::loadTexture("Resource/Image/Button.png"));
	baseRect = sf::IntRect(0, 0, 56, 16);
}

Button::Button(std::string text, sf::Texture &texture, sf::IntRect textureRect) {
	this->text = text;
	buttonSprite.setTexture(texture);
	baseRect = textureRect;
	showText = false;
}

void Button::init() {

}

void Button::update(sf::Time elapsed) {
	// Press animation
	pressTime -= elapsed.asSeconds();
	if (pressTime < 0) {
		pressTime = 0;
	}
	sf::IntRect textureRect = baseRect;
	if (!enabled) {
		textureRect.top = 2 * textureRect.height;
	}
	else if (pressTime > 0) {
		textureRect.top = textureRect.height;
	}
	buttonSprite.setTextureRect(textureRect);
	buttonSprite.setColor(cm::getUIColor());
}

std::string Button::clickPosition(sf::Vector2f position) {
	if (enabled) {
		sf::FloatRect hitBox(getPosition(), sf::Vector2f(baseRect.width, baseRect.height));
		if (hitBox.contains(position)) {
			pressTime = 0.1;
			return text;
		}
	}
	return "";
}

void Button::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(getPosition());

	// Todo: add highlighting on hover, will need to add stuff to ButtonManager
	target.draw(buttonSprite, states);

	if (showText) {
		BitmapText textSprite;
		textSprite.setTexture(rm::loadTexture("Resource/Image/Font.png"));
		if (!enabled) {
			textSprite.setColor(cm::getDisabledTextColor());
		}
		else {
			textSprite.setColor(cm::getTextColor());
		}
		textSprite.setText(text);
		textSprite.setPosition(baseRect.width / 2 - textSprite.getWidth() / 2, (pressTime > 0 || !enabled ? 5 : 3));
		target.draw(textSprite, states);
	}
}
