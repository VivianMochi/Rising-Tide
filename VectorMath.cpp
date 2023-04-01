#include "VectorMath.h"

float vm::getMagnitude(sf::Vector2f input) {
	return std::sqrt(input.x * input.x + input.y * input.y);
}
