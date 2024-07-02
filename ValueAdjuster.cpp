#include "ValueAdjuster.h"

#include "ResourceManager.h"
#include "ColorManager.h"

ValueAdjuster::ValueAdjuster(std::string text) {
	this->text = text;

	buttons.push_back(std::make_shared<Button>("Left", rm::loadTexture("Resource/Image/MiniButton.png"), sf::IntRect(0, 0, 7, 8)));
	buttons.push_back(std::make_shared<Button>("Right", rm::loadTexture("Resource/Image/MiniButton.png"), sf::IntRect(14, 0, 7, 8)));
}

void ValueAdjuster::init() {
	displayValue.setTexture(rm::loadTexture("Resource/Image/Font.png"));
	displayValue.setColor(cm::getUIColorMedium());

	for (auto &button : buttons) {
		button->init();
	}
}

void ValueAdjuster::update(sf::Time elapsed) {
	// Update buttons
	buttons[0]->setPosition(getPosition());
	buttons[0]->enabled = enabled && internalValue > 0;
	buttons[0]->update(elapsed);
	buttons[1]->setPosition(getPosition() + sf::Vector2f(15, 0));
	buttons[1]->enabled = enabled && internalValue < 9;
	buttons[1]->update(elapsed);

	// Update display
	displayValue.setPosition(getPosition() + sf::Vector2f(11 - displayValue.getWidth() / 2, 0));
	displayValue.setColor(cm::getUIColorMedium());
}

std::string ValueAdjuster::clickPosition(sf::Vector2f position) {
	for (auto &button : buttons) {
		std::string result = button->clickPosition(position);
		if (result == "Left") {
			setValue(internalValue - (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ? 10 : 1));
			return text + std::to_string(internalValue);
		}
		else if (result == "Right") {
			setValue(internalValue + (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) ? 10 : 1));
			return text + std::to_string(internalValue);
		}
	}
	return "";
}

void ValueAdjuster::setValue(int value) {
	internalValue = std::clamp(value, 0, 9);
	displayValue.setText(std::to_string(internalValue));
}

void ValueAdjuster::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	target.draw(displayValue, states);
	
	for (auto &button : buttons) {
		target.draw(*button, states);
	}
}
