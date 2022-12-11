#include "RenderAssist.h"

#include "ResourceManager.h"
#include "ColorManager.h"

void ra::renderShell(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position) {
	sf::Sprite sprite(rm::loadTexture("Resource/Image/Squares.png"));
	sprite.setPosition(position);
	sprite.setTextureRect(sf::IntRect(0, 10, 10, 10));
	sprite.setColor(cm::getShellColor());
	target.draw(sprite, states);
}

void ra::renderWeed(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position) {
	sf::Sprite sprite(rm::loadTexture("Resource/Image/Squares.png"));
	sprite.setPosition(position);
	sprite.setTextureRect(sf::IntRect(10, 10, 10, 10));
	sprite.setColor(cm::getWeedColor());
	target.draw(sprite, states);
}

void ra::renderStar(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position) {
	sf::Sprite sprite(rm::loadTexture("Resource/Image/Squares.png"));
	sprite.setPosition(position);
	sprite.setTextureRect(sf::IntRect(10, 20, 10, 10));
	sprite.setColor(cm::getWeedColor());
	target.draw(sprite, states);
}

void ra::renderJelly(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position) {
	sf::Sprite sprite(rm::loadTexture("Resource/Image/Squares.png"));
	sprite.setPosition(position);
	sprite.setTextureRect(sf::IntRect(20, 10, 10, 10));
	sprite.setColor(cm::getJellyColor());
	target.draw(sprite, states);
}

void ra::renderFlag(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position, bool dug) {
	sf::Sprite sprite(rm::loadTexture("Resource/Image/Squares.png"));
	sprite.setPosition(position);
	sprite.setTextureRect(sf::IntRect(30, dug ? 20 : 10, 10, 10));
	sprite.setColor(cm::getFlagColor());
	target.draw(sprite, states);
}

void ra::renderUIBox(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position, int width, int style) {
	sf::Sprite sprite(rm::loadTexture("Resource/Image/UIBox.png"));
	sprite.setPosition(position);
	sprite.setTextureRect(sf::IntRect(0, 12 * style, width - 1, 12));
	sprite.setColor(cm::getUIColor());
	target.draw(sprite, states);

	sprite.setPosition(position + sf::Vector2f(width - 1, 0));
	sprite.setTextureRect(sf::IntRect(99, 12 * style, 1, 12));
	target.draw(sprite, states);
}
