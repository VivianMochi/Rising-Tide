#pragma once

#include <SFML/Graphics.hpp>

class Ocean : public sf::Drawable, public sf::Transformable {
public:
	Ocean();

	void update(sf::Time elapsed);

private:
	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	float getHeightFromTime(float time);
	void updatePositions();

	float animationTime = 0;

	sf::Sprite wetSandLayer;
	sf::Sprite baseLayer;
	sf::Sprite frothSprite;
	sf::Sprite frothShadowSprite;

	sf::Shader shader;
	sf::Shader wetSandShader;
};

