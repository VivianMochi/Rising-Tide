#pragma once

#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

const int GRID_WIDTH = 10;
const int GRID_HEIGHT = 10;

struct Square {
	// What this block contains
	// Can be "", "shell", "weed", "star", or "jelly"
	std::string inside = "";
	// If this square has been dug out
	bool dug = false;
	// If this square has been flagged
	bool flagged = false;
	// If this square has been marked with an X
	bool marked = false;

	// 0-3, purely cosmetic
	int spriteIndex = 0;
	// The position to be rendered on the grid, affects click hitbox
	sf::Vector2f position;
	// The velocity of this square, affects position over time
	// Currently unimplemented
	sf::Vector2f velocity;
};

struct Tab {
	// The row this is assigned to
	int row = 0;
	// The number to display
	int number = 0;
	// If this tab's row is complete
	bool complete = false;

	// The x offset of the tab
	// Anything > 0 is hidden, -12 is fully visible
	float offsetX = 0;
};

enum ActionType {
	noAction,
	digAction,
	flagAction,
	markAction,
};

enum ActionRule {
	toggleState,
	onlyAdd,
	onlyRemove,
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
	// The rule can be used to limit to only adding or removing flags
	// Returns 1 if a flag was placed, -1 if a flag was removed, or 0 if nothing happened
	int flagPosition(sf::Vector2f position, ActionRule rule = toggleState);

	// Draw an X in the sand at the position relative to the grid
	// The rule can be used to limit to only adding or removing marks
	// Returns 1 if a mark was placed, -1 if a mark was removed, or 0 if nothing happened
	int markPosition(sf::Vector2f position, ActionRule rule = toggleState);

	// Pop open the top left unpopped grid square and return what was inside
	// Flagged determines whether this only opens flagged squares or unflagged squares
	// Will return "none" if there are no applicable grid squares left
	std::string popSquare(bool flagged = false);

	// This will reveal the contents of all tiles when true
	// Used for debugging
	bool xray = false;

	// Total jelly count on the board
	int totalJellies = 0;
	// Amount of jellies unaccounted for via completed tabs
	int hiddenJellies = 0;

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	// Surround the given position with seaweed
	// If onlySome is true, there is a chance seaweed will be skipped
	void surroundWithSeaweed(sf::Vector2i position, bool onlySome = false);

	// Check the grid state and complete any tabs 
	void checkTabCompletion();

	// Returns a random square with nothing inside it
	// If preferBottom is chosen, there is a ~50% chance it will be on the bottom half of the grid
	sf::Vector2i getEmptySquare(bool preferBottom = false);
	// Returns a random square matching the specified type
	sf::Vector2i getRandomSquare(std::string type = "");
	sf::Vector2f getPositionForSquare(int x, int y);
	bool isOnGrid(int x, int y);
	// Returns the number of jellyfish around the given position
	// Ignores the given position
	int getSurroundingJellies(sf::Vector2i position, bool includeDiagonals = true);

	std::vector<std::vector<std::shared_ptr<Square>>> squares;
	std::vector<Tab> tabs;

	sf::Sprite gridSprite;
};

