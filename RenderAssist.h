#pragma once

#include <SFML/Graphics.hpp>

namespace ra {
	void renderShell(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position);
	void renderWeed(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position);
	void renderStar(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position);
	void renderJelly(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position);
	void renderFlag(sf::RenderTarget &target, sf::RenderStates states, sf::Vector2f position, bool dug = false);
};

