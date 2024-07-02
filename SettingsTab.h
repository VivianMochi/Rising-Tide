#pragma once

#include "Entity.h"
#include "ButtonManager.h"
#include "Checkbox.h"
#include "ValueAdjuster.h"

// Settings
const std::string settingWindowed = "setting-windowed";
const std::string settingCursor = "setting-cursor";
const std::string settingShake = "setting-shake";
const std::string settingMusic = "setting-music";
const std::string settingSound = "setting-sound";

class SettingsTab : public Entity {
public:
	virtual void init();
	virtual void update(sf::Time elapsed);

	std::string clickPosition(sf::Vector2f position);

	bool open = false;

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	// Construction helpers
	void addCheckbox(std::string setting, sf::Vector2f position);
	void addAdjuster(std::string setting, sf::Vector2f position);

	sf::Sprite backdropSprite;
	sf::Sprite edgeShadowSprite;

	ButtonManager buttons;
	std::shared_ptr<Button> buttonSettings;
	std::map<std::string, std::shared_ptr<Checkbox>> checkboxes;
	std::map<std::string, std::shared_ptr<ValueAdjuster>> adjusters;
};

