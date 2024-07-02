#pragma once

#include "Entity.h"
#include <SFML/Graphics.hpp>

class Title : public Entity {
public:
	virtual void init() override;
	virtual void update(sf::Time elapsed) override;

private:
	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	sf::Sprite backdropSprite;
	sf::Sprite topTextSprite;
	sf::Sprite bottomTextSprite;

	// Shader stuff
	sf::Shader waterShader;
	float waterTime = 0;
};

