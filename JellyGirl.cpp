#include "JellyGirl.h"

#include "ResourceManager.h"

const sf::Vector2i SPRITE_SIZE = sf::Vector2i(58, 62);

const int BG_LAYER = 0;
const int BODY_LAYER = 1;
const int HEAD_LAYER = 2;
const int MOUTH_LAYER = 3;
const int EYE_LAYER = 4;
const int JELLY_LAYER = 5;
const int BOOK_LAYER = 6;
const int ARM_LAYER = 7;

void JellyGirl::init() {
	for (int i = 0; i <= ARM_LAYER; i++) {
		layers.emplace_back();
		layers.back().setTexture(rm::loadTexture("Resource/Image/JellyGirl.png"));
		setLayerFrame(i, 0);
	}
}

void JellyGirl::update(sf::Time elapsed) {
	bool reading = false;
	if (eyeLockTimer > 0) {
		eyeLockTimer -= elapsed.asSeconds();
		if (eyeLockTimer <= 0) {
			setLayerFrame(EYE_LAYER, 0);
		}
	}

	if (joyTimer > 0) {
		// Joy control
		joyTimer -= elapsed.asSeconds();
		setLayerFrame(EYE_LAYER, std::fmod(joyTimer, 0.2) < 0.1 ? 5 : 4);
		setLayerFrame(JELLY_LAYER, std::fmod(joyTimer, 0.2) < 0.1 ? 5 : 4);
		if (joyTimer <= 0) {
			setLayerFrame(EYE_LAYER, 0);
			setLayerFrame(JELLY_LAYER, 0);
		}
	}
	else if (awkwardTimer > 0) {
		// Awkward control
		awkwardTimer -= elapsed.asSeconds();
		if (awkwardTimer <= 0) {
			setLayerFrame(EYE_LAYER, 0);
		}
	}
	else if (eyeLockTimer <= 0) {
		reading = true;
		// Blink control
		blinkTimer -= elapsed.asSeconds();
		if (blinkTimer <= -0.1) {
			blinkTimer += std::rand() % 2 + 4.3;
			setLayerFrame(EYE_LAYER, 0);
		}
		else if (blinkTimer <= 0) {
			setLayerFrame(EYE_LAYER, 1);
		}
	}

	// Page turn control
	if (reading) {
		bookTimer -= elapsed.asSeconds();
		speedReadTimer -= elapsed.asSeconds();
		if (speedReadTimer > 0) {
			bookTimer -= elapsed.asSeconds();
			if (bookTimer > 0.1) {
				bookTimer = 0.1;
			}
		}
		if (bookTimer <= -0.4) {
			setLayerFrame(BOOK_LAYER, 0);
			bookTimer += speedReadTimer > 0 ? 0.1 : std::rand() % 3 + 6;
		}
		else if (bookTimer <= -0.3) {
			setLayerFrame(BOOK_LAYER, 4);
		}
		else if (bookTimer <= -0.2) {
			setLayerFrame(BOOK_LAYER, 3);
		}
		else if (bookTimer <= -0.1) {
			setLayerFrame(BOOK_LAYER, 2);
		}
		else if (bookTimer <= 0) {
			setLayerFrame(BOOK_LAYER, 1);
		}
	}

	// Wiggle animation control
	if (wiggleTimer > 0) {
		wiggleTimer -= elapsed.asSeconds();
		float frameFloat = std::fmod(wiggleTimer, 0.25);
		setLayerFrame(HEAD_LAYER, 1 + std::floor(frameFloat * 4 / 0.25));
		if (wiggleTimer < 0) {
			setLayerFrame(HEAD_LAYER, 0);
		}
	}

	// Mouth control
	if (talkTimer > 0) {
		talkTimer -= elapsed.asSeconds();
		mouthTimer -= elapsed.asSeconds();
		if (mouthTimer <= -0.1) {
			// Open mouth based on emotion
			mouthTimer += 0.2;
			if (joyTimer > 0) {
				setLayerFrame(MOUTH_LAYER, 3);
			}
			else if (awkwardTimer > 0) {
				setLayerFrame(MOUTH_LAYER, 2);
			}
			else {
				setLayerFrame(MOUTH_LAYER, 1);
			}
		}
		else if (mouthTimer <= 0) {
			setLayerFrame(MOUTH_LAYER, 0);
		}
	}
	else if (mouthTimer > 0) {
		// Close mouth when ready
		mouthTimer -= elapsed.asSeconds();
		if (mouthTimer <= 0) {
			setLayerFrame(MOUTH_LAYER, 0);
		}
	}
}

void JellyGirl::playWelcomeAnimation() {
	fullResetAnimation();
	wiggleTimer = 3;
	playTalkAnimation(1.5);
	setLayerFrame(EYE_LAYER, 1);
	eyeLockTimer += 3;

	// Also set up other animation timers to play after
	blinkTimer = 0.8;
	bookTimer = 2.5;
}

void JellyGirl::playJoyAnimation(float duration) {
	fullResetAnimation();
	setLayerFrame(EYE_LAYER, 4);
	joyTimer = duration;
	wiggleTimer = duration;
}

void JellyGirl::playAwkwardAnimation(float duration) {
	fullResetAnimation();
	setLayerFrame(EYE_LAYER, 2);
	awkwardTimer = duration;
}

void JellyGirl::playTalkAnimation(float duration) {
	talkTimer = duration;
}

void JellyGirl::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(getPosition());

	for (int i = 0; i < layers.size(); i++) {
		target.draw(layers[i], states);
	}
}

void JellyGirl::setLayerFrame(int layer, int frame) {
	if (layer >= 0 && layer < layers.size()) {
		layers[layer].setTextureRect(sf::IntRect(SPRITE_SIZE.x * frame, SPRITE_SIZE.y * layer, SPRITE_SIZE.x, SPRITE_SIZE.y));
	}
}

void JellyGirl::fullResetAnimation() {
	for (int i = 0; i < layers.size(); i++) {
		setLayerFrame(i, 0);
	}
	wiggleTimer = 0;
	joyTimer = 0;
	awkwardTimer = 0;
}
