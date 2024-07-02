#include "Title.h"

#include "ResourceManager.h"
#include "ColorManager.h"

const int TITLE_SPLIT_Y = 22;

void Title::init() {
	sf::Texture &titleTexture = rm::loadTexture("Resource/Image/Title.png");
	int spriteWidth = titleTexture.getSize().x;
	int spriteHeight = titleTexture.getSize().y / 2;
	
	backdropSprite.setTexture(titleTexture);
	backdropSprite.setTextureRect(sf::IntRect(0, 0, spriteWidth, spriteHeight));

	topTextSprite.setTexture(titleTexture);
	topTextSprite.setTextureRect(sf::IntRect(0, spriteHeight, spriteWidth, TITLE_SPLIT_Y));

	bottomTextSprite.setTexture(titleTexture);
	bottomTextSprite.setTextureRect(sf::IntRect(0, spriteHeight + TITLE_SPLIT_Y, spriteWidth, spriteHeight - TITLE_SPLIT_Y));

	// Load shader
	waterShader.loadFromFile("WaterFragShader.txt", sf::Shader::Fragment);
	waterShader.setUniform("texture", sf::Shader::CurrentTexture);
}

void Title::update(sf::Time elapsed) {
	// Update colors
	backdropSprite.setColor(cm::getJellyColor());
	topTextSprite.setColor(cm::getFlashColor());
	bottomTextSprite.setColor(cm::getWaterColor());

	// Update positions
	backdropSprite.setPosition(getPosition());
	topTextSprite.setPosition(getPosition());
	bottomTextSprite.setPosition(getPosition() + sf::Vector2f(0, TITLE_SPLIT_Y));

	// Update shader
	waterTime += elapsed.asSeconds();
	waterShader.setUniform("time", waterTime);
}

void Title::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	target.draw(backdropSprite, states);

	target.draw(topTextSprite, states);

	//target.draw(bottomTextSprite, states);
	target.draw(bottomTextSprite, &waterShader);
}
