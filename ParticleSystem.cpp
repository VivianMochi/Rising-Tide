#include "ParticleSystem.h"

#include "ResourceManager.h"

void ParticleSystem::createParticle(sf::Vector2f position, sf::Vector2f velocity, float lifespan, sf::Color color) {
	Particle particle;
	particle.position = position;
	particle.velocity = velocity;
	particle.lifespan = lifespan;
	particle.color = color;
	particles.push_back(particle);
}

void ParticleSystem::update(sf::Time elapsed) {
	for (int i = 0; i < particles.size(); i++) {
		particles[i].lifespan -= elapsed.asSeconds();
		if (particles[i].lifespan <= 0) {
			particles.erase(particles.begin() + i);
			i--;
		}
		else {
			particles[i].position += particles[i].velocity * elapsed.asSeconds();
			particles[i].velocity *= float(std::pow(.5, elapsed.asSeconds()));
		}
	}
}

void ParticleSystem::accelerateParticles(sf::Vector2f delta) {
	for (Particle &particle : particles) {
		particle.velocity += delta;
	}
}

void ParticleSystem::moveParticles(sf::Vector2f delta) {
	for (Particle &particle : particles) {
		particle.position += delta;
	}
}

void ParticleSystem::clearParticles() {
	particles.clear();
}

void ParticleSystem::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	sf::Sprite particleSprite(rm::loadTexture("Resource/Image/Sparkle.png"), sf::IntRect(0, 0, 3, 3));
	for (const Particle &particle : particles) {
		particleSprite.setPosition(particle.position - sf::Vector2f(1, 1));
		particleSprite.setColor(particle.color);
		if (particle.lifespan >= .6) {
			particleSprite.setTextureRect(sf::IntRect(0, 0, 3, 3));
		}
		else if (particle.lifespan >= .3) {
			particleSprite.setTextureRect(sf::IntRect(3, 0, 3, 3));
		}
		else if (particle.lifespan >= .15) {
			particleSprite.setTextureRect(sf::IntRect(0, 0, 3, 3));
		}
		else {
			particleSprite.setTextureRect(sf::IntRect(3, 0, 3, 3));
		}
		target.draw(particleSprite, states);
	}
}

