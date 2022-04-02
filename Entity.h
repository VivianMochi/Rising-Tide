#pragma once

#include <SFML/Graphics.hpp>

class State;

class Entity : public sf::Drawable, public sf::Transformable {
public:
	void setState(State *state);
	virtual void init();
	virtual void update(sf::Time elapsed);

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	State *state = nullptr;
};

