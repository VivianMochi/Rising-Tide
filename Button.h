#pragma once

#include "Entity.h"


class Button : public Entity {
public:
	Button(std::string text = "");
	Button(std::string text, sf::Texture &texture, sf::IntRect textureRect);

	virtual void init() override;
	virtual void update(sf::Time elapsed) override;

	std::string clickPosition(sf::Vector2f position);

	std::string text = "";
	sf::IntRect baseRect;
	bool showText = true;
	bool enabled = true;

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	float pressTime = 0;

	sf::Sprite buttonSprite;
};

