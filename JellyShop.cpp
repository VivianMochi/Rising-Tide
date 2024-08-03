#include "JellyShop.h"

#include "ResourceManager.h"
#include "RenderAssist.h"
#include "ColorManager.h"
#include "PlayState.h"
#include <memory>
#include <algorithm>

void JellyShop::init() {
	shopData[shopSpeedy] = { 0, 50, "Speedy", "Speedy Mode",
		"Tide rises\nover time.\nBe quick!" };
	shopData[shopEndless] = { 0, 200, "Endless", "Endless Mode",
		"Tide removes\nrows, new\nrows appear!\nIt never ends..." };
	shopData[shopPaletteSelect] = { 1, 100, "Palettes", "Palette Select",
		"Choose your\nfavorite\ncolor schemes!" };
	shopData[shopMusicPlayer] = { 1, 250, "Music", "Music Player",
		"Play your\nfavorite songs!" };
	shopData[shopPalettePbj] = { 2, 100, "PB&J", "PB&J Palette",
		"Made with\njellyfish\njelly!" };
	shopData[shopPaletteCream] = { 2, 200, "Cream", "Cream Palette",
		"Mmmmmm...\nCreamy!" };
	shopData[shopPaletteWatermelon] = { 2, 300, "Melon", "Watermelon Palette",
		"Juicy and\nrefreshing!" };
	shopData[shopPaletteSpicy] = { 2, 400, "Spicy", "Spicy Palette",
		"Spicy!" };
	shopData[shopPaletteMoon] = { 2, 500, "Moon", "Moon Palette",
		"Aurelia's\nfavorite!" };
	shopData[shopPaletteCustom] = { 2, 9000, "Custom", "Custom Palette",
		"Build your own!" };

	buttons.setState(state);
	buttons.init();
	shopRowManager.setState(state);
	shopRowManager.init();

	buttonMenu = std::make_shared<Button>("Menu", rm::loadTexture("Resource/Image/MenuButton.png"), sf::IntRect(0, 0, 41, 16));
	buttons.initButton(buttonMenu);
	buttonBuy = std::make_shared<Button>("Buy", rm::loadTexture("Resource/Image/Button40.png"), sf::IntRect(0, 0, 40, 16));
	buttonBuy->showText = true;
	buttons.initButton(buttonBuy);
	buttonSettings = std::make_shared<Button>("Settings", rm::loadTexture("Resource/Image/SettingsButton.png"), sf::IntRect(0, 0, 13, 16));
	buttons.initButton(buttonSettings);

	leftPane.setTexture(rm::loadTexture("Resource/Image/ShopLeftPane.png"));
	leftPane.setPosition(-89, 0);
	leftBackdrop.setTexture(rm::loadTexture("Resource/Image/ShopLeftBackdrop.png"));
	leftBackdrop.setPosition(-89, 19);
	detailsPane.setTexture(rm::loadTexture("Resource/Image/ShopDetailsPane.png"));
	detailsPane.setPosition(240, 73);

	// Build shop rows
	for (auto &data : shopData) {
		if (data.second.section + 1 > shopRows.size()) {
			shopRows.resize(data.second.section + 1);
		}
		shopRows[data.second.section].push_back(std::make_shared<ShopRow>(data.first, data.second.buttonName, data.second.cost));
		shopRowManager.initButton(shopRows[data.second.section].back());
	}
	for (auto &section : shopRows) {
		std::sort(section.begin(), section.end(), [](std::shared_ptr<ShopRow> &rowA, std::shared_ptr<ShopRow> &rowB) { return rowA->cost < rowB->cost; });
	}

	// Initialize Aurelia & speech bubble
	aurelia.setState(state);
	aurelia.init();
	aurelia.setPosition(500, 500);
	speechBubble.setTexture(rm::loadTexture("Resource/Image/SpeechBubble.png"));
	speechBubble.setPosition(500, 500);
}

void JellyShop::update(sf::Time elapsed) {
	float desiredX = -89;
	if (shopActive && !aquariumFocused) {
		desiredX = 0;
	}
	leftPane.move((sf::Vector2f(desiredX, 0) - leftPane.getPosition()) * elapsed.asSeconds() * 5.0f);
	leftPane.setColor(cm::getUIColor());
	leftBackdrop.setPosition(leftPane.getPosition() + sf::Vector2f(0, 19));
	leftBackdrop.setColor(cm::getUIColor());

	desiredX = 240;
	if (shopActive && !aquariumFocused) {
		desiredX = 88;
	}
	detailsPane.move((sf::Vector2f(desiredX, 72) - detailsPane.getPosition()) * elapsed.asSeconds() * 5.0f);
	detailsPane.setColor(cm::getUIColor());

	// Update scroll
	if (desiredScroll < 0) {
		desiredScroll = 0;
	}
	if (desiredScroll > scrollMax) {
		desiredScroll = scrollMax;
	}
	scroll += (desiredScroll - scroll) * elapsed.asSeconds() * 15.0f;

	// Update rows
	PlayState *playState = dynamic_cast<PlayState*>(state);
	int offset = 20;
	for (int section = 0; section < shopRows.size(); section++) {
		for (int i = 0; i < shopRows[section].size(); i++) {
			if (i == 0) {
				offset += 10;
			}
			shopRows[section][i]->setPosition(leftPane.getPosition() + sf::Vector2f(0, offset - scroll));
			shopRows[section][i]->selected = shopRows[section][i]->text == selection;
			if (playState) {
				shopRows[section][i]->purchased = playState->saveData[shopRows[section][i]->text];
			}
			offset += 10;
		}
	}
	shopRowManager.update(elapsed);

	// Update max scroll
	scrollMax = offset - 113;

	// Update buttons
	buttonMenu->setPosition(leftPane.getPosition() + sf::Vector2f(2, 117));
	buttonSettings->setPosition(leftPane.getPosition() + sf::Vector2f(45, 117));
	buttonBuy->setPosition(detailsPane.getPosition() + sf::Vector2f(110, 45));
	buttonBuy->enabled = false;
	buttonBuy->text = "Buy";
	if (selection != "") {
		if (playState) {
			if (!playState->saveData[selection]) {
				buttonBuy->enabled = true;
			}
			else {
				buttonBuy->text = "Paid";
			}
		}
	}
	buttons.update(elapsed);

	// Update Aurelia
	aurelia.setPosition(detailsPane.getPosition() + sf::Vector2f(99, -62));
	aurelia.update(elapsed);

	// Update speech bubble
	sf::Vector2f desiredPosition = detailsPane.getPosition() + sf::Vector2f(3, -18);
	if (speechTime <= 0) {
		desiredPosition = detailsPane.getPosition() + sf::Vector2f(3, 3);
	}
	speechBubble.move((desiredPosition - speechBubble.getPosition()) * elapsed.asSeconds() * 10.0f);
	if (speechBubble.getPosition().x < desiredPosition.x) {
		// Speech bubble can never be left of its target, helps shoo it away when shop is closed
		speechBubble.move(desiredPosition.x - speechBubble.getPosition().x, 0);
	}
	speechBubble.setColor(cm::getFlashColor());
	if (speechTime > 0) {
		speechTime -= elapsed.asSeconds();
	}

	// Update aquarium
	if (shopActive) {
		// Todo: this logic should be somewhere cleaner
		if (aquariumFocused && sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
			aquariumFocused = false;
		}

		aquarium->focusTopRight = !aquariumFocused;

		// Todo: implement mouse panning
		if (aquariumFocused) {
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
				aquarium->cameraFocus = "";
				aquarium->desiredCameraPosition.y -= elapsed.asSeconds() * 100.f;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
				aquarium->cameraFocus = "";
				aquarium->desiredCameraPosition.x -= elapsed.asSeconds() * 100.f;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
				aquarium->cameraFocus = "";
				aquarium->desiredCameraPosition.y += elapsed.asSeconds() * 100.f;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
				aquarium->cameraFocus = "";
				aquarium->desiredCameraPosition.x += elapsed.asSeconds() * 100.f;
			}
		}
		else {
			aquarium->cameraFocus = "Test";
		}
	}
}

void JellyShop::setActive(bool active) {
	this->shopActive = active;
	desiredScroll = 0;
	aquarium->focusTopRight = active;
	if (active) {
		selection = "";
		aquariumFocused = false;
		aurelia.playWelcomeAnimation();
		speak("Welcome!", 3);
	}
}

std::string JellyShop::clickPosition(sf::Vector2f position) {
	std::string clickedButton = buttons.clickPosition(position);
	PlayState *playState = dynamic_cast<PlayState*>(state);

	if (clickedButton == "Buy" && selection != "" && playState && !playState->saveData[selection]) {
		// Do purchase logic
		if (playState->saveData[totalJellies] >= shopData[selection].cost) {
			playState->saveData[totalJellies] -= shopData[selection].cost;
			playState->saveData[selection] = 1;
			playState->save();

			aurelia.playJoyAnimation(3);
			aurelia.playTalkAnimation(1);
			speak("Thank you!!", 3);
		}
		else {
			aurelia.playAwkwardAnimation(3);
			aurelia.playTalkAnimation(1);
			speak("Need more $", 3);
		}
	}
	else if (clickedButton == "" && position.x >= 89 && position.y <= 72) {
		// Focused the aquarium
		aquariumFocused = true;
	}
	else if (clickedButton == "" && position.y >= 19 && position.y <= 114) {
		// Clicked a shop row
		clickedButton = shopRowManager.clickPosition(position);
		if (shopData.count(clickedButton) > 0) {
			selection = clickedButton;
		}
	}

	return clickedButton;
}

void JellyShop::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	BitmapText text;
	text.setTexture(rm::loadTexture("Resource/Image/Font.png"));
	text.setColor(cm::getUIColorDark());

	// Render Aurelia and speech bubble
	if (speechText != "") {
		target.draw(speechBubble);
		text.setColor(cm::getJellyColor());
		text.setText(speechText);
		text.setPosition(speechBubble.getPosition() + sf::Vector2f(47 - text.getWidth() / 2, 3));
		target.draw(text);
	}
	target.draw(aurelia);

	target.draw(detailsPane);

	target.draw(leftBackdrop);
	target.draw(shopRowManager);

	// Render headers
	for (int i = 0; i < sections.size(); i++) {
		if (shopRows.size() > i && !shopRows[i].empty()) {
			text.setColor(cm::getFlagColor());
			text.setText("<" + sections[i] + ">");
			text.setPosition(leftPane.getPosition().x + 82 / 2 - text.getWidth() / 2, shopRows[i][0]->getPosition().y - 9);
			target.draw(text);
		}
	}

	// Render scroll bar
	float scrollPercent = scroll / scrollMax;
	sf::Sprite scrollBar(rm::loadTexture("Resource/Image/ScrollBar.png"));
	scrollBar.setColor(cm::getUIColor());
	scrollBar.setPosition(leftBackdrop.getPosition() + sf::Vector2f(82, 2 + 69 * scrollPercent));
	target.draw(scrollBar);

	target.draw(leftPane);

	// Render jelly count
	PlayState *playState = dynamic_cast<PlayState*>(state);
	if (playState) {
		text.setColor(cm::getUIColorDark());
		text.setText("$" + std::to_string(std::clamp(playState->saveData[totalJellies], 0, 9999)));
		text.setPosition(leftPane.getPosition() + sf::Vector2f(66 - text.getWidth() / 2, 5));
		target.draw(text);
	}

	// Render selected details
	std::string titleText = "";
	std::string descriptionText = "";
	if (selection != "") {
		titleText = shopData.at(selection).fullName;
		descriptionText = shopData.at(selection).description;

		// Render cost
		text.setColor(cm::getUIColorDark());
		if (playState && playState->saveData[selection]) {
			text.setColor(cm::getUIColorMedium());
		}
		text.setText("$" + std::to_string(shopData.at(selection).cost));
		text.setPosition(detailsPane.getPosition() + sf::Vector2f(130 - text.getWidth() / 2, 33));
		target.draw(text);
	}
	// Render selection name
	text.setColor(cm::getUIColorDark());
	text.setText(titleText);
	text.setPosition(detailsPane.getPosition() + sf::Vector2f(6, 6));
	target.draw(text);

	// Render detail text
	text.setColor(cm::getUIColorBright());
	int offset = 21;
	std::string temp = descriptionText;
	int extraLines = std::count(temp.begin(), temp.end(), '\n');
	offset = 21 + ((3 - extraLines) / 3.0) * 15;
	while (temp.find("\n") != -1) {
		text.setText(temp.substr(0, temp.find("\n")));
		text.setPosition(detailsPane.getPosition() + sf::Vector2f(55 - text.getWidth() / 2, offset));
		target.draw(text);
		temp = temp.substr(temp.find("\n") + 1);
		offset += 10;
	}
	text.setText(temp);
	text.setPosition(detailsPane.getPosition() + sf::Vector2f(55 - text.getWidth() / 2, offset));
	target.draw(text);

	target.draw(buttons);
}

void JellyShop::speak(std::string text, float duration) {
	speechBubble.setPosition(detailsPane.getPosition() + sf::Vector2f(240, -18));
	speechText = text;
	speechTime = duration;
}

