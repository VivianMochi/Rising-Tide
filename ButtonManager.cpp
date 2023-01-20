#include "ButtonManager.h"

#include "State.h"

void ButtonManager::init() {

}

void ButtonManager::update(sf::Time elapsed) {
	for (std::shared_ptr<Button> &button : buttons) {
		button->update(elapsed);
	}
}

void ButtonManager::initButton(std::shared_ptr<Button> button) {
	button->setState(state);
	button->init();
	buttons.push_back(button);
}

void ButtonManager::addButton(std::shared_ptr<Button> button) {
	buttons.push_back(button);
}

std::string ButtonManager::clickPosition(sf::Vector2f position) {
	for (std::shared_ptr<Button> &button : buttons) {
		std::string result = button->clickPosition(position);
		if (result != "") {
			return result;
		}
	}
	return "";
}

void ButtonManager::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	for (const std::shared_ptr<Button> &button : buttons) {
		target.draw(*button, states);
	}
}
