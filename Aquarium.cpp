#include "Aquarium.h"

#include "ColorManager.h"
#include "PlayState.h"

void Aquarium::init() {
	water = Water(size.x, size.y);
	water.setState(state);
	water.init();

	flow.resize(size.x / 10);
	for (auto &column : flow) {
		column.resize(size.y / 10);
	}

	// Randomize flow
	for (int x = 0; x < flow.size(); x++) {
		for (int y = 0; y < flow[x].size(); y++) {
			flow[x][y].x = std::rand() % 21 - 10;
			flow[x][y].y = std::rand() % 21 - 10;
		}
	}

	// Add a test jelly
	jellies["Test"] = std::make_shared<Jelly>();
	jellies["Test"]->setState(state);
	jellies["Test"]->aquarium = shared_from_this();
	jellies["Test"]->init();
	jellies["Test"]->setPosition(150, 150);
	cameraFocus = "Test";

	jellies["Test2"] = std::make_shared<Jelly>();
	jellies["Test2"]->setState(state);
	jellies["Test2"]->aquarium = shared_from_this();
	jellies["Test2"]->init();
	jellies["Test2"]->setPosition(50, 250);
}

void Aquarium::update(sf::Time elapsed) {
	shadeAlpha += ((active ? 1 : 0) - shadeAlpha) * elapsed.asSeconds() * 2.5f;
	aquariumY += ((active ? 0 : 135) - aquariumY) * elapsed.asSeconds() * 2.5f;

	water.masterDepth = size.y - 10;
	water.update(elapsed);

	// Debug randomize flow
	if (DEBUG_ENABLED && sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
		debugView = false;
	}
	if (DEBUG_ENABLED && sf::Keyboard::isKeyPressed(sf::Keyboard::E)) {
		debugView = true;
		for (int x = 0; x < flow.size(); x++) {
			for (int y = 0; y < flow[x].size(); y++) {
				flow[x][y].x = std::rand() % 21 - 10;
				flow[x][y].y = std::rand() % 21 - 10;
			}
		}
	}
	// Debug add flow in corner
	if (DEBUG_ENABLED && sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		addFlow(sf::Vector2f(50, 50), sf::Vector2f(0, -10));
	}

	// Update flow
	std::vector<std::vector<sf::Vector2f>> flowDesired;
	flowDesired.resize(flow.size());
	for (auto &column : flowDesired) {
		column.resize(flow[0].size());
	}
	for (int x = 0; x < flow.size(); x++) {
		for (int y = 0; y < flow[x].size(); y++) {
			// This is a very basic design, all nodes trend towards zero
			flowDesired[x][y] = (getFlowAtGridSpace(sf::Vector2i(x - 1, y)) +
				getFlowAtGridSpace(sf::Vector2i(x + 1, y)) +
				getFlowAtGridSpace(sf::Vector2i(x, y - 1)) +
				getFlowAtGridSpace(sf::Vector2i(x, y + 1))) / 4.0f;
		}
	}
	// Apply flow step
	for (int x = 0; x < flow.size(); x++) {
		for (int y = 0; y < flow[x].size(); y++) {
			flow[x][y] += (flowDesired[x][y] - flow[x][y]) * elapsed.asSeconds();
		}
	}

	// Update particles
	auto particleIter = particles.begin();
	while (particleIter != particles.end()) {
		particleIter->lifespan -= elapsed.asSeconds();
		particleIter->velocity += (getFlow(particleIter->position) - particleIter->velocity) * elapsed.asSeconds() * 2.5f;
		particleIter->position += particleIter->velocity * elapsed.asSeconds();
		if (particleIter->lifespan <= 1) {
			particleIter->alpha -= elapsed.asSeconds();
		}
		else {
			particleIter->alpha += elapsed.asSeconds();
		}
		particleIter->alpha = std::clamp(particleIter->alpha, 0.0f, 1.0f);

		if (particleIter->lifespan <= 0) {
			particleIter = particles.erase(particleIter);
		}
		else {
			particleIter++;
		}
	}
	// Add new particle
	if (std::rand() % 2 == 0) {
		Particle particle;
		particle.position = sf::Vector2f(size.x * (std::rand() % 100) / 100.0f,
			size.y * (std::rand() % 100) / 100.0f);
		particle.velocity = getFlow(particle.position);
		particle.lifespan = 5 + std::rand() % 5;
		particles.push_back(particle);
	}

	// Update jellies
	for (auto &jelly : jellies) {
		jelly.second->update(elapsed);
	}

	// Update camera
	if (active && cameraFocus != "" && jellies[cameraFocus]) {
		desiredCameraPosition = jellies[cameraFocus]->getPosition();
		desiredCameraPosition.y += AQUARIUM_TOP_BUFFER;
		desiredCameraPosition -= (focusTopRight ? sf::Vector2f(164, 36) : sf::Vector2f(120, 68));
	}
	if (!active) {
		desiredCameraPosition.y = 0;
	}
	cameraPosition += (desiredCameraPosition - cameraPosition) * elapsed.asSeconds() * 5.0f;
}

void Aquarium::setActive(bool active) {
	this->active = active;
}

sf::Vector2f Aquarium::getFlow(sf::Vector2f position) const {
	sf::Vector2f gridPosition = position / 10.0f - sf::Vector2f(0.5, 0.5);
	sf::Vector2i closestPoint(std::round(gridPosition.x), std::round(gridPosition.y));
	if (std::abs(gridPosition.x - closestPoint.x) < 0.1 && std::abs(gridPosition.y - closestPoint.y) < 0.1) {
		return getFlowAtGridSpace(closestPoint);
	}
	else {
		// Get the weighted average force of all nearby points
		// Grid cells are 1x1, so we can skip dividing subsquares by the total area
		// Multiply a node's force by the area of the box across from it
		sf::Vector2i topLeft(std::floor(gridPosition.x), std::floor(gridPosition.y));
		sf::Vector2f offset = gridPosition - sf::Vector2f(topLeft);
		sf::Vector2f topLeftInfluence = ((1 - offset.x) * (1 - offset.y)) * getFlowAtGridSpace(topLeft);
		sf::Vector2f topRightInfluence = (offset.x * (1 - offset.y)) * getFlowAtGridSpace(topLeft + sf::Vector2i(1, 0));
		sf::Vector2f bottomLeftInfluence = ((1 - offset.x) * offset.y) * getFlowAtGridSpace(topLeft + sf::Vector2i(0, 1));
		sf::Vector2f bottomRightInfluence = (offset.x * offset.y) * getFlowAtGridSpace(topLeft + sf::Vector2i(1, 1));
		return topLeftInfluence + topRightInfluence + bottomLeftInfluence + bottomRightInfluence;
	}
	return sf::Vector2f(0, 0);
}

sf::Vector2f Aquarium::getFlowAtGridSpace(sf::Vector2i gridSpace) const {
	if (gridSpace.x >= 0 && gridSpace.x < flow.size()) {
		if (gridSpace.y >= 0 && gridSpace.y < flow[0].size()) {
			return flow[gridSpace.x][gridSpace.y];
		}
	}
	// Otherwise it isn't on the grid
	sf::Vector2f result = sf::Vector2f(0, 0);
	if (gridSpace.x < 0) {
		result.x = AQUARIUM_MAX_FLOW;
	}
	else if (gridSpace.x >= flow.size()) {
		result.x = -AQUARIUM_MAX_FLOW;
	}
	if (gridSpace.y < 0) {
		result.y = AQUARIUM_MAX_FLOW;
	}
	else if (gridSpace.y >= flow[0].size()) {
		result.y = -AQUARIUM_MAX_FLOW;
	}
	return result;
}

void Aquarium::addFlow(sf::Vector2f position, sf::Vector2f force) {
	// Todo: influence nearby points rather than closest point
	sf::Vector2f gridPosition = position / 10.0f - sf::Vector2f(0.5, 0.5);
	sf::Vector2i closestPoint(std::round(gridPosition.x), std::round(gridPosition.y));
	for (int dx = -1; dx <= 1; dx++) {
		for (int dy = -1; dy <= 1; dy++) {
			sf::Vector2i editedPoint = closestPoint + sf::Vector2i(dx, dy);
			if (editedPoint.x >= 0 && editedPoint.x < flow.size()) {
				if (editedPoint.y >= 0 && editedPoint.y < flow[0].size()) {
					sf::Vector2f offset = gridPosition - sf::Vector2f(editedPoint);
					float strength = std::pow(0.5, std::sqrt(offset.x * offset.x + offset.y * offset.y));
					flow[editedPoint.x][editedPoint.y] += force * strength;
				}
			}
		}
	}
	

}

void Aquarium::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(getPosition() + sf::Vector2f(0, aquariumY) - cameraPosition);

	sf::RectangleShape shade(sf::Vector2f(240, 135));
	sf::Color shadeColor = cm::getSkyColor();
	shadeColor.a = 255 * shadeAlpha;
	shade.setFillColor(shadeColor);
	target.draw(shade);

	target.draw(water, states);

	states.transform.translate(sf::Vector2f(0, AQUARIUM_TOP_BUFFER));

	// Draw particles
	float particleRadius = 1;
	sf::CircleShape particleSprite;
	particleSprite.setRadius(particleRadius);
	particleSprite.setOrigin(particleRadius, particleRadius);
	for (const auto &particle : particles) {
		sf::Color particleColor = cm::getSkyColor();
		particleColor.a = 200 * particle.alpha;
		particleSprite.setFillColor(particleColor);
		particleSprite.setPosition(particle.position);
		target.draw(particleSprite, states);
	}

	// Draw jellies
	for (auto &jelly : jellies) {
		if (DEBUG_ENABLED && debugView) {
			jelly.second->drawDebug(target, states);
		}
		else {
			target.draw(*jelly.second, states);
		}
	}

	// Debug draw flow
	if (DEBUG_ENABLED && debugView) {
		particleSprite.setFillColor(cm::getJellyColor());
		for (int x = 0; x < flow.size(); x++) {
			for (int y = 0; y < flow[x].size(); y++) {
				sf::Vector2f flowNodePosition = sf::Vector2f(10 * x + 5, 10 * y + 5);
				sf::Vertex flowLine[] = {
					sf::Vertex(flowNodePosition, cm::getFlagColor()),
					sf::Vertex(flowNodePosition + flow[x][y] * 0.5f, cm::getFlagColor())
				};
				target.draw(flowLine, 2, sf::Lines, states);
				particleSprite.setPosition(flowNodePosition);
				target.draw(particleSprite, states);
			}
		}

		// Draw the flow at the cursor point
		if (state) {
			sf::Vector2f cursorPosition = state->getGame()->getCursorPosition();
			cursorPosition.y += -aquariumY - AQUARIUM_TOP_BUFFER;
			cursorPosition += cameraPosition;
			sf::Vertex flowLine[] = {
				sf::Vertex(cursorPosition, cm::getFlagColor()),
				sf::Vertex(cursorPosition + getFlow(cursorPosition), cm::getFlagColor())
			};
			target.draw(flowLine, 2, sf::Lines, states);
			particleSprite.setPosition(cursorPosition);
			target.draw(particleSprite, states);
		}
	}
}
