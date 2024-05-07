#include "level.h"
#include <Box2D/Box2D.h>
#include "tinyxml2.h"

#include <iostream>
#include <random>

using namespace tinyxml2;


int main() {
	srand(time(NULL));

	Level level;
	level.LoadFromFile("Tiles/MapMain.tmx");

	Game *game = new Game(&level); // создание всех объектов
	b2World *world = game->get_world();
	sf::RenderWindow game_window(sf::VideoMode(1500, 1000), "Test");
	sf::FloatRect visibleArea = sf::FloatRect(0.f, 0.f, game_window.getSize().x * 0.4, game_window.getSize().y * 0.9);// для регистрации масштабирования окна

	// окна героев
	sf::View player1View;
	player1View.setViewport(sf::FloatRect(0.f, 0.f, 0.5f, 1.f));
	player1View.reset(visibleArea);
	sf::View player2View;
	player2View.setViewport(sf::FloatRect(0.5f, 0.f, 0.5f, 1.f));
	player2View.reset(visibleArea);


	//Цикл игры
	while (game_window.isOpen()) {
		sf::Event event;
		game_window.pollEvent(event);

			//Действия
			switch (event.type) {

			case sf::Event::Closed:
				game_window.close();
				break;
			case sf::Event::Resized:
				// update the view to the new size of the window
				 visibleArea = sf::FloatRect(0.f, 0.f, event.size.width * 0.4, event.size.height * 0.9);
				 player1View.reset(visibleArea);
				 player1View.zoom(0.7f);
				 player2View.reset(visibleArea);
				 player2View.zoom(0.7f);
				break;
			default: break;
			}
		
		game->behavior(event);
		world->Step(1.0f / 3.f, 1, 1);

		game->game_draw(&game_window, level, player1View, 1);
		game->game_draw(&game_window, level, player2View, 2);
		game_window.display();
		game_window.clear();
	}
	return 0;
}
