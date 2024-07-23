#pragma once

#include <SFML/Graphics.hpp>

namespace ra {
	void renderIcon(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position, sf::Vector2f spriteSheetPosition, sf::Color color);
	void renderShell(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position);
	void renderWeed(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position);
	void renderStar(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position);
	void renderJelly(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position);
	void renderFlag(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position, bool dug = false);
	void renderFlagBottom(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position, bool dug = false);
	void renderFlagTop(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position, bool dug = false);
	void renderFlagWake(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position, bool frame);

	void renderUIBox(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position, int width = 55, int style = 0);
};

