#include "Ocean.h"

#include "ResourceManager.h"
#include "ColorManager.h"

Ocean::Ocean() {
	wetSandLayer.setTexture(rm::loadTexture("Resource/Image/OceanBaseLayer.png"));
	baseLayer.setTexture(rm::loadTexture("Resource/Image/OceanBaseLayer.png"));
	frothSprite.setTexture(rm::loadTexture("Resource/Image/Froth.png"));
	frothShadowSprite.setTexture(rm::loadTexture("Resource/Image/Froth.png"));
	rm::loadTexture("Resource/Image/Froth.png").setRepeated(true);
	frothShadowSprite.setPosition(0, 1);

	// Set up shaders
	shader.loadFromFile("OceanShader.txt", sf::Shader::Fragment);
	wetSandShader.loadFromFile("OceanShader.txt", sf::Shader::Fragment);

	// Use noise for displacement
	rm::loadTexture("Resource/Image/Noise.png").setRepeated(true);
	shader.setUniform("noise", rm::loadTexture("Resource/Image/Noise.png"));
	wetSandShader.setUniform("noise", rm::loadTexture("Resource/Image/Noise.png"));

	// Set graphics to start
	updatePositions();
}

void Ocean::update(sf::Time elapsed) {
	// Update timer
	float lastAnimationTime = animationTime;
	animationTime += elapsed.asSeconds();

	// Update coloration
	wetSandLayer.setColor(cm::getUIColorBright());
	baseLayer.setColor(cm::getWaterColor());
	frothSprite.setColor(cm::getFlashColor());
	frothShadowSprite.setColor(cm::getWaterColor());

	// Update ocean movement only several times per frame, gives it a nice chunky look rather than constantly shifting pixels
	if (fmod(animationTime, 0.05) < fmod(lastAnimationTime, 0.05)) {
		updatePositions();
	}
}

void Ocean::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(getPosition());
	states.shader = &shader;

	sf::RenderStates sandStates = states;
	sandStates.shader = &wetSandShader;

	target.draw(wetSandLayer, sandStates);
	target.draw(baseLayer, states);
	target.draw(frothShadowSprite, states);
	target.draw(frothSprite, states);
}

float Ocean::getHeightFromTime(float time) {
	return 4 + 4 * std::sin(time * 0.5);
}

void Ocean::updatePositions() {
	shader.setUniform("time", animationTime);

	// Update positions
	baseLayer.setPosition(0, getHeightFromTime(animationTime));
	frothSprite.setPosition(baseLayer.getPosition());
	frothShadowSprite.setPosition(baseLayer.getPosition() + sf::Vector2f(0, 1));
	wetSandLayer.setPosition(0, getHeightFromTime(animationTime - 2));

	// Snapshot the wet sand animation when the waves are at their crest
	if (getHeightFromTime(animationTime) < 0.2) {
		wetSandShader.setUniform("time", animationTime);
	}
}
