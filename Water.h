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

	virtual void init() override;
	virtual void update(sf::Time elapsed) override;

	float masterDepth = 0;

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	std::vector<Segment> segments;

	float waveTime = 0;
};

