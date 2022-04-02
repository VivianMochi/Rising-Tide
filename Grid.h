#pragma once

#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 10;

struct Square {
	// What this block contains
	// Can be "", "shell", "weed", or "jelly"
	std::string inside = "";
	// If this square has been dug out
	bool dug = false;

	// 0-3, purely cosmetic
	int spriteIndex = 0;
	// The position to be rendered on the grid, affects click hitbox
	sf::Vector2f position;
	// The velocity of this square, affects position over time
	sf::Vector2f velocity;
};

class Grid : public Entity {
public:
	virtual void init() override;
	virtual void update(sf::Time elapsed) override;

	// Dig at the position relative to the grid
	// Returns true if there was something to dig
	bool digPosition(sf::Vector2f position);

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	sf::Vector2f getPositionForSquare(int x, int y);

	std::vector<std::vector<std::shared_ptr<Square>>> squares;

	sf::Sprite gridSprite;
};

