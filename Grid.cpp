#include "Grid.h"

#include "State.h"
#include "ResourceManager.h"

void Grid::init() {
	gridSprite.setTexture(rm::loadTexture("Resource/Image/Grid.png"));

	generateGrid(0, 0);
}

void Grid::update(sf::Time elapsed) {
	// Update squares
	for (int x = 0; x < GRID_WIDTH; x++) {
		for (int y = 0; y < GRID_HEIGHT; y++) {
			if (squares[x][y]) {
				// Nothing here yet
			}
		}
	}

	// Update tabs
	for (Tab &tab : tabs) {
		tab.offsetX += (-12 - tab.offsetX) * elapsed.asSeconds() * 8;
	}
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
		}
	}

	// Place all jellies, leaning towards bottom of grid
	for (int j = 0; j < jellyfish; j++) {
		sf::Vector2i position = getEmptySquare(true);
		squares[position.x][position.y]->inside = "jelly";
	}

	// Place all shells
	for (int s = 0; s < shells; s++) {
		sf::Vector2i position = getEmptySquare();
		squares[position.x][position.y]->inside = "shell";
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

	// Add tabs
	for (int y = 0; y < GRID_HEIGHT; y++) {
		int jelliesInRow = 0;
		for (int x = 0; x < GRID_WIDTH; x++) {
			if (squares[x][y]) {
				if (squares[x][y]->inside == "jelly") {
					jelliesInRow += 1;
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
}

std::string Grid::digPosition(sf::Vector2f position) {
	for (int x = 0; x < GRID_WIDTH; x++) {
		for (int y = 0; y < GRID_HEIGHT; y++) {
			if (squares[x][y]) {
				if (sf::FloatRect(squares[x][y]->position, sf::Vector2f(10, 10)).contains(position) && !squares[x][y]->dug && !squares[x][y]->flagged) {
					squares[x][y]->dug = true;
					return squares[x][y]->inside;
				}
			}
		}
	}
	return "none";
}

int Grid::flagPosition(sf::Vector2f position, bool onlyRemove) {
	for (int x = 0; x < GRID_WIDTH; x++) {
		for (int y = 0; y < GRID_HEIGHT; y++) {
			if (squares[x][y]) {
				if (sf::FloatRect(squares[x][y]->position, sf::Vector2f(10, 10)).contains(position) && !squares[x][y]->dug) {
					if (onlyRemove) {
						if (squares[x][y]->flagged) {
							squares[x][y]->flagged = false;
							return -1;
						}
					}
					else {
						squares[x][y]->flagged = !squares[x][y]->flagged;
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

std::string Grid::popSquare(bool flagged) {
	for (int y = 0; y < GRID_HEIGHT; y++) {
		for (int x = 0; x < GRID_WIDTH; x++) {
			if (squares[x][y]) {
				if (!squares[x][y]->dug) {
					if ((flagged && squares[x][y]->flagged) || (!flagged && !squares[x][y]->flagged)) {
						squares[x][y]->dug = true;
						return squares[x][y]->inside;
					}
				}
			}
		}
	}
	return "none";
}

void Grid::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(getPosition());

	// Draw tabs
	sf::Sprite tabSprite(rm::loadTexture("Resource/Image/Tabs.png"));
	tabSprite.setTextureRect(sf::IntRect(0, 0, 12, 8));
	sf::Sprite numberSprite(rm::loadTexture("Resource/Image/Tabs.png"));
	for (const Tab &tab : tabs) {
		if (tab.offsetX < 0) {
			tabSprite.setPosition(tab.offsetX, tab.row * 10 + 1);
			target.draw(tabSprite, states);

			numberSprite.setTextureRect(sf::IntRect(0, 8 * tab.number, 12, 8));
			numberSprite.setPosition(tabSprite.getPosition());
			target.draw(numberSprite, states);
		}
	}

	// Draw the base grid
	target.draw(gridSprite, states);

	// Draw squares
	sf::Sprite squareSprite(rm::loadTexture("Resource/Image/Squares.png"));
	for (int x = 0; x < GRID_WIDTH; x++) {
		for (int y = 0; y < GRID_HEIGHT; y++) {
			if (squares[x][y]) {
				// Draw sand square
				squareSprite.setPosition(squares[x][y]->position);
				if (!squares[x][y]->dug) {
					squareSprite.setTextureRect(sf::IntRect(squares[x][y]->spriteIndex * 10, 0, 10, 10));
					target.draw(squareSprite, states);
				}
				else {
					if (squares[x][y]->inside == "shell") {
						squareSprite.setTextureRect(sf::IntRect(0, 10, 10, 10));
						target.draw(squareSprite, states);
					}
					else if (squares[x][y]->inside == "weed") {
						squareSprite.setTextureRect(sf::IntRect(10, 10, 10, 10));
						target.draw(squareSprite, states);
					}
					else if (squares[x][y]->inside == "jelly") {
						squareSprite.setTextureRect(sf::IntRect(20, 10, 10, 10));
						target.draw(squareSprite, states);
					}
				}

				// Draw flag
				if (squares[x][y]->flagged) {
					squareSprite.setTextureRect(sf::IntRect(30, (squares[x][y]->dug ? 20 : 10), 10, 10));
					target.draw(squareSprite, states);
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
