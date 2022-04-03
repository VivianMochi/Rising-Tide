#pragma once

#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <vector>

struct WaterBlock {
	sf::Vector2f position;
	bool thin = true;

	float flashTime = 0;
};

class WaterBar : public Entity {
public:
	virtual void init() override;
	virtual void update(sf::Time elapsed) override;

	void resetSystem();
	void setMaxBlocks(int blocks);

	// Increment the water counter
	void increment(int amount = 1);
	// Increase the water level by a whole block
	void flood(int amount = 1);
	// Reset the water level
	void drain();

	int waterLevel = 0;
	int activeBlocks = 10;

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	int maxBlocks = 10;

	float flashTime = 0;
	float waterFlashTime = 0;

	std::vector<WaterBlock> blocks;

	sf::Sprite tankWater;
	sf::Sprite verticalBarSprite;
	sf::Sprite verticalBarFrame;
	sf::Sprite horizontalBarSprite;
};

