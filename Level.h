#ifndef LEVEL_H
#define LEVEL_H

#pragma comment(lib,"Box2d.lib")
#pragma comment(lib,"sfml-graphics.lib")
#pragma comment(lib,"sfml-window.lib")
#pragma comment(lib,"sfml-system.lib")

#include <string>
#include <vector>
#include <map>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>

#include <iostream>

class Level;
class Layer;
class Game;

class Object;
class objects_from_map;
class Unit;
class environment_element;

class hero;
class near_Enemy;
class lond_range_Enemy;

class block;
class coin;
class box;
class trap;
class bomb;


class Level {
private:
	int width, height, tileWidth, tileHeight;
	int firstTileID;
	std::vector <sf::Texture> tilesetImage;
	std::vector<objects_from_map*> objects;
	std::vector<Layer> layers;

public:
	bool LoadFromFile(std::string filename);
	objects_from_map* GetObject(std::string name);
	std::vector<objects_from_map*> GetObjects(std::string type_name);
	void Draw(sf::RenderWindow& window);
	sf::Vector2i GetTileSize();
	
	void clear();
};

class Object {
private:
	std::string type;   // тип
	std::string name;   // имя
	int health;         // здоровье
	int damage;         // урон
	bool is_alive;      // статус жизни
	bool attack_state = false;
	bool direction = true; // направление взгляда
	int money = 0;      // деньги

	// спрайты и номера тайлов
	sf::Vector2i nof_tile;
	sf::Vector2i nof_health_bar = sf::Vector2i(1,6);
	sf::Rect<int> rect;
	sf::Sprite sprite;
	sf::Sprite health_bar;


	float CurrentFrame = 0; // для счетчика анимаций движения
	float CurrentAttac = 2.99; // для счетчика анимаций атак
	sf::Clock clock; // часы для учёта времени

	friend class Level;

public:
	Object();
	Object(int _damage, std::string name);
	

	virtual bool behavior(sf::Event event, Game* game, int nof_object) = 0;
	virtual void accept_damage(int _damage, class Game* game, int nof_object) = 0;
	void add_money(int _money);
	
	int get_money();
	sf::Rect<int> get_rect();
	sf::Sprite* get_sprite_pntr();
	sf::Sprite* get_health_bar_pntr();

	bool get_state();
	bool get_attack_state();
	bool get_direction();
	sf::Vector2i get_nof_tile();
	sf::Vector2i get_nof_health_bar();
	int get_damage();
	int get_health();
	std::string get_name();

	float* get_CurrentFrame();
	float* get_CurrentAttac();
	sf::Clock* get_clock();

	void set_state(bool state);
	void set_attack_state(bool _attack_state);

	void set_rect(sf::Rect<int> _rect);
	void set_health(int damage);
	void set_sprite(sf::Sprite _sprite);
	void set_nof_tile(sf::Vector2i _nof_tile);
	void set_health_bar(sf::Sprite _health_bar);
	void set_direction(bool _direction);
};

// Выгружаемые из файла объекты
class objects_from_map: public Object {
public:
	bool behavior(sf::Event event, Game* game, int nof_object);
	void accept_damage(int _damage, class Game* game, int nof_object);
};

// Юниты
class Unit: public Object {
public:
	Unit( int _damage, std::string _name);

	virtual bool behavior(sf::Event event, Game* game, int nof_object) = 0;
};

class hero : public Unit {
public:
	hero();

	bool behavior(sf::Event event, Game* game, int nof_object);
	void accept_damage(int _damage, class Game* game, int nof_object);

	// передвижение героя вправо
	void run_hero_right(b2Body* playerBody);

	// передвижение героя влево
	void run_hero_left(b2Body* playerBody);

	// передвижение героя прыжок
	void hero_jump(b2Body* playerBody);

	// действия героя стрельба
	void hero_shoot(Game* game, int nof_player);

};

class enemy: public Unit {
public:
	enemy(int _damage, std::string _name);

	virtual bool behavior(sf::Event event, Game* game, int nof_object) = 0;
	void accept_damage(int _damage, class Game* game, int nof_object);
};

class near_Enemy: public enemy {
public:
	near_Enemy();
	bool behavior(sf::Event event, Game* game, int nof_object);
};

class long_range_Enemy: public enemy {
public:
	long_range_Enemy();
	bool behavior(sf::Event event, Game* game, int nof_object);
};

// Элементы окружения
class environment_element : public Object {
public:
	environment_element(int _damage, std::string _name);
	virtual bool behavior(sf::Event event, Game* game, int nof_object) = 0;
	void accept_damage(int _damage, class Game* game, int nof_object);
};

class block : public environment_element { // под вопросом. Нет спрайта, не наносится урон.
public:
	block();

	bool behavior(sf::Event event, Game* game, int nof_object);
};

class coin: public environment_element {
public:
	coin();

	bool behavior(sf::Event event, Game* game, int nof_object);
};

class box : public environment_element {
public:
	box();

	bool behavior(sf::Event event, Game* game, int nof_object);
};

class trap : public environment_element {
public:
	trap();

	bool behavior(sf::Event event, Game* game, int nof_object);
};

class bomb: public environment_element{
public:
	bomb();

	bool behavior(sf::Event event, Game* game, int nof_object);
};

//слой для бекграунда
class Layer {
	int opasity;
	std::vector<sf::Sprite> tiles;
	friend class Level;
};

// Игра
class Game {
private:
	std::vector <Object*> player;
	std::vector <b2Body*> playerBody;

	std::vector<Object*> enemy;
	std::vector<b2Body*> enemyBody;

	std::vector<Object*> environment_element;
	std::vector<b2Body*> environment_elementBody;

	std::vector<class Object*> block;
	b2World *world = new b2World(b2Vec2(0.0f, 0.5f));

public:
	// создание всех объектов
	Game(Level *level);
	 
	// отрисовка
	void game_draw(class sf::RenderWindow* window, class Level level, class  sf::View view, int nof_player);

	// поведение всех объектов
	void behavior(sf::Event event);

	void attack(Object* attacker, Object* victim, int nof_victim);
	//убить объект
	void die(Object* dier, int nof_object);
	void add_object_to_environment_element(Object* object, b2Body* body);
	void  create_object(Object* object, Level* level);
	b2Body* create_Body(Object* object, Level* level);

	Object* get_player(int nof_player);
	b2Body* get_playerBody(int nof_player);
	b2Body* get_enemyBody(int nof_object);
	Object* get_enemy(int nof_Body);
	b2Body* get_environment_elementBody(int nof_object);
	b2World* get_world();

};

#endif