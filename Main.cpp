#include "Game.h"

#include "ColorManager.h"
#include <cstdlib>
#include <ctime>

#include <iostream>

int main() {
	std::srand(std::time(nullptr));

	cm::init();

	Game game;
	return 0;
}
