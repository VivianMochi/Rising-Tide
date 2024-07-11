#include "WaterBar.h"

#include "State.h"
#include "ResourceManager.h"
#include "ColorManager.h"

void WaterBar::init() {
	tankWater.setTexture(rm::loadTexture("Resource/Image/WaterTank.png"));
	tankWater.setPosition(3, 135);

	verticalBarSprite.setTexture(rm::loadTexture("Resource/Image/WaterBar.png"));
	verticalBarSprite.setTextureRect(sf::IntRect(0, 0, 6, 104));
	verticalBarSprite.setPosition(3, 31);

	verticalBarFrame.setTexture(rm::loadTexture("Resource/Image/WaterBar.png"));
	verticalBarFrame.setTextureRect(sf::IntRect(7, 0, 6, 104));
	verticalBarFrame.setPosition(3, 31);

	// Create blocks
	for (int i = 0; i < 10; i++) {
		blocks.emplace_back();
		if (i == 0) {
			blocks.back().thicknessIndex = 2;
		}
		else if (i <= 2) {
			blocks.back().thicknessIndex = 1;
		}
		else {
			blocks.back().thicknessIndex = 0;
		}
		blocks.back().position = sf::Vector2f(10, 32 + i * 10);
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
	verticalBarSprite.setColor(cm::getUIColor());
	verticalBarFrame.setColor(cm::getUIColor());
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
		}
		flashTime = 0;
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
	flashTime = 0;
}

void WaterBar::drain(int amount) {
	if (amount == -1) {
		resetSystem();
	}
	else {
		waterLevel -= amount;
		if (waterLevel < 0) {
			waterLevel = 0;
		}
	}
	waterFlashTime = 1;
	flashTime = 0;
}

void WaterBar::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(getPosition());

	target.draw(verticalBarSprite, states);
	target.draw(tankWater, states);
	target.draw(verticalBarFrame, states);

	for (int i = 0; i < blocks.size(); i++) {
		const WaterBlock &block = blocks[i];
		sf::Sprite blockSprite(rm::loadTexture("Resource/Image/WaterBar.png"));
		blockSprite.setTextureRect(sf::IntRect(18 + block.thicknessIndex * 5, 91, 4, 9));
		blockSprite.setPosition(block.position);
		if (i >= maxBlocks) {
			blockSprite.setColor(cm::getUIColorBright());
		}
		else if (block.flashTime > 0 && flashTime < 0.1) {
			blockSprite.setColor(cm::getFlashColor());
		}
		else if (i >= activeBlocks) {
			blockSprite.setColor(cm::getWaterColor());
		}
		else {
			blockSprite.setColor(cm::getUIColorMedium());
		}
		target.draw(blockSprite, states);
	}
}
