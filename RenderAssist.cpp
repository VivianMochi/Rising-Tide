#include "RenderAssist.h"

#include "ResourceManager.h"
#include "ColorManager.h"

void ra::renderIcon(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position, sf::Vector2f spriteSheetPosition, sf::Color color) {
	sf::Sprite sprite(rm::loadTexture("Resource/Image/Squares.png"));
	sprite.setPosition(position);
	sprite.setTextureRect(sf::IntRect(spriteSheetPosition.x * 10, spriteSheetPosition.y * 10, 10, 10));
	sprite.setColor(color);
	target.draw(sprite, states);
}

void ra::renderShell(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position) {
	renderIcon(target, states, position, sf::Vector2f(0, 1), cm::getShellColor());
}

void ra::renderWeed(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position) {
	renderIcon(target, states, position, sf::Vector2f(1, 1), cm::getWeedColor());
}

void ra::renderStar(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position) {
	renderIcon(target, states, position, sf::Vector2f(1, 2), cm::getWeedColor());
}

void ra::renderJelly(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position) {
	renderIcon(target, states, position, sf::Vector2f(2, 1), cm::getJellyColor());
}

void ra::renderFlag(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position, bool dug) {
	if (!dug) {
		renderIcon(target, states, position, sf::Vector2f(3, 1), cm::getFlagColor());
	}
	else {
		renderIcon(target, states, position, sf::Vector2f(3, 2), cm::getFlagColor());
	}
}

void ra::renderFlagBottom(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position, bool dug) {
	if (!dug) {
		renderIcon(target, states, position, sf::Vector2f(0, 4), cm::getFlagColor());
	}
}

void ra::renderFlagTop(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position, bool dug) {
	if (!dug) {
		renderIcon(target, states, position, sf::Vector2f(1, 4), cm::getFlagColor());
	}
	else {
		renderIcon(target, states, position, sf::Vector2f(3, 2), cm::getFlagColor());
	}
}

void ra::renderFlagWake(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position, bool frame) {
	renderIcon(target, states, position, sf::Vector2f(frame ? 3 : 2, 4), cm::getWaterColor());
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
