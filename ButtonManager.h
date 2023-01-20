#pragma once

#include "Entity.h"
#include "Button.h"
#include <vector>

class ButtonManager : public Entity {
public:
	virtual void init() override;
	virtual void update(sf::Time elapsed) override;

	void initButton(std::shared_ptr<Button> button);
	void addButton(std::shared_ptr<Button> button);

	std::string clickPosition(sf::Vector2f position);

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	std::vector<std::shared_ptr<Button>> buttons;
};

