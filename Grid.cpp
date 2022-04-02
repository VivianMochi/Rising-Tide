#include "Grid.h"

#include "State.h"
#include "ResourceManager.h"

void Grid::init() {
	gridSprite.setTexture(rm::loadTexture("Resource/Image/Grid.png"));

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
}

void Grid::update(sf::Time elapsed) {
	// Update squares
	for (int x = 0; x < GRID_WIDTH; x++) {
		for (int y = 0; y < GRID_HEIGHT; y++) {
			if (squares[x][y]) {
			}
		}
	}
}

bool Grid::digPosition(sf::Vector2f position) {
	for (int x = 0; x < GRID_WIDTH; x++) {
		for (int y = 0; y < GRID_HEIGHT; y++) {
			if (squares[x][y]) {
				if (sf::FloatRect(squares[x][y]->position, sf::Vector2f(10, 10)).contains(position) && !squares[x][y]->dug) {
					squares[x][y]->dug = true;
					return true;
				}
			}
		}
	}
	return false;
}

void Grid::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(getPosition());

	target.draw(gridSprite, states);

	// Draw squares
	sf::Sprite squareSprite(rm::loadTexture("Resource/Image/Squares.png"));
	for (int x = 0; x < GRID_WIDTH; x++) {
		for (int y = 0; y < GRID_HEIGHT; y++) {
			if (squares[x][y]) {
				squareSprite.setPosition(squares[x][y]->position);
				squareSprite.setTextureRect(sf::IntRect(squares[x][y]->spriteIndex * 10, 0, 10, 10));
				if (!squares[x][y]->dug) {
					target.draw(squareSprite, states);
				}
			}
		}
	}
}

sf::Vector2f Grid::getPositionForSquare(int x, int y) {
	return sf::Vector2f(x * 10, y * 10);
}
