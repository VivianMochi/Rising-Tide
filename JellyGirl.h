#pragma once

#include "Entity.h"

class JellyGirl : public Entity {
public:
	virtual void init();
	virtual void update(sf::Time elapsed);

	void playWelcomeAnimation();
	void playJoyAnimation(float duration);
	void playAwkwardAnimation(float duration);
	void playTalkAnimation(float duration);

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
	
	void setLayerFrame(int layer, int frame);
	void fullResetAnimation();

	std::vector<sf::Sprite> layers;

	// Animations
	float blinkTimer = 0;
	float bookTimer = 0;
	float wiggleTimer = 0;
	float eyeLockTimer = 0;
	float speedReadTimer = 0;

	// Mouth stuff
	float talkTimer = 0;
	float mouthTimer = 0;

	// Emotions
	float joyTimer = 0;
	float awkwardTimer = 0;
};

