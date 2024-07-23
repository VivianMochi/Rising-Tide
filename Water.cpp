#include "Water.h"

#include "State.h"
#include "ResourceManager.h"
#include "ColorManager.h"

Water::Water(int totalSegments) {
	// Figure size
	waterTextureSize.x = totalSegments * 2;
	waterTextureSize.y = 135;

	// Create segments
	for (int i = 0; i < totalSegments; i++) {
		segments.emplace_back();
	}
}

Water::Water(int width, int height) {
	waterTextureSize.x = width;
	waterTextureSize.y = height;

	// Create segments
	for (int i = 0; i <= width / 2; i++) {
		segments.emplace_back();
	}
}

void Water::init() {
	
}

void Water::update(sf::Time elapsed) {
	waveTime += elapsed.asSeconds();
	if (waveTime > 2 * 3.14) {
		waveTime -= 2 * 3.14;
	}

	turbulence -= elapsed.asSeconds() / 2.0f;
	if (turbulence < 0) {
		turbulence = 0;
	}
	actualTurbulence += (turbulence - actualTurbulence) * elapsed.asSeconds() * 10.0f;

	// Update segments
	for (int i = 0; i < segments.size(); i++) {
		float depthVariance = (2 - actualTurbulence) * std::sin(waveTime + i / 10.0f);
		float turbulenceVariance = 2 * actualTurbulence * std::sin(-waveTime * 6.0f - i / 3.0f);
		float thisDepth = masterDepth + depthVariance + turbulenceVariance;
		segments[i].depth = thisDepth;
	}
}

void Water::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(getPosition());

	// Todo: case for really high water level, just render a water-colored rectangle

	sf::RenderTexture waterTexture;
	waterTexture.create(waterTextureSize.x, waterTextureSize.y);
	waterTexture.clear(sf::Color(255, 255, 255, 0));

	// Render segments
	sf::Sprite segmentSprite(rm::loadTexture("Resource/Image/Water.png"));
	for (int i = 0; i < segments.size(); i++) {
		segmentSprite.setPosition(i * 2 - 1, waterTextureSize.y - segments[i].depth);
		waterTexture.draw(segmentSprite);

		sf::RectangleShape deepSegmentSprite(sf::Vector2f(2, 320 + segments[i].depth));
		deepSegmentSprite.setPosition(segmentSprite.getPosition() + sf::Vector2f(1, 12));
		deepSegmentSprite.setFillColor(sf::Color(255, 255, 255, 75));
		waterTexture.draw(deepSegmentSprite);
	}

	waterTexture.display();
	sf::Sprite waterSprite(waterTexture.getTexture());
	waterSprite.setColor(cm::getWaterColor());
	target.draw(waterSprite, states);
}
