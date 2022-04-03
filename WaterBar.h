#pragma once

#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <vector>

struct WaterBlock {
	sf::Vector2f position;
	bool thin = true;
};

class WaterBar : public Entity {
public:
	virtual void init() override;
	virtual void update(sf::Time elapsed) override;

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	std::vector<WaterBlock> blocks;

	sf::Sprite verticalBarSprite;
	sf::Sprite verticalBarFrame;
	sf::Sprite horizontalBarSprite;
};

