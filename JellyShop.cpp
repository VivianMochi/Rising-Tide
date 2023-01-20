#include "JellyShop.h"

#include "ResourceManager.h"
#include "RenderAssist.h"
#include "ColorManager.h"
#include "PlayState.h"
#include <memory>
#include <algorithm>

void JellyShop::init() {
	shopData[shopSpeedy] = { 0, 50, "Speedy", "Speedy Mode",
		"Tide rises with time\ninstead of digs.\nBe quick!" };
	shopData[shopEndless] = { 0, 200, "Endless", "Endless Mode",
		"Dear god\nit never ends..." };
	shopData[shopPaletteSelect] = { 1, 100, "Palettes", "Palette Select",
		"Choose your favorite\ncolor schemes!" };
	shopData[shopMusicPlayer] = { 1, 250, "Music", "Music Player",
		"Play your\nfavorite songs!" };
	shopData[shopPalettePbj] = { 2, 100, "PB&J", "PB&J Palette",
		"Made with only\nthe finest\njellyfish jelly!" };
	shopData[shopPaletteCream] = { 2, 200, "Cream", "Cream Palette",
		"Mmmmmm...\nCreamy!" };
	shopData[shopPaletteWatermelon] = { 2, 300, "Melon", "Watermelon Palette",
		"Make sure\nyou don't\neat the seeds!" };
	shopData[shopPaletteSpicy] = { 2, 400, "Spicy", "Spicy Palette",
		"Spicy!" };
	shopData[shopPaletteMoon] = { 2, 500, "Moon", "Moon Palette",
		"Just like a\nfull moon!\nUnlocks Moon Jelly" };
	shopData[shopPaletteCustom] = { 2, 1000, "Custom", "Custom Palette",
		"Build your own!" };

	buttons.setState(state);
	buttons.init();
	shopRowManager.setState(state);
	shopRowManager.init();

	buttonMenu = std::make_shared<Button>("Menu");
	buttons.initButton(buttonMenu);
	buttonBuy = std::make_shared<Button>("Buy");
	buttons.initButton(buttonBuy);

	leftPane.setTexture(rm::loadTexture("Resource/Image/ShopLeftPane.png"));
	leftPane.setPosition(-89, 0);
	leftBackdrop.setTexture(rm::loadTexture("Resource/Image/ShopLeftBackdrop.png"));
	leftBackdrop.setPosition(-89, 16);
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
}

void JellyShop::update(sf::Time elapsed) {
	float desiredX = -89;
	if (shopActive) {
		desiredX = 0;
	}
	leftPane.move((sf::Vector2f(desiredX, 0) - leftPane.getPosition()) * elapsed.asSeconds() * 5.0f);
	leftPane.setColor(cm::getUIColor());
	leftBackdrop.setPosition(leftPane.getPosition() + sf::Vector2f(0, 16));
	leftBackdrop.setColor(cm::getUIColor());

	desiredX = 240;
	if (shopActive) {
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
	scroll += (desiredScroll - scroll) * elapsed.asSeconds() * 10.f;

	// Update rows
	PlayState *playState = dynamic_cast<PlayState*>(state);
	int offset = 17;
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
	scrollMax = offset - 116;

	// Update buttons
	buttonMenu->setPosition(leftPane.getPosition() + sf::Vector2f(2, 119));
	buttonBuy->setPosition(detailsPane.getPosition() + sf::Vector2f(48, 47));
	buttonBuy->enabled = false;
	if (selection != "" && playState && !playState->saveData[selection]) {
		buttonBuy->enabled = true;
	}
	buttons.update(elapsed);
}

void JellyShop::setActive(bool active) {
	this->shopActive = active;
	desiredScroll = 0;
	if (active) {
		selection = "";
	}
}

std::string JellyShop::clickPosition(sf::Vector2f position) {
	std::string clickedButton = buttons.clickPosition(position);
	PlayState *playState = dynamic_cast<PlayState*>(state);

	if (clickedButton == "Buy" && selection != "" && playState && !playState->saveData[selection]) {
		if (playState->saveData[totalJellies] >= shopData[selection].cost) {
			playState->saveData[totalJellies] -= shopData[selection].cost;
			playState->saveData[selection] = 1;
			playState->save();
		}
	}
	else if (clickedButton == "" && position.y >= 16 && position.y <= 116) {
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
	text.setColor(cm::getTextColor());

	target.draw(detailsPane);

	target.draw(leftBackdrop);
	target.draw(shopRowManager);

	// Render headers
	for (int i = 0; i < sections.size(); i++) {
		if (shopRows.size() > i && !shopRows[i].empty()) {
			text.setColor(cm::getFlagColor());
			text.setText(sections[i]);
			text.setPosition(leftPane.getPosition().x + 82 / 2 - text.getWidth() / 2, shopRows[i][0]->getPosition().y - 9);
			target.draw(text);
		}
	}

	// Render scroll bar
	float scrollPercent = scroll / scrollMax;
	sf::RectangleShape scrollBar(sf::Vector2f(4, 20));
	scrollBar.setFillColor(cm::getDisabledTextColor());
	scrollBar.setPosition(leftPane.getPosition() + sf::Vector2f(82, 16 + 2 + 77 * scrollPercent));
	target.draw(scrollBar);

	target.draw(leftPane);

	// Render jelly count
	PlayState *playState = dynamic_cast<PlayState*>(state);
	if (playState) {
		ra::renderJelly(target, states, leftPane.getPosition() + sf::Vector2f(3, 3));
		text.setColor(cm::getTextColor());
		text.setText(std::to_string(playState->saveData[totalJellies]));
		text.setPosition(leftPane.getPosition() + sf::Vector2f(21, 4));
		target.draw(text);
	}

	// Render selected text
	if (selection != "") {
		text.setColor(cm::getTextColor());
		text.setText(shopData.at(selection).fullName);
		text.setPosition(detailsPane.getPosition() + sf::Vector2f(3, 3));
		target.draw(text);

		text.setColor(cm::getDisabledTextColor());
		int offset = 15;
		std::string temp = shopData.at(selection).description;
		while (temp.find("\n") != -1) {
			text.setText(temp.substr(0, temp.find("\n")));
			text.setPosition(detailsPane.getPosition() + sf::Vector2f(76 - text.getWidth() / 2, offset));
			target.draw(text);
			temp = temp.substr(temp.find("\n") + 1);
			offset += 10;
		}
		text.setText(temp);
		text.setPosition(detailsPane.getPosition() + sf::Vector2f(76 - text.getWidth() / 2, offset));
		target.draw(text);
	}

	target.draw(buttons);
}

