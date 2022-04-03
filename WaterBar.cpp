#include "WaterBar.h"

#include "State.h"
#include "ResourceManager.h"
#include "ColorManager.h"

void WaterBar::init() {
	rm::loadTexture("Resource/Image/Water.png").setRepeated(true);
	tankWater.setTexture(rm::loadTexture("Resource/Image/Water.png"));
	tankWater.setTextureRect(sf::IntRect(0, 0, 6, 110));
	tankWater.setPosition(3, 135);

	verticalBarSprite.setTexture(rm::loadTexture("Resource/Image/WaterBar.png"));
	verticalBarSprite.setTextureRect(sf::IntRect(0, 13, 6, 101));
	verticalBarSprite.setPosition(3, 32);

	verticalBarFrame.setTexture(rm::loadTexture("Resource/Image/WaterBar.png"));
	verticalBarFrame.setTextureRect(sf::IntRect(6, 12, 6, 104));
	verticalBarFrame.setPosition(3, 31);

	horizontalBarSprite.setTexture(rm::loadTexture("Resource/Image/WaterBar.png"));
	horizontalBarSprite.setTextureRect(sf::IntRect(0, 0, 62, 12));
	horizontalBarSprite.setPosition(7, 121);

	// Create blocks
	for (int i = 0; i < 10; i++) {
		blocks.emplace_back();
		if (i == 0) {
			blocks.back().thin = false;
			blocks.back().position = sf::Vector2f(16 + i * 5, 123);
		}
		else {
			blocks.back().position = sf::Vector2f(19 + i * 5, 123);
		}
	}
}

void WaterBar::update(sf::Time elapsed) {
	flashTime += elapsed.asSeconds();
	if (flashTime >= 0.2) {
		flashTime = 0;
	}

	// Update blocks
	for (WaterBlock &block : blocks) {
		block.flashTime -= elapsed.asSeconds();
		if (block.flashTime < 0) {
			block.flashTime = 0;
		}
	}

	// Update tank water
	waterFlashTime -= elapsed.asSeconds();
	if (waterFlashTime < 0) {
		waterFlashTime = 0;
	}
	tankWater.move((sf::Vector2f(3, 31 + 100 - (10 * waterLevel)) - tankWater.getPosition()) * elapsed.asSeconds() * 5.0f);
	if (waterFlashTime > 0 && flashTime < 0.1) {
		tankWater.setColor(cm::getFlashColor());
	}
	else {
		tankWater.setColor(cm::getWaterColor());
	}

	// Update colors
	verticalBarSprite.setColor(cm::getPaletteColor(0));
	verticalBarFrame.setColor(cm::getPaletteColor(1));
	horizontalBarSprite.setColor(cm::getPaletteColor(0));
}

void WaterBar::resetSystem() {
	activeBlocks = maxBlocks;
	waterLevel = 0;
}

void WaterBar::setMaxBlocks(int blocks) {
	maxBlocks = blocks;
	if (activeBlocks > maxBlocks) {
		activeBlocks = maxBlocks;
	}
}

void WaterBar::increment(int amount) {
	activeBlocks -= amount;
	if (activeBlocks >= 0) {
		for (WaterBlock &block : blocks) {
			block.flashTime = 0;
		}
		blocks[activeBlocks].flashTime = 0.5;
		flashTime = 0;
	}
	while (activeBlocks <= 0) {
		for (WaterBlock &block : blocks) {
			block.flashTime = 1;
			flashTime = 0;
		}
		activeBlocks += maxBlocks;
		flood();
	}
}

void WaterBar::flood(int amount) {
	waterLevel += amount;
	if (waterLevel > 10) {
		waterLevel = 10;
	}
	waterFlashTime = 1;
}

void WaterBar::drain() {
	waterLevel = 0;
	waterFlashTime = 0.5;
}

void WaterBar::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(getPosition());

	target.draw(verticalBarSprite, states);
	target.draw(tankWater, states);
	target.draw(verticalBarFrame, states);

	target.draw(horizontalBarSprite, states);

	for (int i = 0; i < blocks.size(); i++) {
		const WaterBlock &block = blocks[i];
		sf::Sprite blockSprite(rm::loadTexture("Resource/Image/WaterBar.png"));
		if (block.thin) {
			blockSprite.setTextureRect(sf::IntRect(59, 12, 3, 8));
		}
		else {
			blockSprite.setTextureRect(sf::IntRect(53, 12, 6, 8));
		}
		blockSprite.setPosition(block.position);
		if (i >= maxBlocks) {
			blockSprite.setColor(cm::getPaletteColor(0));
		}
		else if (block.flashTime > 0 && flashTime < 0.1) {
			blockSprite.setColor(cm::getFlashColor());
		}
		else if (i >= activeBlocks) {
			blockSprite.setColor(cm::getWaterColor());
		}
		else {
			blockSprite.setColor(cm::getPaletteColor(2));
		}
		target.draw(blockSprite, states);
	}
}
