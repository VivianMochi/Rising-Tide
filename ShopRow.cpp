#include "ShopRow.h"

#include "BitmapText.h"
#include "ColorManager.h"
#include "ResourceManager.h"

ShopRow::ShopRow(std::string text, std::string displayText, int cost) {
	this->text = text;
	this->displayText = displayText;
	this->cost = cost;

	baseRect = sf::IntRect(0, 0, 81, 10);
}

void ShopRow::init() {

}

void ShopRow::update(sf::Time elapsed) {

}

std::string ShopRow::clickPosition(sf::Vector2f position) {
	if (enabled) {
		sf::FloatRect hitBox(getPosition(), sf::Vector2f(baseRect.width, baseRect.height));
		if (hitBox.contains(position)) {
			return text;
		}
	}
	return "";
}

void ShopRow::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(getPosition());

	// Debug render hitbox
	//sf::RectangleShape hitbox(sf::Vector2f(baseRect.width, baseRect.height));
	//hitbox.setFillColor(sf::Color::Red);
	//target.draw(hitbox, states);

	if (selected) {
		sf::RectangleShape backdrop(sf::Vector2f(baseRect.width, baseRect.height));
		backdrop.setFillColor(cm::getUIColorMedium());
		target.draw(backdrop, states);
	}

	BitmapText textSprite;
	textSprite.setTexture(rm::loadTexture("Resource/Image/Font.png"));
	textSprite.setColor(cm::getUIColorMedium());
	if (selected) {
		textSprite.setColor(cm::getUIColorDark());
	}
	textSprite.setText(displayText);
	textSprite.setPosition(2, 1);
	target.draw(textSprite, states);

	if (!purchased) {
		textSprite.setColor(cm::getJellyColor());
		textSprite.setText(std::to_string(cost));
	}
	else {
		textSprite.setText("X");
	}
	textSprite.setPosition(baseRect.width - 1 - textSprite.getWidth(), 1);
	target.draw(textSprite, states);
}
