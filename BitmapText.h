#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class BitmapText : public sf::Drawable, public sf::Transformable {
public:
	BitmapText();
	BitmapText(const sf::Texture &texture, std::string text = "");
	~BitmapText();

	void setTexture(const sf::Texture &texture);
	void setText(std::string text);
	void setColor(sf::Color color);
	std::string getText() const;
	int getWidth();

	std::string text;

	bool monowidth = false;

private:
	void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
	int getGlyphWidth(char character) const;

	const sf::Texture *texture;

	sf::Color color = sf::Color::Black;
};