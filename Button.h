#pragma once

#include "Entity.h"


class Button : public Entity {
public:
	Button(std::string text = "");

	virtual void init() override;
	virtual void update(sf::Time elapsed) override;

	bool clickPosition(sf::Vector2f position);

	std::string text = "";
	bool enabled = true;

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	float pressTime = 0;

	sf::Sprite buttonSprite;
};

