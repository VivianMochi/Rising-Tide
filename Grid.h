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
	// If this square has been flagged
	bool flagged = false;

	// 0-3, purely cosmetic
	int spriteIndex = 0;
	// The position to be rendered on the grid, affects click hitbox
	sf::Vector2f position;
	// The velocity of this square, affects position over time
	sf::Vector2f velocity;
};

struct Tab {
	// The row this is assigned to
	int row = 0;
	// The number to display
	int number = 0;

	// The x offset of the tab
	// Anything > 0 is hidden, -12 is fully visible
	float offsetX = 0;
};

class Grid : public Entity {
public:
	virtual void init() override;
	virtual void update(sf::Time elapsed) override;

	void generateGrid(int jellyfish, int shells = 1);

	// Dig at the position relative to the grid
	// Returns what was inside
	// Returns "none" if there was nothing to dig
	std::string digPosition(sf::Vector2f position);

	// Flag at the position relative to the grid
	// onlyRemove will only allow the removal of existing flags, not place new ones
	// Returns 1 if a flag was placed, -1 if a flag was removed, or 0 if nothing happened
	int flagPosition(sf::Vector2f position, bool onlyRemove);

	// Pop open the top left unpopped grid square and return what was inside
	// Flagged determines whether this only opens flagged squares or unflagged squares
	// Will return "none" if there are no applicable grid squares left
	std::string popSquare(bool flagged = false);

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	// Surround the given position with seaweed
	// If onlySome is true, there is a chance seaweed will be skipped
	void surroundWithSeaweed(sf::Vector2i position, bool onlySome = false);

	// Returns a random square with nothing inside it
	// If preferBottom is chosen, there is a ~50% chance it will be on the bottom half of the grid
	sf::Vector2i getEmptySquare(bool preferBottom = false);
	sf::Vector2f getPositionForSquare(int x, int y);
	bool isOnGrid(int x, int y);

	std::vector<std::vector<std::shared_ptr<Square>>> squares;
	std::vector<Tab> tabs;

	sf::Sprite gridSprite;
};

