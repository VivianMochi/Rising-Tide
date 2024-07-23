#pragma once

#include "Entity.h"
#include <SFML/Graphics.hpp>
#include <vector>

struct Segment {
	float depth = 0;
	float depthVelocity = 0;
};

class Water : public Entity {
public:
	Water(int totalSegments = 50);
	Water(int width, int height);

	virtual void init() override;
	virtual void update(sf::Time elapsed) override;

	float masterDepth = 0;
	float turbulence = 0;

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	std::vector<Segment> segments;

	float waveTime = 0;
	float actualTurbulence = 0;

	sf::Vector2i waterTextureSize;
};

