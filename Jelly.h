#pragma once

#include "Entity.h"

class Jelly : public Entity {
public:
	virtual void init();
	virtual void update(sf::Time elapsed);

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};

