#include "WaterBar.h"

#include "State.h"
#include "ResourceManager.h"
#include "ColorManager.h"

void WaterBar::init() {
	verticalBarSprite.setTexture(rm::loadTexture("Resource/Image/WaterBar.png"));
	verticalBarSprite.setTextureRect(sf::IntRect(0, 12, 6, 98));
	verticalBarSprite.setPosition(3, 32);

	verticalBarFrame.setTexture(rm::loadTexture("Resource/Image/WaterBar.png"));
	verticalBarFrame.setTextureRect(sf::IntRect(6, 12, 6, 98));
	verticalBarFrame.setPosition(3, 32);

	horizontalBarSprite.setTexture(rm::loadTexture("Resource/Image/WaterBar.png"));
	horizontalBarSprite.setTextureRect(sf::IntRect(0, 0, 62, 12));
	horizontalBarSprite.setPosition(7, 118);


	// Create blocks
	for (int i = 0; i < 10; i++) {
		blocks.emplace_back();
		if (i == 0) {
			blocks.back().thin = false;
			blocks.back().position = sf::Vector2f(16 + i * 5, 120);
		}
		else {
			blocks.back().position = sf::Vector2f(19 + i * 5, 120);
		}
	}
}

void WaterBar::update(sf::Time elapsed) {
	// Update colors
	verticalBarSprite.setColor(cm::getPaletteColor(0));
	verticalBarFrame.setColor(cm::getPaletteColor(1));
	horizontalBarSprite.setColor(cm::getPaletteColor(0));
}

void WaterBar::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(getPosition());

	target.draw(verticalBarSprite, states);
	target.draw(verticalBarFrame, states);
	target.draw(horizontalBarSprite, states);

	for (const WaterBlock &block : blocks) {
		sf::Sprite blockSprite(rm::loadTexture("Resource/Image/WaterBar.png"));
		if (block.thin) {
			blockSprite.setTextureRect(sf::IntRect(59, 12, 3, 8));
		}
		else {
			blockSprite.setTextureRect(sf::IntRect(53, 12, 6, 8));
		}
		blockSprite.setPosition(block.position);
		blockSprite.setColor(cm::getPaletteColor(1));
		target.draw(blockSprite, states);
	}
}
