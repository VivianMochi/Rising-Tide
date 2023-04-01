#include "Jelly.h"

#include "ColorManager.h"
#include "PlayState.h"
#include "VectorMath.h"
#include "ResourceManager.h"

void Jelly::init() {
	// Create jelly skeleton
	nodes.clear();
	bodyNodes.clear();
	centerNode = nullptr;
	ribs.clear();
	tentacleRoots.clear();

	// Build jelly body
	centerNode = std::make_shared<JellyNode>();
	nodes.push_back(centerNode);
	std::shared_ptr<JellyNode> previousL;
	std::shared_ptr<JellyNode> previousR;
	for (int i = 0; i < jellyRibs; i++) {
		int nodeX = std::sin(i / jellyRibs * 3.14) * jellyRadius + jellyNodeSpacing;
		int nodeY = -jellyRadius + i / jellyRibs * jellyRadius * 2;

		// Build opposing nodes
		std::shared_ptr<JellyNode> nodeL = std::make_shared<JellyNode>();
		nodeL->position = sf::Vector2f(-nodeX, nodeY);
		nodes.push_back(nodeL);

		std::shared_ptr<JellyNode> nodeR = std::make_shared<JellyNode>();
		nodeR->position = sf::Vector2f(nodeX, nodeY);
		nodes.push_back(nodeR);

		// Build rib and links
		float percentDown = i / (float)jellyRibs;
		float strength = std::sqrt(1 - std::pow(percentDown - 1, 2));
		float lip = 1 - std::pow(percentDown * 0.9, 4);
		float naturalWidth = jellyNodeSpacing * (1 - strength) + jellyRadius * 2 * (strength * lip);
		ribs.push_back(nodeL->linkNode(nodeR, naturalWidth));

		// Link both nodes to center
		centerNode->linkNode(nodeL, jellyRadius);
		centerNode->linkNode(nodeR, jellyRadius);

		// Link nodes to previous
		if (previousL) {
			previousL->linkNode(nodeL, jellyNodeSpacing);
		}
		if (previousR) {
			previousR->linkNode(nodeR, jellyNodeSpacing);
		}

		previousL = nodeL;
		previousR = nodeR;
	}

	// Add all previous nodes to bodyNodes
	for (auto &node : nodes) {
		bodyNodes.push_back(node);
	}

	// Build tentacles
	for (int i = 0; i < tentacles; i++) {
		std::shared_ptr<JellyNode> root = std::make_shared<JellyNode>();
		nodes.push_back(root);
		tentacleRoots.push_back(root);

		std::shared_ptr<JellyNode> connection = root;
		int extraNodes = std::rand() % 4;
		for (int j = 0; j < tentacleNodes + extraNodes; j++) {
			std::shared_ptr<JellyNode> tentacleNode = std::make_shared<JellyNode>();
			tentacleNode->position = sf::Vector2f(0, jellyRadius + 10 + jellyNodeSpacing * j);
			connection->linkNode(tentacleNode, 5);
			nodes.push_back(tentacleNode);

			connection = tentacleNode;
		}
	}
}

void Jelly::update(sf::Time elapsed) {
	// Debug reset
	if (DEBUG_ENABLED && sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
		init();
	}

	// Update rib contraction
	if (!contracting && std::rand() % 100 == 0) {
		contracting = true;
	}
	if (contracting) {
		contractTimer += elapsed.asSeconds();
		if (contractTimer >= 0.2) {
			contractTimer = 0;
			contractIndex += 1;
		}
		if (contractIndex >= jellyRibs) {
			contracting = false;
			contractTimer = 0;
			contractIndex = 0;
		}
	}
	else {
		contractTimer = 0;
		contractIndex = 0;
	}
	
	float totalContraction = 0;
	for (int i = 0; i < jellyRibs; i++) {
		if (i == contractIndex) {
			float percentDown = i / (float)jellyRibs;
			float strength = std::sqrt(1 - std::pow(percentDown - 1, 2));
			ribs[i]->contractStrength = 1 * strength;
		}
		ribs[i]->contractStrength -= elapsed.asSeconds() * 0.5f;
		if (ribs[i]->contractStrength <= 0) {
			ribs[i]->contractStrength = 0;
		}
		totalContraction += ribs[i]->contractStrength;
	}
	if (totalContraction > 0) {
		// Todo: Keep track of total "area" of the jelly, as the area decreases, create jet
		sf::Vector2f jetLocation = ribs.back()->getCenter();
		sf::Vector2f jetDirection = jetLocation;
		jetDirection /= vm::getMagnitude(jetDirection);
		jetLocation += jetDirection * 10.0f;
		float jetForce = 3 * totalContraction;
		aquarium->addFlow(getPosition() + jetLocation, jetDirection * jetForce * elapsed.asSeconds());
		aquarium->addFlow(getPosition(), -jetDirection * jetForce * elapsed.asSeconds());
	}

	// Update links
	for (auto &node : nodes) {
		for (const auto &link : node->links) {
			std::shared_ptr<JellyNode> start = link->start.lock();
			std::shared_ptr<JellyNode> end = link->end.lock();
			sf::Vector2f offset = end->position - start->position;
			float linkLength = vm::getMagnitude(offset);
			offset /= linkLength;
			float desiredLength = link->desiredLength;
			if (link->contractStrength > 0) {
				desiredLength *= (1 - link->contractStrength * 0.25f);
			}
			sf::Vector2f velocityDelta = (desiredLength - linkLength) * -offset * elapsed.asSeconds();
			velocityDelta *= 2 + (link->contractStrength * 2);
			start->velocity += velocityDelta;
			end->velocity -= velocityDelta;
		}
	}
	// Update nodes
	sf::Vector2f averagePosition;
	for (auto &node : nodes) {
		// Add flow force and outward force
		node->velocity += (aquarium->getFlow(getPosition() + node->position) - node->velocity) * elapsed.asSeconds();
		node->position += node->velocity * elapsed.asSeconds();
	}

	// Fix tentacle roots
	for (int i = 0; i < tentacleRoots.size(); i++) {
		float percent = i / (float)(tentacleRoots.size() - 1);
		percent = (-std::cos(percent * 3.14159f) + 1.0f) / 2.0f;
		float percentBuffer = 0.1;
		percent *= (1 - percentBuffer * 2);
		percent += percentBuffer;
		tentacleRoots[i]->position = ribs.back()->getPointOn(percent);
	}

	// Fix average position
	for (auto &bodyNode : bodyNodes) {
		averagePosition += bodyNode->position;
	}
	averagePosition /= (float)bodyNodes.size();
	move(averagePosition);
	for (auto &node : nodes) {
		node->position -= averagePosition;
	}
}

sf::IntRect Jelly::getJellyBounds() const {
	float bufferSpace = 5;
	sf::Vector2i topLeft;
	sf::Vector2i bottomRight;
	for (auto const &node : nodes) {
		if (topLeft.x > node->position.x - bufferSpace) {
			topLeft.x = node->position.x - bufferSpace;
		}
		if (topLeft.y > node->position.y - bufferSpace) {
			topLeft.y = node->position.y - bufferSpace;
		}
		if (bottomRight.x < node->position.x + bufferSpace) {
			bottomRight.x = node->position.x + bufferSpace;
		}
		if (bottomRight.y < node->position.y + bufferSpace) {
			bottomRight.y = node->position.y + bufferSpace;
		}
	}
	return sf::IntRect(topLeft, bottomRight - topLeft);
}

void Jelly::drawDebug(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(getPosition());

	// Draw nodes
	sf::CircleShape nodeSprite;
	nodeSprite.setRadius(2);
	nodeSprite.setOrigin(2, 2);
	nodeSprite.setFillColor(cm::getJellyColor());
	for (const auto &node : nodes) {
		nodeSprite.setPosition(node->position);
		target.draw(nodeSprite, states);
	}
	// Draw links
	for (const auto &node : nodes) {
		for (const auto &link : node->links) {
			sf::Vertex linkLine[] = {
				sf::Vertex(link->start.lock()->position, cm::getFlagColor()),
				sf::Vertex(link->end.lock()->position, cm::getFlagColor())
			};
			target.draw(linkLine, 2, sf::Lines, states);
		}
	}

	// Draw render box
	sf::IntRect bounds = getJellyBounds();
	sf::RectangleShape renderBox;
	renderBox.setPosition(sf::Vector2f(bounds.left, bounds.top));
	renderBox.setSize(sf::Vector2f(bounds.width, bounds.height));
	renderBox.setFillColor(sf::Color::Transparent);
	renderBox.setOutlineColor(sf::Color::Red);
	renderBox.setOutlineThickness(-1);
	target.draw(renderBox, states);

	// Draw hitbox
	sf::CircleShape debugSprite;
	debugSprite.setRadius(10);
	debugSprite.setOrigin(10, 10);
	debugSprite.setOutlineThickness(2);
	debugSprite.setOutlineColor(cm::getJellyColor());
	debugSprite.setFillColor(sf::Color(255, 255, 255, 50));
	target.draw(debugSprite, states);
}

void Jelly::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	// This process involves creating multiple RenderTextures, each containing different jelly parts.
	// The RenderTextures are quantized to integer locations, so we need to keep track of a floating
	// offset vector2f to retain the smooth sub-pixel movement of the jelly components.

	// Set up bounds
	sf::IntRect bounds = getJellyBounds();

	// Set up RenderStates
	sf::Vector2f statesPosition = states.transform.transformPoint(getPosition());
	sf::Vector2f integerPosition = statesPosition;
	integerPosition.x = std::round(integerPosition.x);
	integerPosition.y = std::round(integerPosition.y);
	sf::Vector2f overflow = sf::Vector2f(statesPosition - integerPosition);
	sf::RenderStates integerStates = states;
	integerStates.transform.translate(getPosition() - overflow);
	sf::RenderStates overflowStates;
	overflowStates.transform.translate(sf::Vector2f(-bounds.left, -bounds.top) + overflow);

	// Ensure jelly is on screen
	sf::Vector2i renderBoxPositionOnScreen = sf::Vector2i(integerPosition.x + bounds.left, integerPosition.y + bounds.top);
	if (renderBoxPositionOnScreen.x > 240 || renderBoxPositionOnScreen.x + bounds.width < 0) {
		return;
	}
	if (renderBoxPositionOnScreen.y > 135 || renderBoxPositionOnScreen.y + bounds.height < 0) {
		return;
	}

	// Set up color profiles
	sf::Color jellyColor = cm::getJellyColor();
	jellyColor.a = 100;
	sf::Color jellyOutlineColor = cm::adjustColor(cm::getJellyColor(), sf::Color::Black, 20);
	jellyOutlineColor.a = 150;

	// Set up base shapes
	float blobRadius = 2.5;
	blobRadius = 1.5;
	sf::CircleShape blob = sf::CircleShape(blobRadius, 12);
	blob.setOrigin(blobRadius, blobRadius);
	blob.setFillColor(sf::Color::White);

	// Set up the base layer
	sf::RenderTexture baseLayer;
	baseLayer.create(bounds.width, bounds.height);

	// Draw body shape
	for (int i = 0; i < jellyRibs - 1; i++) {
		sf::ConvexShape chunk;
		chunk.setPointCount(4);
		chunk.setPoint(0, ribs[i]->getPointOn(0));
		chunk.setPoint(1, ribs[i]->getPointOn(1));
		chunk.setPoint(2, ribs[i + 1]->getPointOn(1));
		chunk.setPoint(3, ribs[i + 1]->getPointOn(0));
		chunk.setFillColor(sf::Color::White);
		baseLayer.draw(chunk, overflowStates);
	}

	// Draw tentacles
	for (int i = 0; i < tentacleRoots.size(); i++) {
		std::shared_ptr<JellyNode> currentNode = tentacleRoots[i];
		while (currentNode) {
			// Render this link
			if (!currentNode->links.empty()) {
				// Get the current link
				std::shared_ptr<JellyLink> currentLink = currentNode->links[0];
				if (currentNode != tentacleRoots[i]) {
					if (currentNode->links.size() > 1) {
						currentLink = currentNode->links[1];
					}
					else {
						currentLink = nullptr;
					}
				}

				if (currentLink) {
					// Render link
					for (int j = 0; j < 10; j++) {
						blob.setPosition(currentLink->getPointOn(j / 10.0f));
						baseLayer.draw(blob, overflowStates);
					}

					currentNode = currentLink->end.lock();
				}
				else {
					currentNode = nullptr;
				}
			}
			else {
				currentNode = nullptr;
			}
		}
	}

	// Display the base layer
	baseLayer.display();
	sf::Sprite baseLayerSprite;
	baseLayerSprite.setTexture(baseLayer.getTexture());
	baseLayerSprite.setColor(sf::Color::White);

	// Set up the outline layer
	sf::RenderTexture outlineLayer;
	outlineLayer.create(bounds.width, bounds.height);
	sf::Sprite outlineLayerSprite;
	if (RENDER_JELLY_OUTLINE) {
		// Render the outline
		for (int dy = -1; dy <= 1; dy++) {
			for (int dx = -1; dx <= 1; dx++) {
				if (!(dx == 0 && dy == 0)) {
					baseLayerSprite.setPosition(dx, dy);
					outlineLayer.draw(baseLayerSprite);
				}
			}
		}

		// Erase the inside of the outline
		baseLayerSprite.setPosition(sf::Vector2f());
		sf::RenderStates maskState;
		maskState.blendMode.alphaEquation = sf::BlendMode::Equation::ReverseSubtract;
		outlineLayer.draw(baseLayerSprite, maskState);

		// Display the outline layer
		outlineLayer.display();
		outlineLayerSprite.setTexture(outlineLayer.getTexture());
	}

	// Render all of the finished layer sprites
	baseLayerSprite.setPosition(bounds.left, bounds.top);
	baseLayerSprite.setColor(jellyColor);
	target.draw(baseLayerSprite, integerStates);

	if (RENDER_JELLY_OUTLINE) {
		outlineLayerSprite.setPosition(bounds.left, bounds.top);
		outlineLayerSprite.setColor(jellyOutlineColor);
		target.draw(outlineLayerSprite, integerStates);
	}
}

std::shared_ptr<JellyLink> JellyNode::linkNode(std::shared_ptr<JellyNode> otherNode, float desiredLength) {
	std::shared_ptr<JellyLink> link = std::make_shared<JellyLink>();
	link->start = shared_from_this();
	link->end = otherNode;
	link->desiredLength = desiredLength;
	links.push_back(link);
	otherNode->links.push_back(link);
	return link;
}

sf::Vector2f JellyLink::getCenter() {
	return getPointOn(0.5);
}

sf::Vector2f JellyLink::getPointOn(float percent) {
	std::shared_ptr<JellyNode> start = this->start.lock();
	std::shared_ptr<JellyNode> end = this->end.lock();
	if (start && end) {
		return start->position * (1 - percent) + end->position * percent;
	}
	return sf::Vector2f();
}

sf::Vector2f JellyLink::normal() {
	sf::Vector2f offset = start.lock()->position - end.lock()->position;
	offset /= sqrt(offset.x * offset.x + offset.y * offset.y);
	return sf::Vector2f(offset.y, -offset.x);
}
