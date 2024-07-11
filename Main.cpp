#include "Game.h"

#include <Windows.h>

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

// To remove console window...
// Linker -> System -> from Console to Windows
/*
int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
	main();
}
*/