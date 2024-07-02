#include "SettingsTab.h"

#include "ResourceManager.h"
#include "ColorManager.h"
#include "PlayState.h"

void SettingsTab::init() {
	setPosition(0, 135);

	// Setup sprites
	backdropSprite.setTexture(rm::loadTexture("Resource/Image/SettingsTab.png"));
	backdropSprite.setTextureRect(sf::IntRect(0, 12, 240, 53));
	backdropSprite.setPosition(0, 1);

	edgeShadowSprite.setTexture(rm::loadTexture("Resource/Image/SettingsTab.png"));
	edgeShadowSprite.setTextureRect(sf::IntRect(0, 0, 240, 12));

	// Setup buttons
	buttons.setState(state);
	buttons.init();

	buttonSettings = std::make_shared<Button>("Settings", rm::loadTexture("Resource/Image/SettingsButton.png"), sf::IntRect(0, 0, 13, 16));
	buttonSettings->setPosition(2, 4);
	buttons.initButton(buttonSettings);

	addCheckbox(settingWindowed, sf::Vector2f(63, 24));
	addCheckbox(settingCursor, sf::Vector2f(63, 34));
	addCheckbox(settingShake, sf::Vector2f(63, 44));

	addAdjuster(settingMusic, sf::Vector2f(151, 24));
	addAdjuster(settingSound, sf::Vector2f(151, 34));

	// Todo: add checkbox for particles under game/effects section
}

void SettingsTab::update(sf::Time elapsed) {
	// Update tab position
	float desiredY = open ? 81 : 135;
	move(0, (desiredY - getPosition().y) * elapsed.asSeconds() * 5.0f);

	// Update sprite colors
	backdropSprite.setColor(cm::getUIColor());

	// Only update when onscreen
	if (getPosition().y < 130) {
		// Get state info
		PlayState *playState = dynamic_cast<PlayState*>(state);

		// Update buttons
		buttons.update(elapsed);
		buttonSettings->enabled = open;

		// Update checkboxes
		for (auto &entry : checkboxes) {
			entry.second->toggled = playState->saveData[entry.first];
			entry.second->enabled = open;
		}
	}
}

std::string SettingsTab::clickPosition(sf::Vector2f position) {
	if (open) {
		// Adjust click position by transform
		position.y -= getPosition().y;
		return buttons.clickPosition(position);
	}
	return "";
}

void SettingsTab::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(getPosition());

	target.draw(edgeShadowSprite, states);
	target.draw(backdropSprite, states);

	target.draw(buttons, states);
}

void SettingsTab::addCheckbox(std::string setting, sf::Vector2f position) {
	checkboxes[setting] = std::make_shared<Checkbox>(setting);
	checkboxes[setting]->setPosition(position);
	buttons.initButton(checkboxes[setting]);
}

void SettingsTab::addAdjuster(std::string setting, sf::Vector2f position) {
	// Get state info to pull save data from
	PlayState *playState = dynamic_cast<PlayState*>(state);
	if (playState) {
		adjusters[setting] = std::make_shared<ValueAdjuster>(setting);
		adjusters[setting]->setPosition(position);
		buttons.initButton(adjusters[setting]);
		adjusters[setting]->setValue(playState->saveData[setting]);
	}
}
