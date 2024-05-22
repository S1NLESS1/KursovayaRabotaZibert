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

	Game* game = new Game(&level); // создание всех объектов
	b2World* world = game->get_world();
	sf::RenderWindow game_window(sf::VideoMode(1500, 1000), "Test");
	sf::FloatRect visibleArea = sf::FloatRect(0.f, 0.f, game_window.getSize().x * 0.4, game_window.getSize().y * 0.9);// для регистрации масштабирования окна
	
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
			break;
		default: break;
		}

		game->behavior(event);
		if (abs(game->get_playerBody(0)->GetPosition().x - game->get_playerBody(1)->GetPosition().x) < 250 && abs(game->get_playerBody(0)->GetPosition().y - game->get_playerBody(1)->GetPosition().y) < 200){
			game->game_draw(&game_window, level, visibleArea, 0,"GENERAL");
			world->Step(1.0f / 9.f, 1, 1);
		}
		else {
			game->game_draw(&game_window, level, visibleArea, 0, "SINGLE");
			world->Step(1.0f / 7.f, 1, 1);
		}

		game_window.display();
		game_window.clear();
	}
	return 0;
}
