#pragma once

#include "Button.h"

class ButtonBar : public Button {
public:
	ButtonBar(std::string text = "", int segments = 5);

	virtual void init() override;
	virtual void update(sf::Time elapsed) override;

	virtual std::string clickPosition(sf::Vector2f position) override;

	void selectSegment(int segment);

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	std::vector<std::shared_ptr<Button>> buttons;
};

