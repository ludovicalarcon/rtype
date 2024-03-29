#include <SFML/Graphics.hpp>
#include <iostream>

#include "ResourceManager.h"
#include "World.h"
#include "Systems.h"
#include "Game.h"
#include "hpl.h"

int								main(int argc, char *argv[], char *env[])
{
	Game						game;

	std::srand(static_cast<unsigned int>(std::time(NULL)));

	if (!game.initialize(sf::Vector2u(1280, 720), "R-Type")) {
		system("pause");
		return (EXIT_FAILURE);
	}
	game.run();
	game.exit();
	return (EXIT_SUCCESS);
}