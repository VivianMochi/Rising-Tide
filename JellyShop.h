#pragma once

#include "Entity.h"
#include "ButtonManager.h"
#include "ButtonBar.h"
#include "ShopRow.h"
#include "Aquarium.h"
#include <map>

const std::vector<std::string> sections = { "Modes", "Extras", "Palettes" };

// Shop modes
const std::string shopSpeedy = "shop-speedy";
const std::string shopEndless = "shop-endless";
// Shop extras
const std::string shopPaletteSelect = "shop-palette";
const std::string shopMusicPlayer = "shop-music";
// Shop palettes
const std::string shopPalettePbj = "shop-palette-pbj";
const std::string shopPaletteCream = "shop-palette-cream";
const std::string shopPaletteWatermelon = "shop-palette-watermelon";
const std::string shopPaletteSpicy = "shop-palette-spicy";
const std::string shopPaletteMoon = "shop-palette-moon";
const std::string shopPaletteCustom = "shop-palette-custom";

struct ShopData {
	int section = 0;
	int cost = 10;
	std::string buttonName = "Error";
	std::string fullName = "Error";
	std::string description = "Undefined";
};

class JellyShop : public Entity {
public:
	virtual void init() override;
	virtual void update(sf::Time elapsed) override;

	void setActive(bool active);

	std::string clickPosition(sf::Vector2f position);

	float desiredScroll = 0;

	std::shared_ptr<Aquarium> aquarium;

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	std::map<std::string, ShopData> shopData;

	bool shopActive = false;
	float scroll = 0;
	float scrollMax = 0;
	std::string selection = "";
	bool aquariumFocused = false;

	ButtonManager buttons;
	std::shared_ptr<Button> buttonMenu;
	std::shared_ptr<Button> buttonBuy;
	ButtonManager shopRowManager;
	std::vector<std::vector<std::shared_ptr<ShopRow>>> shopRows;

	sf::Sprite leftPane;
	sf::Sprite leftBackdrop;
	sf::Sprite detailsPane;
};

