#include "Grid.h"

#include "State.h"
#include "ResourceManager.h"
#include "ColorManager.h"
#include "RenderAssist.h"
#include "PlayState.h"
#include "WaterBar.h"

void Grid::init() {
	gridSprite.setTexture(rm::loadTexture("Resource/Image/Grid.png"));

	generateGrid(0, 0);
}

void Grid::update(sf::Time elapsed) {
	// Update squares
	for (int x = 0; x < GRID_WIDTH; x++) {
		for (int y = 0; y < GRID_HEIGHT; y++) {
			if (squares[x][y]) {
				squares[x][y]->animationTimer += elapsed.asSeconds();
				if (squares[x][y]->animationTimer > 2.0) {
					squares[x][y]->animationTimer -= 2.0;
				}
			}
		}
	}

	// Update tabs
	for (Tab &tab : tabs) {
		tab.offsetX += (-12 - tab.offsetX) * elapsed.asSeconds() * 8;
	}

	// Update colors
	gridSprite.setColor(cm::getUIColor());
}

void Grid::generateGrid(int jellyfish, int shells) {
	squares.clear();
	tabs.clear();

	// Fill grid
	squares.resize(GRID_WIDTH);
	for (int x = 0; x < GRID_WIDTH; x++) {
		squares[x].resize(GRID_HEIGHT);
		for (int y = 0; y < GRID_HEIGHT; y++) {
			squares[x][y] = std::make_shared<Square>();
			squares[x][y]->spriteIndex = std::rand() % 4;
			squares[x][y]->position = getPositionForSquare(x, y);
			squares[x][y]->animationTimer = y * 0.1f + squares[x][y]->spriteIndex / 20.0f;
		}
	}

	// Place all jellies
	for (int j = 0; j < jellyfish; j++) {
		sf::Vector2i position = getEmptySquare();
		squares[position.x][position.y]->inside = "jelly";
	}

	// Place starfish
	if (NEW_MODE) {
		for (int x = 0; x < GRID_WIDTH; x++) {
			for (int y = 0; y < GRID_HEIGHT; y++) {
				if (squares[x][y]) {
					if (squares[x][y]->inside == "") {
						int jellies = getSurroundingJellies(sf::Vector2i(x, y));
						if (jellies >= 3) {
							squares[x][y]->inside = "star";
						}
					}
				}
			}
		}
	}

	// Shell option A: Place shells randomly
	if (!NEW_MODE) {
		for (int s = 0; s < shells; s++) {
			sf::Vector2i position = getEmptySquare();
			squares[position.x][position.y]->inside = "shell";
		}
	}

	// Place seaweed
	for (int x = 0; x < GRID_WIDTH; x++) {
		for (int y = 0; y < GRID_HEIGHT; y++) {
			if (squares[x][y]) {
				if (squares[x][y]->inside == "jelly") {
					surroundWithSeaweed(sf::Vector2i(x, y));
				}
				else if (squares[x][y]->inside == "shell") {
					// In hindsight, surrounding shells with seaweed was kinda evil
					//surroundWithSeaweed(sf::Vector2i(x, y), true);
				}
			}
		}
	}

	// Shell option B: Replace random seaweed with shells
	if (NEW_MODE) {
		for (int s = 0; s < shells; s++) {
			sf::Vector2i position = getRandomSquare("weed");
			squares[position.x][position.y]->inside = "shell";
		}
	}

	// Add tabs and validate jelly count
	totalJellies = 0;
	for (int y = 0; y < GRID_HEIGHT; y++) {
		int jelliesInRow = 0;
		for (int x = 0; x < GRID_WIDTH; x++) {
			if (squares[x][y]) {
				if (squares[x][y]->inside == "jelly") {
					jelliesInRow += 1;
					totalJellies += 1;
				}
			}
		}
		if (jelliesInRow > 0) {
			tabs.emplace_back();
			tabs.back().row = y;
			tabs.back().number = jelliesInRow;
			tabs.back().offsetX = y * 20;
		}
	}
	hiddenJellies = totalJellies;
}

std::string Grid::digPosition(sf::Vector2f position) {
	for (int x = 0; x < GRID_WIDTH; x++) {
		for (int y = 0; y < GRID_HEIGHT; y++) {
			if (squares[x][y]) {
				if (sf::FloatRect(squares[x][y]->position, sf::Vector2f(10, 10)).contains(position) && !squares[x][y]->dug && !squares[x][y]->flagged) {
					squares[x][y]->dug = true;
					checkTabCompletion();
					return squares[x][y]->inside;
				}
			}
		}
	}
	return "none";
}

int Grid::flagPosition(sf::Vector2f position, ActionRule rule) {
	for (int x = 0; x < GRID_WIDTH; x++) {
		for (int y = 0; y < GRID_HEIGHT; y++) {
			if (squares[x][y]) {
				if (sf::FloatRect(squares[x][y]->position, sf::Vector2f(10, 10)).contains(position) && !squares[x][y]->dug) {
					if (rule == onlyRemove) {
						if (squares[x][y]->flagged) {
							squares[x][y]->flagged = false;
							checkTabCompletion();
							return -1;
						}
					}
					else if (rule == onlyAdd) {
						if (!squares[x][y]->flagged) {
							squares[x][y]->flagged = true;
							squares[x][y]->marked = false;
							checkTabCompletion();
							return 1;
						}
					}
					else {
						squares[x][y]->flagged = !squares[x][y]->flagged;
						squares[x][y]->marked = false;
						checkTabCompletion();
						if (squares[x][y]->flagged) {
							return 1;
						}
						else {
							return -1;
						}
					}
				}
			}
		}
	}
	return 0;
}

int Grid::markPosition(sf::Vector2f position, ActionRule rule) {
	for (int x = 0; x < GRID_WIDTH; x++) {
		for (int y = 0; y < GRID_HEIGHT; y++) {
			if (squares[x][y]) {
				if (sf::FloatRect(squares[x][y]->position, sf::Vector2f(10, 10)).contains(position) && !squares[x][y]->dug && !squares[x][y]->flagged) {
					if (rule == onlyRemove) {
						if (squares[x][y]->marked) {
							squares[x][y]->marked = false;
							checkTabCompletion();
							return -1;
						}
					}
					else if (rule == onlyAdd) {
						if (!squares[x][y]->marked) {
							squares[x][y]->marked = true;
							checkTabCompletion();
							return 1;
						}
					}
					else {
						squares[x][y]->marked = !squares[x][y]->marked;
						checkTabCompletion();
						if (squares[x][y]->marked) {
							return 1;
						}
						else {
							return -1;
						}
					}
				}
			}
		}
	}
	return 0;
}

std::string Grid::popSquare(bool flagged) {
	for (int y = 0; y < GRID_HEIGHT; y++) {
		for (int x = 0; x < GRID_WIDTH; x++) {
			if (squares[x][y]) {
				if (!squares[x][y]->dug) {
					if ((flagged && squares[x][y]->flagged) || (!flagged && !squares[x][y]->flagged)) {
						squares[x][y]->dug = true;
						checkTabCompletion();
						return squares[x][y]->inside;
					}
				}
			}
		}
	}
	return "none";
}

void Grid::renderOverWater(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(getPosition());

	for (int x = 0; x < GRID_WIDTH; x++) {
		for (int y = 0; y < GRID_HEIGHT; y++) {
			if (squares[x][y]) {
				if (squares[x][y]->flagged) {
					// 7 from the flag wake's y position within the square
					if (!squares[x][y]->dug && y * 10 + 7 > 120 - water->masterDepth) {
						ra::renderFlagWake(target, states, squares[x][y]->position, squares[x][y]->animationTimer < 1.0f);
					}
					ra::renderFlagTop(target, states, squares[x][y]->position, squares[x][y]->dug);
				}
			}
		}
	}
}

void Grid::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(getPosition());

	// Draw tabs
	sf::Sprite tabSprite(rm::loadTexture("Resource/Image/Tabs.png"));
	sf::Sprite numberSprite(rm::loadTexture("Resource/Image/Tabs.png"));
	for (const Tab &tab : tabs) {
		if (tab.offsetX < 0) {
			tabSprite.setPosition(tab.offsetX, tab.row * 10 + 1);
			if (tab.complete) {
				tabSprite.setTextureRect(sf::IntRect(0, 8, 12, 8));
				tabSprite.setColor(cm::getUIColor());
			}
			else {
				tabSprite.setTextureRect(sf::IntRect(0, 0, 12, 8));
				tabSprite.setColor(cm::getJellyColor());
			}
			target.draw(tabSprite, states);

			numberSprite.setTextureRect(sf::IntRect(0, 8 + 8 * tab.number, 12, 8));
			numberSprite.setPosition(tabSprite.getPosition());
			if (tab.complete) {
				numberSprite.setColor(cm::getUIColorMedium());
			}
			else {
				numberSprite.setColor(cm::getFlashColor());
			}
			target.draw(numberSprite, states);
		}
	}

	// Draw the base grid
	target.draw(gridSprite, states);

	// Draw squares
	sf::Sprite squareSprite(rm::loadTexture("Resource/Image/Squares.png"));
	squareSprite.setColor(cm::getSandColor());
	for (int x = 0; x < GRID_WIDTH; x++) {
		for (int y = 0; y < GRID_HEIGHT; y++) {
			if (squares[x][y]) {
				// Draw sand square
				squareSprite.setPosition(squares[x][y]->position);
				if (!squares[x][y]->dug && !xray) {
					squareSprite.setTextureRect(sf::IntRect(squares[x][y]->spriteIndex * 10, 0, 10, 10));
					target.draw(squareSprite, states);
				}
				else {
					if (squares[x][y]->inside == "shell") {
						ra::renderShell(target, states, squares[x][y]->position);
					}
					else if (squares[x][y]->inside == "weed") {
						ra::renderWeed(target, states, squares[x][y]->position);
					}
					else if (squares[x][y]->inside == "star") {
						ra::renderStar(target, states, squares[x][y]->position);
					}
					else if (squares[x][y]->inside == "jelly") {
						ra::renderJelly(target, states, squares[x][y]->position);
					}
				}

				if (squares[x][y]->flagged) {
					ra::renderFlagBottom(target, states, squares[x][y]->position, squares[x][y]->dug);
				}
				else if (!squares[x][y]->dug && squares[x][y]->marked) {
					ra::renderIcon(target, states, squares[x][y]->position, sf::Vector2f(0, 2), cm::getSandColor());
				}
			}
		}
	}
}

void Grid::surroundWithSeaweed(sf::Vector2i position, bool onlySome) {
	// Add surrounding seaweed
	for (int dx = -1; dx <= 1; dx++) {
		for (int dy = -1; dy <= 1; dy++) {
			int wx = position.x + dx;
			int wy = position.y + dy;
			if (isOnGrid(wx, wy) && squares[wx][wy] && squares[wx][wy]->inside == "") {
				if (!onlySome || std::rand() % 4) {
					squares[wx][wy]->inside = "weed";
				}
			}
		}
	}
}

void Grid::checkTabCompletion() {
	hiddenJellies = totalJellies;
	for (Tab &tab : tabs) {
		tab.complete = true;
		int jelliesFound = 0;
		int flagsInRow = 0;
		for (int x = 0; x < GRID_WIDTH; x++) {
			if (squares[x][tab.row]) {
				if (!squares[x][tab.row]->dug && !squares[x][tab.row]->flagged && !squares[x][tab.row]->marked) {
					// If any cell is untouched in the row, we say the row is incomplete by default
					tab.complete = false;
				}
				if (squares[x][tab.row]->flagged) {
					flagsInRow += 1;
				}
				if (squares[x][tab.row]->dug && squares[x][tab.row]->inside == "jelly") {
					jelliesFound += 1;
				}
			}
		}
		if (flagsInRow + jelliesFound < tab.number) {
			// If flags + found jellies in the row is less than the number of known jellies, the row isn't done yet
			tab.complete = false;
		}
		if (jelliesFound >= tab.number) {
			// Conversely, if all jellies in the row are revealed, the row is done
			tab.complete = true;
		}

		// Adjust hidden jelly count based on tab state
		if (tab.complete) {
			hiddenJellies -= tab.number;
		}
		else {
			// Otherwise account for jellies found
			hiddenJellies -= jelliesFound;
		}
	}
}

sf::Vector2i Grid::getEmptySquare(bool preferBottom) {
	for (int attempt = 0; attempt < 5; attempt++) {
		sf::Vector2i output;
		if (preferBottom && std::rand() % 2) {
			output.x = std::rand() % 10;
			output.y = std::rand() % 5 + 5;
		}
		else {
			output.x = std::rand() % 10;
			output.y = std::rand() % 10;
		}
		if (squares[output.x][output.y] && squares[output.x][output.y]->inside == "") {
			// This output is compliant, return it
			return output;
		}
	}
	// On failure, returns the top left corner
	return sf::Vector2i(0, 0);
}

sf::Vector2i Grid::getRandomSquare(std::string type) {
	for (int attempt = 0; attempt < 50; attempt++) {
		sf::Vector2i output;
		output.x = std::rand() % 10;
		output.y = std::rand() % 10;
		if (squares[output.x][output.y] && squares[output.x][output.y]->inside == type) {
			// This output is compliant, return it
			return output;
		}
	}
	// On failure, returns the top left corner
	return sf::Vector2i(0, 0);
}

sf::Vector2f Grid::getPositionForSquare(int x, int y) {
	return sf::Vector2f(x * 10, y * 10);
}

bool Grid::isOnGrid(int x, int y) {
	if (x >= 0 && x < GRID_WIDTH) {
		if (y >= 0 && y < GRID_HEIGHT) {
			return true;
		}
	}
	return false;
}

int Grid::getSurroundingJellies(sf::Vector2i position, bool includeDiagonals) {
	int jellies = 0;
	for (int dx = -1; dx <= 1; dx++) {
		for (int dy = -1; dy <= 1; dy++) {
			if (!(dx == 0 && dy == 0)) {
				if (includeDiagonals || (dx == 0 || dy == 0)) {
					int jx = position.x + dx;
					int jy = position.y + dy;
					if (isOnGrid(jx, jy) && squares[jx][jy] && squares[jx][jy]->inside == "jelly") {
						jellies += 1;
					}
				}
			}
		}
	}
	return jellies;
}
