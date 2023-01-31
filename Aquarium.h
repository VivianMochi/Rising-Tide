#pragma once

#include "Entity.h"
#include "Water.h"
#include "Jelly.h"

const int AQUARIUM_TOP_BUFFER = 20;
const float AQUARIUM_MAX_FLOW = 10;

struct Particle {
	sf::Vector2f position;
	sf::Vector2f velocity;
	float lifespan = 0;
	float alpha = 0;
};

class Aquarium : public Entity {
public:
	virtual void init();
	virtual void update(sf::Time elapsed);

	void setActive(bool active);

	sf::Vector2f getFlow(sf::Vector2f position, bool gridSpace = false) const;

	std::string cameraFocus = "";
	sf::Vector2f desiredCameraPosition;
	sf::Vector2f cameraPosition;
	bool focusTopRight = false;

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	bool active = false;
	float shadeAlpha = 0;
	float aquariumY = 135;
	sf::Vector2f size = { 500, 300 };
	std::vector<std::vector<sf::Vector2f>> flow;

	std::map<std::string, std::shared_ptr<Jelly>> jellies;
	std::vector<Particle> particles;

	Water water;
};

