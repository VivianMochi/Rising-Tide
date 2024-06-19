#pragma once

#include "Button.h"

class Checkbox : public Button {
public:
	Checkbox(std::string text = "");

	virtual void init() override;
	virtual void update(sf::Time elapsed) override;

	bool toggled = false;

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	sf::Sprite boxSprite;
};

