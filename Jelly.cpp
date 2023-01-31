#include "Jelly.h"

#include "ColorManager.h"

void Jelly::init() {

}

void Jelly::update(sf::Time elapsed) {

}

void Jelly::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	sf::CircleShape debugSprite;
	debugSprite.setPosition(getPosition());
	debugSprite.setRadius(10);
	debugSprite.setOrigin(10, 10);
	debugSprite.setFillColor(cm::getJellyColor());
	target.draw(debugSprite, states);
}
