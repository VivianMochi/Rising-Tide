#pragma once

#include <SFML/Graphics.hpp>

struct Particle {
	sf::Vector2f position;
	sf::Vector2f velocity;
	float lifespan;
	sf::Color color;
};

class ParticleSystem : public sf::Drawable, public sf::Transformable {
public:
	void createParticle(sf::Vector2f position, sf::Vector2f velocity, float lifespan, sf::Color color = sf::Color::White);

	void update(sf::Time elapsed);

	void accelerateParticles(sf::Vector2f delta);
	void moveParticles(sf::Vector2f delta);
	void clearParticles();

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	std::vector<Particle> particles;
};
