#pragma once

#include "Button.h"

class ShopRow : public Button {
public:
	ShopRow(std::string text = "", std::string displayText = "", int cost = 10);

	virtual void init() override;
	virtual void update(sf::Time elapsed) override;

	virtual std::string clickPosition(sf::Vector2f position) override;

	int cost = 10;
	bool selected = false;
	bool purchased = false;

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	std::string displayText;
};

