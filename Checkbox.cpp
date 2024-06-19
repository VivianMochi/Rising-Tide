#include "Checkbox.h"

#include "ResourceManager.h"
#include "ColorManager.h"

Checkbox::Checkbox(std::string text) {
    this->text = text;
	baseRect = sf::IntRect(0, 0, 10, 8);
	boxSprite.setTexture(rm::loadTexture("Resource/Image/Checkbox.png"));
	boxSprite.setTextureRect(baseRect);
	showText = false;
}

void Checkbox::init() {

}

void Checkbox::update(sf::Time elapsed) {
	// Press animation
	sf::IntRect textureRect = baseRect;
	if (toggled) {
		textureRect.top = textureRect.height;
	}
	boxSprite.setTextureRect(textureRect);
	boxSprite.setColor(cm::getUIColor());
}

void Checkbox::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(getPosition());

	target.draw(boxSprite, states);
}
