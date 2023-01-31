#include "Water.h"

#include "State.h"
#include "ResourceManager.h"
#include "ColorManager.h"

Water::Water(int totalSegments) {
	// Create segments
	for (int i = 0; i < totalSegments; i++) {
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

	// Update segments
	float accelerationFactor = 20;
	float velocityCap = 10;
	for (int i = 0; i < segments.size(); i++) {
		// Adjust for masterDepth
		float thisDepth = masterDepth + 2 * std::sin(waveTime + i / 10.0f);

		// I was trying to get the water to move like water but I couldn't get it to look right :(
		/*
		if (segments[i].depth < thisDepth) {
			segments[i].depthVelocity += elapsed.asSeconds() * accelerationFactor;
			if (segments[i].depthVelocity > velocityCap) {
				segments[i].depthVelocity = velocityCap;
			}
		}
		else {
			segments[i].depthVelocity -= elapsed.asSeconds() * accelerationFactor;
			if (segments[i].depthVelocity < -velocityCap) {
				segments[i].depthVelocity = -velocityCap;
			}
		}
		// Adjust for adjacent segments
		if (i > 0) {
			segments[i].depthVelocity += segments[i - 1].depth - segments[i].depth;
		}
		if (i < segments.size() - 1) {
			segments[i].depthVelocity += segments[i + 1].depth - segments[i].depth;
		}
		segments[i].depthVelocity *= std::pow(0.75, elapsed.asSeconds());
		segments[i].depth += segments[i].depthVelocity * elapsed.asSeconds();
		*/
		segments[i].depth = thisDepth;
	}
}

void Water::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(getPosition());

	// Render segments
	sf::Sprite segmentSprite(rm::loadTexture("Resource/Image/Water.png"));
	segmentSprite.setColor(cm::getWaterColor());
	for (int i = 0; i < segments.size(); i++) {
		segmentSprite.setPosition(i * 2, -segments[i].depth);
		target.draw(segmentSprite, states);

		sf::RectangleShape deepSegmentSprite(sf::Vector2f(2, 320 + segments[i].depth));
		deepSegmentSprite.setPosition(segmentSprite.getPosition() + sf::Vector2f(0, 10));
		sf::Color deepColor = cm::getWaterColor();
		deepColor.a = 100;
		deepSegmentSprite.setFillColor(deepColor);
		target.draw(deepSegmentSprite, states);
	}
}
