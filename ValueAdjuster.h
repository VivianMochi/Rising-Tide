#pragma once

#include "Button.h"
#include "BitmapText.h"

class ValueAdjuster : public Button {
public:
	ValueAdjuster(std::string text = "");

	virtual void init() override;
	virtual void update(sf::Time elapsed) override;

	virtual std::string clickPosition(sf::Vector2f position) override;

	void setValue(int value);

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	BitmapText displayValue;
	std::vector<std::shared_ptr<Button>> buttons;

	int internalValue = 0;
};

