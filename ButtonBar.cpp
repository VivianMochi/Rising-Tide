#include "ButtonBar.h"

#include "ResourceManager.h"

ButtonBar::ButtonBar(std::string text, int segments) {
	this->text = text;

	for (int i = 0; i < segments; i++) {
		buttons.push_back(std::make_shared<Button>(std::to_string(i), rm::loadTexture("Resource/Image/ButtonBar.png"), sf::IntRect(0, 0, 8, 10)));
	}
}

void ButtonBar::init() {
	// Todo: these buttons never get setState() called
	for (auto &button : buttons) {
		button->init();
	}
}

void ButtonBar::update(sf::Time elapsed) {
	for (int i = 0; i < buttons.size(); i++) {
		buttons[i]->setPosition(getPosition() + sf::Vector2f(i * 9, 0));
		buttons[i]->enabled = enabled;
		buttons[i]->update(elapsed);
	}
}

std::string ButtonBar::clickPosition(sf::Vector2f position) {
	for (auto &button : buttons) {
		std::string result = button->clickPosition(position);
		if (result != "") {
			selectSegment(std::stoi(result));
			return text + result;
		}
	}
	return "";
}

void ButtonBar::selectSegment(int segment) {
	for (int i = 0; i < buttons.size(); i++) {
		buttons[i]->baseRect = sf::IntRect((i == segment ? 7 : 0), 0, 8, 10);
		if (i < segment) {
			//buttons[i]->baseRect = sf::IntRect(14, 0, 8, 10);
		}
	}
}

void ButtonBar::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	for (auto &button : buttons) {
		target.draw(*button, states);
	}
}
