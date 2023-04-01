#pragma once

#include "Entity.h"
#include <memory>

const bool RENDER_JELLY_OUTLINE = true;

class JellyNode;
class JellyLink;
class Aquarium;

struct JellyNode : public std::enable_shared_from_this<JellyNode> {
	sf::Vector2f position;
	sf::Vector2f velocity;
	std::vector<std::shared_ptr<JellyLink>> links;

	float radius = 2;
	std::string material = "";

	std::shared_ptr<JellyLink> linkNode(std::shared_ptr<JellyNode> otherNode, float desiredLength = 10);
};

struct JellyLink {
	std::weak_ptr<JellyNode> start;
	std::weak_ptr<JellyNode> end;

	float desiredLength = 10;
	float contractStrength = 0;

	sf::Vector2f getCenter();
	sf::Vector2f getPointOn(float percent);
	// Returns the perpendicular to this link, facing right
	sf::Vector2f normal();
};

class Jelly : public Entity {
public:
	virtual void init();
	virtual void update(sf::Time elapsed);

	float jellyRadius = 15;
	float jellyRibs = 5;
	float jellyNodeSpacing = 8;
	float jellyMouth = 15;
	float tentacles = 6;
	float tentacleNodes = 5;

	std::shared_ptr<Aquarium> aquarium;

	sf::IntRect getJellyBounds() const;

	void drawDebug(sf::RenderTarget &target, sf::RenderStates states) const;
private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	bool contracting = false;
	float contractTimer = 0;
	int contractIndex = 0;

	std::vector<std::shared_ptr<JellyNode>> nodes;
	std::vector<std::shared_ptr<JellyNode>> bodyNodes;
	std::shared_ptr<JellyNode> centerNode;
	std::vector<std::shared_ptr<JellyLink>> ribs;
	std::vector<std::shared_ptr<JellyNode>> tentacleRoots;
};

