#include "level.h"
#include <iostream>
#include "tinyxml2.h"

using namespace tinyxml2;


// *************OBJECT*******************
Object::Object(){

}

Object::Object(int _damage, std::string _name) {
	health = 100;
	damage = _damage;
	is_alive = true;
	name = _name;
}

void Object::add_money(int _money) {
	money += _money;
}

int Object::get_money() {
	return money;
}

int Object::get_damage() {
	return damage;
}

int Object::get_health() {
	return health;
}

std::string Object::get_name(){
	return name;
}

bool Object::get_state(){
	return is_alive;
}

bool Object::get_attack_state(){
	return attack_state;
}

bool Object::get_direction(){
	return direction;
}

sf::Vector2i Object::get_nof_tile() {
	return nof_tile;
}

sf::Vector2i Object::get_nof_health_bar(){
	return nof_health_bar;
}

sf::Clock* Object::get_clock(){
	return &clock;
}

sf::Rect<int> Object::get_rect() {
	return rect;
}

sf::Sprite* Object::get_sprite_pntr() {
	return &sprite;
}

sf::Sprite* Object::get_health_bar_pntr()
{
	return &health_bar;
}

float* Object::get_CurrentFrame() {
	return &CurrentFrame;
}

float* Object::get_CurrentAttac()
{
	return &CurrentAttac;
}

void Object::set_state(bool state){
	is_alive = state;
}

void Object::set_attack_state(bool _attack_state){
	attack_state = _attack_state;
}

void Object::set_rect(sf::Rect<int> _rect) {
	rect = _rect;
}

void Object::set_health(int damage){
	health -= damage;
	if (health < 0) health = 0;
}

void Object::set_sprite(sf::Sprite _sprite) {
	sprite = _sprite;
}

void Object::set_nof_tile(sf::Vector2i _nof_tile){
	nof_tile = _nof_tile;
}

void Object::set_health_bar(sf::Sprite _health_bar){
	health_bar = _health_bar;
}

void Object::set_direction(bool _direction){
	direction = _direction;
}


// *************objects_from_map*****************
bool objects_from_map::behavior(sf::Event event, Game* game, int nof_object) {
	return false;
}

void objects_from_map::accept_damage(int _damage, Game* game, int nof_object){
	return;
}


//******************UNIT****************
Unit::Unit(int _damage,  std::string _name): Object(_damage, _name) {

}


//******************HERO*****************
hero::hero(): Unit(30, "hero") {

}

bool hero::behavior(sf::Event event, Game* game, int nof_object) {
	b2Body* playerBody = game->get_playerBody(nof_object);
	sf::Clock *clock = this->get_clock();
	float time = clock->getElapsedTime().asMicroseconds();
	time = time / 800; 
	clock->restart();
	float* CurrentFrame = this->get_CurrentFrame();
	float* CurrentAttac =this->get_CurrentAttac();
	//выкуп
	bool flag = false;
	if (!this->get_state()) { // мертв
		switch (nof_object) {
		case 0:
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) 
				flag = true;
			else return false;
			break;
		case 1:
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) 
				flag = true;
			else return false;
			break;
		}
		if (flag)
			if (this->get_money() >= 5) {
				this->set_state(true);
				this->set_health(-100); //нанести отрицательный урон(вылечить)
				this->get_health_bar_pntr()->setTextureRect(sf::Rect <int>((this->get_nof_health_bar().x) * 32, this->get_nof_health_bar().y * 32, 45, 20));
				this->add_money(-5);
			}
			else return false;
		else return false;
	}

	 // текстура статики
	int direction = (this->get_direction()) ? 0 : -1;
	
	if (this->get_attack_state()) {
		*CurrentFrame += 0.006 * time;
		if (*CurrentFrame > 3) {
			*CurrentFrame = 0;
			this->set_attack_state(false);
			return false;
		}
		if((int)*CurrentFrame == 2)
			this->get_sprite_pntr()->setTextureRect(sf::Rect <int>((this->get_nof_tile().x + (int)*CurrentFrame * 2 - direction*7) * 32, (this->get_nof_tile().y + 3) * 32, 32 + direction*64, 56)); // атака
		else this->get_sprite_pntr()->setTextureRect(sf::Rect <int>((this->get_nof_tile().x + (int)*CurrentFrame * 2 - direction*7) * 32, (this->get_nof_tile().y + 3) * 32, 51 + direction*102, 56)); // атака
	}
	else if (game->get_playerBody(nof_object)->GetLinearVelocity().y == 0) {
		*CurrentFrame += 0.0045 * time;
		if (*CurrentFrame > 10) *CurrentFrame = 0;
		this->get_sprite_pntr()->setTextureRect(sf::Rect <int>((this->get_nof_tile().x + (int)*CurrentFrame) * 32, (this->get_nof_tile().y + direction) * 32, 32, 32)); // покой(бег)
	}
	else
		this->get_sprite_pntr()->setTextureRect(sf::Rect <int>((this->get_nof_tile().x - direction) * 32, (this->get_nof_tile().y + 2) * 32, 32, 32)); // прыжок(падение)


	// передвижение и атака
	switch (nof_object) {
	case 0:
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
			this->set_direction(true);  // смотрит вправо
			run_hero_right(playerBody);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) { 
			this->set_direction(false); // смотрит влево
			run_hero_left(playerBody); 
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
			hero_jump(playerBody);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
			*CurrentAttac += 0.003 * time; // кд на атаку
			if (*CurrentAttac > 3) {
				*CurrentAttac = 0;
				*CurrentFrame = 0;
				this->set_attack_state(true);
				hero_shoot(game, nof_object);
			}
		}

	break;
	case 1: 
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			this->set_direction(true);  // смотрит вправо
			run_hero_right(playerBody);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			this->set_direction(false); // смотрит влево
			run_hero_left(playerBody);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			hero_jump(playerBody);
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
			*CurrentAttac += 0.003 * time; // кд на атаку
			if (*CurrentAttac > 3) {
				*CurrentAttac = 0;
				*CurrentFrame = 0;
				this->set_attack_state(true);
				hero_shoot(game, nof_object);
			}
		}
	break;
	}
	return false;
}

void hero::run_hero_right(b2Body* playerBody) {
	playerBody->SetLinearVelocity(b2Vec2(4.0f, playerBody->GetLinearVelocity().y));
}

void hero::run_hero_left(b2Body* playerBody) {
	playerBody->SetLinearVelocity(b2Vec2(-4.0f, playerBody->GetLinearVelocity().y));
}

void hero::hero_jump(b2Body* playerBody) {
	if (playerBody->GetContactList()) // есть контакты с другими объектами
		playerBody->ApplyLinearImpulse(b2Vec2(playerBody->GetLinearVelocity().x, -playerBody->GetMass() * 10), playerBody->GetWorldCenter(), true);
}

void hero::hero_shoot(Game* game, int nof_player) {
	game->attack(this, 0, nof_player);
}

void hero::accept_damage(int _damage, class Game* game, int nof_object) {
	if (!this->get_state()) return;
	set_health(_damage);

	if (get_health() <= 0) {
		this->get_health_bar_pntr()->setTextureRect(sf::Rect <int>((this->get_nof_health_bar().x + 6) * 32, this->get_nof_health_bar().y * 32, 45, 20));
		game->die(this, nof_object);
	}
	else if (get_health() <= 33) {
		this->get_health_bar_pntr()->setTextureRect(sf::Rect <int>((this->get_nof_health_bar().x + 4) * 32, this->get_nof_health_bar().y * 32, 45, 20));
	}
	else if (get_health() <= 66) {
		this->get_health_bar_pntr()->setTextureRect(sf::Rect <int>((this->get_nof_health_bar().x + 2) * 32, this->get_nof_health_bar().y * 32, 45, 20));
	}
	else if (get_health() <= 100) {
		this->get_health_bar_pntr()->setTextureRect(sf::Rect <int>((this->get_nof_health_bar().x) * 32, this->get_nof_health_bar().y * 32, 45, 20));
	}
	return;
}


//***************ENEMY**************
enemy::enemy(int _damage, std::string _name) : Unit(_damage, _name){

}

void enemy::accept_damage(int _damage, class Game* game, int nof_object) {
	if (!this->get_state()) return;
	set_health(_damage);

	int cof_heart_pixels;
	int cof_heart = (get_health() - 1) / 10 + 1;
	if (get_health() == 0) {
		cof_heart_pixels = 32;
		game->die(this, nof_object);
	}
	else
		cof_heart_pixels = (10 - cof_heart) * 3;
	this->get_health_bar_pntr()->setTextureRect(sf::Rect <int>(this->get_nof_health_bar().x * 32, this->get_nof_health_bar().y * 32, 32 - cof_heart_pixels, 6));
	return;
}


// near_Enemy
near_Enemy::near_Enemy() :enemy(20, "near_Enemy") {

}

bool near_Enemy::behavior(sf::Event event, Game* game, int nof_object) {
	if (!this->get_state()) return false; // мертв

	b2Body* enemyBody = game->get_enemyBody(nof_object);

	sf::Clock* clock = this->get_clock();
	float time = clock->getElapsedTime().asMicroseconds();
	time = time / 800;
	clock->restart();
	float* CurrentFrame = this->get_CurrentFrame();
	float* CurrentAttac = this->get_CurrentAttac();


	int direction = (this->get_direction()) ? 0 : -1;

	//движение
	if (enemyBody->GetLinearVelocity() == b2Vec2_zero) {
		int tmp = (rand() % 2 == 1) ? 1 : -1;
		enemyBody->SetLinearVelocity(b2Vec2(3.0f * tmp, 0.0f));
	}
	if (this->get_attack_state()) {
		*CurrentFrame += 0.0055 * time;
		if (*CurrentFrame > 4) {
			*CurrentFrame = 0;
			this->set_attack_state(false);
			return false;
		}
		this->get_sprite_pntr()->setTextureRect(sf::Rect <int>((this->get_nof_tile().x + (int)*CurrentFrame+1) * 32, (this->get_nof_tile().y + 2) * 32, 32, 32)); // атака
	}
	else {
		*CurrentFrame += 0.004 * time;
		if (*CurrentFrame > 11) *CurrentFrame = 0;
		this->get_sprite_pntr()->setTextureRect(sf::Rect <int>((this->get_nof_tile().x + (int)*CurrentFrame) * 32, (this->get_nof_tile().y) * 32, 32, 32)); // покой(бег)
	}

	// атака героев
	for (class b2ContactEdge* ce = game->get_enemyBody(nof_object)->GetContactList(); ce; ce = ce->next) {
		class b2Contact* c = ce->contact;
		for (int j = 0; j < 2; j++) {        // толкнуть и атаковать героя
			if (c->GetFixtureA() == game->get_playerBody(j)->GetFixtureList()) {
				if (game->get_player(j)->get_state()) { // игрок жив
					*CurrentAttac += 0.0015 * time; // ограничение скорости атаки
					if (*CurrentAttac > 2) {
						*CurrentAttac = 0;
						this->set_attack_state(true);
						*CurrentFrame = 0;
						game->attack(this, game->get_player(j), j);
					}
				}
			}
		}
	}
	return false;
}

// long_range_Enemy
long_range_Enemy::long_range_Enemy() : enemy(30, "long_range_Enemy") {
	
}

bool long_range_Enemy::behavior(sf::Event event, Game* game, int nof_object) {
	if (!this->get_state()) return false; // мертв

	b2Body* enemyBody = game->get_enemyBody(nof_object);

	sf::Clock* clock = this->get_clock();
	float time = clock->getElapsedTime().asMicroseconds();
	time = time / 800;
	clock->restart();
	float* CurrentFrame = this->get_CurrentFrame();
	float* CurrentAttac = this->get_CurrentAttac();


	if (this->get_attack_state()) {
		*CurrentFrame += 0.0055 * time;
		if (*CurrentFrame > 4) {
			*CurrentFrame = 0;
			this->set_attack_state(false);
			return false;
		}
		this->get_sprite_pntr()->setTextureRect(sf::Rect <int>((this->get_nof_tile().x + (int)*CurrentFrame*2) * 32, (this->get_nof_tile().y + 1) * 32, 32, 32)); // атака
	}
	else {
		*CurrentFrame += 0.004 * time;
		if (*CurrentFrame > 10) *CurrentFrame = 0;
		this->get_sprite_pntr()->setTextureRect(sf::Rect <int>((this->get_nof_tile().x + (int)*CurrentFrame) * 32, (this->get_nof_tile().y) * 32, 32, 32)); // покой(бег)
	}

	*CurrentAttac += 0.0015 * time; // ограничение скорости атаки
	if (*CurrentAttac > 5) {
		*CurrentAttac = 0;
		this->set_attack_state(true);
		Object* bomb = new class bomb();
		Level level;
		b2Body* bombBody = game->create_Body(this,&level);
		int tmp = (rand() % 2 == 1) ? 1 : -1;
		bombBody->ApplyLinearImpulse(b2Vec2(bombBody->GetMass() * 15 * tmp, -bombBody->GetMass() * 15), bombBody->GetWorldCenter(), true);

		bomb->set_sprite(*this->get_sprite_pntr());
		bomb->set_nof_tile(sf::Vector2i(1, 3));
		bomb->get_sprite_pntr()->setTextureRect(sf::Rect <int>(bomb->get_nof_tile().x * 32, bomb->get_nof_tile().y * 32, 32, 32));
		bomb->get_sprite_pntr()->setPosition(this->get_sprite_pntr()->getPosition().x, this->get_sprite_pntr()->getPosition().y - 32);
		//bomb->set_rect(this->get_rect()); непонятно, зачем это
		game->add_object_to_environment_element(bomb, bombBody);
	}
	return false;
}


// environment_element
environment_element::environment_element(int _damage, std::string _name): Object(_damage, _name){

}

void environment_element::accept_damage(int _damage, Game* game, int nof_object){
	return;
}


// **************BLOCK****************
block::block() : environment_element(0, "block") {
	
}

bool block::behavior(sf::Event event, Game *game, int nof_object) {
	return false;
}


// **************COIN****************
coin::coin() : environment_element(0, "coin") {

}

bool coin::behavior(sf::Event event, Game* game, int nof_object) {

	sf::Clock* clock = this->get_clock();
	float time = clock->getElapsedTime().asMicroseconds();
	time = time / 800;
	clock->restart();
	float* CurrentFrame = this->get_CurrentFrame();
	float* CurrentAttac = this->get_CurrentAttac();
	*CurrentFrame += 0.002 * time;
	if (*CurrentFrame > 6) { // кд на взятие монет
		b2Body* body = game->get_environment_elementBody(nof_object);
		for (int i = 0; i < 2; i++) {
			for (b2ContactEdge* ce = body->GetContactList(); ce; ce = ce->next) {
				b2Contact* c = ce->contact;
				if (c->GetFixtureA() == game->get_playerBody(i)->GetFixtureList()) {
					return true;
				}
			}
		}
	}
	return false;
}


// **************BOX****************
box::box() : environment_element(30, "box") {

}

bool box::behavior(sf::Event event, Game* game, int nof_object) {
	return false;
}


//**************TRAP****************
trap::trap() : environment_element(10, "trap") {

}

bool trap::behavior(sf::Event event, Game *game, int nof_object) {
	sf::Clock* clock = this->get_clock();
	float* CurrentAttac = this->get_CurrentAttac();
	float time = clock->getElapsedTime().asMicroseconds();
	time = time / 800;
	clock->restart();

	// атака героев
	for (class b2ContactEdge* ce = game->get_environment_elementBody(nof_object)->GetContactList(); ce; ce = ce->next) {
		class b2Contact* c = ce->contact;
		for (int j = 0; j < 2; j++) {        // толкнуть и атаковать героя
			if (c->GetFixtureA() == game->get_playerBody(j)->GetFixtureList()) {
				if (game->get_player(j)->get_state()) { // игрок жив
					*CurrentAttac += 0.015 * time; // ограничение скорости атаки
					if (*CurrentAttac > 2) {
						*CurrentAttac = 0;
						game->attack(this, game->get_player(j), j);
					}
				}
			}
		}
	}
	return false;
}


//**************BOMB****************
bomb::bomb() :environment_element(50, "bomb") {

}

bool bomb::behavior(sf::Event event, Game* game, int nof_object) {

	b2Body* bombBody = game->get_environment_elementBody(nof_object);
	b2Vec2 bomb_pos = bombBody->GetPosition();

	sf::Clock* clock = this->get_clock();
	float time = clock->getElapsedTime().asMicroseconds();
	time = time / 800;
	clock->restart();
	float* CurrentFrame = this->get_CurrentFrame();
	float* CurrentAttac = this->get_CurrentAttac();

	if (this->get_attack_state()) {
		*CurrentFrame += 0.006 * time;
		if (*CurrentFrame > 6) {
			return true; // взрывается 
		}
		switch ((int)*CurrentFrame) {
		case 0: this->get_sprite_pntr()->setTextureRect(sf::Rect <int>((this->get_nof_tile().x + (int)*CurrentFrame) * 32, (this->get_nof_tile().y + 2) * 32, 32, 32)); // атака
			break;
		case 1: this->get_sprite_pntr()->setTextureRect(sf::Rect <int>((this->get_nof_tile().x + (int)*CurrentFrame) * 32 + 14, (this->get_nof_tile().y + 2) * 32, 40, 40));
			break;
		case 2: this->get_sprite_pntr()->setTextureRect(sf::Rect <int>((this->get_nof_tile().x + (int)*CurrentFrame*2 -1) * 32 - 1, (this->get_nof_tile().y + 2) * 32 - 3, 48, 47));
			break;
		case 3: this->get_sprite_pntr()->setTextureRect(sf::Rect <int>((this->get_nof_tile().x + (int)*CurrentFrame*2 -1) * 32 - 15, (this->get_nof_tile().y + 2) * 32 - 6, 52, 52));
			break;
		case 4: this->get_sprite_pntr()->setTextureRect(sf::Rect <int>((this->get_nof_tile().x + (int)*CurrentFrame*2 -1) * 32 + 10, (this->get_nof_tile().y + 2) * 32 - 8, 45, 35)); // атака
			break;
		case 5: this->get_sprite_pntr()->setTextureRect(sf::Rect <int>((this->get_nof_tile().x + (int)*CurrentFrame*2 -2) * 32 - 3, (this->get_nof_tile().y + 2) * 32 - 9, 45, 33)); // атака
			break;
		}
		return false;
		
	}

	*CurrentAttac += 0.0015 * time; // ограничение скорости атаки
	if (*CurrentAttac > 6) {
		this->set_attack_state(true);
		for (int j = 0; j < 2; j++) {
			b2Body* playerBody = game->get_playerBody(j);
			b2Vec2 player_pos = playerBody->GetPosition();
			if (abs(bomb_pos.x - player_pos.x) <= 70 && abs(bomb_pos.y - player_pos.y) <= 70)
				game->attack(this, game->get_player(j), j); // наносится урон по игроку
		}
	}
	return false;
}


// **************GAME****************
Game::Game(Level *level) {
	sf::Vector2i tileSize = level->GetTileSize();
	
	// Обработка героев
	std::vector<objects_from_map*> temp = level->GetObjects("hero");
	for (int i = 0; i < temp.size(); i++)
		create_object(temp[i], level);

	//Обработка блоков
	temp.clear();
	temp = level->GetObjects("block");
	for (int i = 0; i < temp.size(); i++)
		create_object(temp[i], level);

	//Обработка элементов окружения
	temp.clear();
	temp = level->GetObjects("environment_element");
	for (int i = 0; i < temp.size(); i++)
		create_object(temp[i], level);

	//Обработка врагов
	temp.clear();
	temp = level->GetObjects("enemy");
	for (int i = 0; i < temp.size(); i++)
		create_object(temp[i], level);

	level->clear(); // удаляем вектор объектов из level
}

void Game::create_object(Object* object, Level* level) {
	sf::Vector2i tileSize = level->GetTileSize();
	std::vector <Object*> *vector_pntr = NULL;
	std::vector <b2Body*> *vectorBody_pntr = NULL;
	Object* creating_object = NULL;
	std::string object_name = object->get_name();
	if (object_name == "hero") {
		creating_object = new class hero();
		vector_pntr = &player;
		vectorBody_pntr = &playerBody;
	}
	else if (object_name == "block") {
		creating_object = new class block();
		vector_pntr = &block;
		vectorBody_pntr = NULL;
	}
	else if (object_name == "coin") {
		creating_object = new class coin();
		vector_pntr = &environment_element;
		vectorBody_pntr = &environment_elementBody;
	}
	else if (object_name == "box") {
		creating_object = new class box();
		vector_pntr = &environment_element;
		vectorBody_pntr = &environment_elementBody;
	}
	else if (object_name == "trap") {
		creating_object = new class trap();
		vector_pntr = &environment_element;
		vectorBody_pntr = &environment_elementBody;
	}
	else if(object_name == "near_Enemy") {
		creating_object = new class near_Enemy();
		vector_pntr = &enemy;
		vectorBody_pntr = &enemyBody;
	}
	else if (object_name == "long_range_Enemy") {
		creating_object = new class long_range_Enemy();
		vector_pntr = &enemy;
		vectorBody_pntr = &enemyBody;
	}
		
	b2Body * body = this->create_Body(object, level);
	if(object_name != "block") vectorBody_pntr->push_back(body);

	creating_object->set_sprite(*object->get_sprite_pntr());
	creating_object->set_health_bar(*object->get_health_bar_pntr());
	sf::Vector2f pos_environment_element = creating_object->get_sprite_pntr()->getPosition();
	creating_object->get_sprite_pntr()->setPosition(pos_environment_element.x, pos_environment_element.y - 32);
	creating_object->set_rect(object->get_rect());
	creating_object->set_nof_tile(object->get_nof_tile());
	if(object_name == "hero")
		creating_object->get_health_bar_pntr()->setTextureRect(sf::Rect <int>(creating_object->get_nof_health_bar().x * 32, creating_object->get_nof_health_bar().y * 32, 45, 20));
	else creating_object->get_health_bar_pntr()->setTextureRect(sf::Rect <int>(creating_object->get_nof_health_bar().x * 32, creating_object->get_nof_health_bar().y * 32, 32, 5));

	vector_pntr->push_back(creating_object);
}

b2Body* Game::create_Body(Object* object, Level* level) {
	sf::Vector2i tileSize = level->GetTileSize();
	b2BodyDef bodyDef;
	std::string object_name = object->get_name();
	if (object_name == "block" || object_name == "trap") {
		bodyDef.type = b2_staticBody;
		bodyDef.position.Set(object->get_sprite_pntr()->getPosition().x, object->get_sprite_pntr()->getPosition().y-32);
	}
	else {
		bodyDef.type = b2_dynamicBody;
		bodyDef.position.Set(object->get_sprite_pntr()->getPosition().x, object->get_sprite_pntr()->getPosition().y);
	}
	bodyDef.fixedRotation = true;

	b2Body* body = world->CreateBody(&bodyDef);
	b2PolygonShape shape;
	shape.SetAsBox(object->get_rect().width / 2, object->get_rect().height / 2);
	if (object_name != "box" && object_name != "block") {
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &shape;
		fixtureDef.density = 1.0f;
		fixtureDef.friction = 0.35f;
		if(object_name == "hero") fixtureDef.filter.groupIndex = -1;
		else fixtureDef.filter.groupIndex = -2;
		body->CreateFixture(&fixtureDef);
	}
	else body->CreateFixture(&shape, 1.0f);
	return body;
}

void Game::game_draw(class sf::RenderWindow* window, class Level level, class  sf::View view, int nof_player) {
	// изменение спрайтов всем динамическим объектам
	for (int i = 0; i < 2; i++) {
		b2Vec2 pos = playerBody[i]->GetPosition();
		player[i]->get_sprite_pntr()->setPosition(pos.x, pos.y);
		player[i]->get_health_bar_pntr()->setPosition(pos.x-10, pos.y-15);
	}

	b2Vec2 pos = playerBody[nof_player - 1]->GetPosition();
	view.setCenter(pos.x + 0, pos.y); // обновить центрирование экрана
	
	window->setView(view); // отобразить новый вид экрана

	for (int i = 0; i < environment_element.size(); i++) {
		environment_element[i]->get_sprite_pntr()->setPosition(environment_elementBody[i]->GetPosition().x, environment_elementBody[i]->GetPosition().y);
	}
	for (int i = 0; i < enemy.size(); i++) {
		if (enemy[i]->get_state()) {
			enemy[i]->get_sprite_pntr()->setPosition(enemyBody[i]->GetPosition().x, enemyBody[i]->GetPosition().y);
			enemy[i]->get_health_bar_pntr()->setPosition(enemyBody[i]->GetPosition().x, enemyBody[i]->GetPosition().y);
		}
	}
	// отрисовка
	level.Draw(*window); // отрисовать бекграунд
	
	// блоки
	for (int i = 0; i < block.size(); i++) {
		window->draw(*block[i]->get_sprite_pntr());
	}
	// ЭО
	for (int i = 0; i < environment_element.size(); i++) {
		window->draw(*environment_element[i]->get_sprite_pntr());
		window->draw(*environment_element[i]->get_health_bar_pntr());
	}
	// враги
	for (int i = 0; i < enemy.size(); i++) {
		window->draw(*enemy[i]->get_sprite_pntr());
		window->draw(*enemy[i]->get_health_bar_pntr());
	}
	// герои
	for (int i = 0; i < 2; i++) {
		window->draw(*player[0]->get_sprite_pntr());
		window->draw(*player[0]->get_health_bar_pntr());

		window->draw(*player[1]->get_sprite_pntr());
		window->draw(*player[1]->get_health_bar_pntr());
	}

}

void Game::behavior(sf::Event event) {
	// поведение героев
	for (int i = 0; i < player.size(); i++) 
			player[i]->behavior(event, this, i);

	// поведение врагов
	for (int i = 0; i < enemy.size(); i++)
		enemy[i]->behavior(event,this,i);

	// поведение монет и бомб
	for (int i = 0; i < environment_element.size(); i++) {
		if (environment_element[i]->behavior(event, this, i)) { // bomb и coin возвращают true для самоуничтожения 
			if(environment_element[i]->get_name() == "coin")
				for (int i = 0; i < player.size(); i++) player[i]->add_money(1);
			environment_element.erase(environment_element.begin() + i);
			environment_elementBody[i]->DestroyFixture(environment_elementBody[i]->GetFixtureList());
			environment_elementBody.erase(environment_elementBody.begin() + i);
		}
	}

}

void Game::attack(Object* attacker, Object* victim, int nof_victim) {
	if (attacker->get_name() == "hero") {
		for (int i = 0; i < enemy.size(); i++) {
			victim = enemy[i];
			if (abs(victim->get_sprite_pntr()->getPosition().x - attacker->get_sprite_pntr()->getPosition().x) <= 50 && abs(victim->get_sprite_pntr()->getPosition().y - attacker->get_sprite_pntr()->getPosition().y) <= 20) {
				nof_victim = i;
				if (victim->get_state()) {
					int direction = (attacker->get_sprite_pntr()->getPosition().x - victim->get_sprite_pntr()->getPosition().x) < 0 ? 1 : -1;
					enemyBody[nof_victim]->ApplyLinearImpulse(b2Vec2(enemyBody[nof_victim]->GetMass() * 5 * direction, enemyBody[nof_victim]->GetMass() * -5), enemyBody[nof_victim]->GetWorldCenter(), true);
					enemy[nof_victim]->get_sprite_pntr()->setTextureRect(sf::Rect <int>(enemy[nof_victim]->get_nof_tile().x * 32, (enemy[nof_victim]->get_nof_tile().y + 1) * 32, 32, 32)); // побелить для получения урона
					victim->accept_damage(attacker->get_damage(), this, nof_victim);
					return;
				}
			}
		}
		return;
	}
	int direction = (attacker->get_sprite_pntr()->getPosition().x - victim->get_sprite_pntr()->getPosition().x) < 0 ? 1 : -1;
	if (attacker->get_name() == "near_Enemy"|| attacker->get_name() == "long_range_Enemy")
		playerBody[nof_victim]->ApplyLinearImpulse(b2Vec2(playerBody[nof_victim]->GetMass() * 5 * direction, playerBody[nof_victim]->GetMass() * -5), playerBody[nof_victim]->GetWorldCenter(), true);
	else if (attacker->get_name() == "bomb") 
		playerBody[nof_victim]->ApplyLinearImpulse(b2Vec2(playerBody[nof_victim]->GetMass() * 10 * direction, playerBody[nof_victim]->GetMass() * -10), playerBody[nof_victim]->GetWorldCenter(), true);
	else if(attacker->get_name() == "trap")
		playerBody[nof_victim]->ApplyLinearImpulse(b2Vec2(0, playerBody[nof_victim]->GetMass() * -2), playerBody[nof_victim]->GetWorldCenter(), true);
	
	player[nof_victim]->get_sprite_pntr()->setTextureRect(sf::Rect <int>(player[nof_victim]->get_nof_tile().x * 32, (player[nof_victim]->get_nof_tile().y + 1) * 32, 37, 32)); // побелить для получения урона
	player[nof_victim]->accept_damage(attacker->get_damage(), this, nof_victim);
}

void Game::die(Object * dier, int nof_object) {
	if (dier->get_name() == "hero") {
		dier->set_state(false);
		dier->get_sprite_pntr()->setTextureRect(sf::Rect <int>((dier->get_nof_tile().x + 4) * 32, (dier->get_nof_tile().y + 1) * 32 , 38, 32)); // установить тайл смерти
	}
	else if (dier->get_name() == "near_Enemy" || dier->get_name() == "long_range_Enemy") {
		Level level;
		for (int i = 0; i < 2; i++) {
			b2Body* coinBody = create_Body(dier, &level);
			int tmp = (rand() % 2 == 1) ? 1 : -1;
			coinBody->ApplyLinearImpulse(b2Vec2(coinBody->GetMass() * enemyBody[nof_object]->GetLinearVelocity().x*2, -coinBody->GetMass() * 15), coinBody->GetWorldCenter(), true);
			environment_elementBody.push_back(coinBody);

			Object* coin = new class coin();
			coin->set_sprite(*this->block[0]->get_sprite_pntr());
			coin->set_nof_tile(sf::Vector2i(7, 18));
			coin->get_sprite_pntr()->setTextureRect(sf::Rect <int>(coin->get_nof_tile().x * 32, coin->get_nof_tile().y * 32, 32, 32)); // установить тайл смерти
			sf::Vector2f pos_environment_element = dier->get_sprite_pntr()->getPosition();
			coin->get_sprite_pntr()->setPosition(pos_environment_element.x, pos_environment_element.y - 32);
			coin->set_rect(dier->get_rect());
			this->environment_element.push_back(coin);
		}
		dier->set_state(false);
		enemyBody[nof_object]->DestroyFixture(enemyBody[nof_object]->GetFixtureList());
		dier->get_sprite_pntr()->setTextureRect(sf::Rect <int>((dier->get_nof_tile().x + 2) * 32, (dier->get_nof_tile().y + 1) * 32, 32, 32)); // установить тайл смерти
	}
	return;
}

void Game::add_object_to_environment_element(Object* object, b2Body* body){
	environment_element.push_back(object);
	environment_elementBody.push_back(body);
}

Object* Game:: get_player(int nof_player){
	return player[nof_player];
}

b2Body* Game::get_playerBody(int nof_player) {
	return playerBody[nof_player];
}

Object* Game::get_enemy(int nof_object)
{
	return enemy[nof_object];
}

b2Body* Game::get_environment_elementBody(int nof_object) {
	return environment_elementBody[nof_object];
}

b2World* Game::get_world()
{
	return world;
}

b2Body* Game::get_enemyBody(int nof_object) {
	return enemyBody[nof_object];
}


//**************LEVEL****************
bool Level::LoadFromFile(std::string filename) {
	XMLDocument levelFile;
	XMLError eResult = levelFile.LoadFile(filename.c_str());

	if (eResult != XML_SUCCESS) {
		std::cout << "Loading level\"" << filename << "\" failed." << std::endl;
		return false;
	}

	XMLElement* map;
	map = levelFile.FirstChildElement("map");
	
	width = atoi(map->Attribute("width"));
	height = atoi(map->Attribute("height"));
	tileWidth = atoi(map->Attribute("tilewidth"));
	tileHeight = atoi(map->Attribute("tileheight"));

	std::vector <int> firstTileIDs;            // первые id тайлсетов
	sf::Image image;
	std::vector <sf::Vector2u> tileset_sizes; // размеры тайлсетов
	
	//выгрузка тайлсетов
	XMLElement* tilesetElement = map->FirstChildElement("tileset");
	do {
		std::string tilepath = tilesetElement->Attribute("source"); // его путь(.tsx)
		std::string imagepath = "Tiles/";                           // собираем путь к картинке на тайлсет
		for (int i = 0; i < tilepath.size() - 4; i++) {
			imagepath += tilepath[i];
		}
		imagepath += ".png";

		firstTileIDs.push_back(atoi(tilesetElement->Attribute("firstgid")));
		if (!image.loadFromFile(imagepath)) {
			std::cout << "Failed to load tile sheet. "<< imagepath << std::endl;
			return false;
		}

		tileset_sizes.push_back(image.getSize());
		
		tilesetImage.push_back(sf::Texture());
		tilesetImage[tilesetImage.size() - 1].loadFromImage(image);
	} while (tilesetElement = tilesetElement->NextSiblingElement("tileset"));

	std::vector<sf::Rect<int>> subRects; // создание квадратов на тайлсетах
	for (int i = 0; i < tileset_sizes.size(); i++) {
		int columns = tileset_sizes[i].x / tileWidth;
		int rows = tileset_sizes[i].y / tileHeight;
		for (int y = 0; y < rows; y++) {
			for (int x = 0; x < columns; x++) {
				sf::Rect<int> rect;
				rect.top = y * tileHeight;
				rect.height = tileHeight;
				rect.left = x * tileWidth;
				rect.width = tileWidth;
				subRects.push_back(rect);
			}
		}
	}
	

	XMLElement* layerElement;
	layerElement = map->FirstChildElement("layer");
	while (layerElement) {
		Layer layer;
		if (layerElement->Attribute("opacity") != NULL) {
			float opacity = strtod(layerElement->Attribute("opacity"), NULL);
			layer.opasity = 255 * opacity;
		}
		else layer.opasity = 255;

		XMLElement* layerDataElement;
		layerDataElement = layerElement->FirstChildElement("data");
		if (layerDataElement == NULL) {
			std::cout << "Bad map. No layer information found." << std::endl;
			return false; // в гайде этого нет
		}

		XMLElement* tileElement;
		tileElement = layerDataElement->FirstChildElement("tile");

		if (tileElement == NULL) {
			std::cout << "Bad map. No tile information found." << std::endl;
			//return false;
		}

		int x = 0;
		int y = 0;
		for(int rows = 0; rows < height; rows++){ 
			for(int columns = 0; columns < width; columns++){
				if (tileElement->Attribute("gid")) {
					//std::cout << rows << " " << columns << std::endl;
					int i = 0;
					firstTileID = firstTileIDs[firstTileIDs.size() - 1];
					int tileGID = atoi(tileElement->Attribute("gid"));
					int subRectToUse = tileGID - 1; // определяем тайла на тайлсете
					while (tileGID < firstTileID) { // определяем нужный тайлсет
						i++;
						firstTileID = firstTileIDs[firstTileIDs.size() - 1 - i];
					}
					//std::cout << "Sprite?" << std::endl;
					sf::Sprite sprite;
					sprite.setTexture(tilesetImage[tilesetImage.size() - 1 - i]);
					sprite.setTextureRect(subRects[subRectToUse]);
					sprite.setPosition(x * tileWidth, y * tileHeight);
					sprite.setColor(sf::Color(255, 255, 255, layer.opasity));
					
					//std::cout << "ADD" << std::endl;
					layer.tiles.push_back(sprite);
					//std::cout << "ADDED" << std::endl;
				}
				tileElement = tileElement->NextSiblingElement("tile");
				x++; // перевод строки
				if (x >= width) {
					x = 0;
					y++;
					if (y >= height) y = 0;
				}
			}
		}
		//std::cout << "ADDING layer" << std::endl;
		layers.push_back(layer);

		layerElement = layerElement->NextSiblingElement("layer");
	}

	// Объекты
	XMLElement* objectGroupElement;
	if (map->FirstChildElement("objectgroup") != NULL) {
		objectGroupElement = map->FirstChildElement("objectgroup");
		while (objectGroupElement) {

			XMLElement* objectElement;
			objectElement = objectGroupElement->FirstChildElement("object");

			while (objectElement) {
				int i = 0;
				firstTileID = firstTileIDs[firstTileIDs.size() - 1];

				int objectGID = atoi(objectElement->Attribute("gid"));// номер тайла на тайлсете.
				int subRectToUse = objectGID - 1; // номер прямоугольника для уточнения спрайта
				while (objectGID < firstTileID) { // определяем нужный тайлсет
					i++;
					firstTileID = firstTileIDs[firstTileIDs.size() - 1 - i];
				}

				std::string objectType;
				if (objectElement->Attribute("type") != NULL) {
					objectType = objectElement->Attribute("type");
				}
				std::string objectName;
				if (objectElement->Attribute("name") != NULL) {
					objectName = objectElement->Attribute("name");
				}
				int x = atoi(objectElement->Attribute("x"));
				int y = atoi(objectElement->Attribute("y"));

				int width, height;
				sf::Sprite sprite;
				sprite.setTexture(tilesetImage[tilesetImage.size() - 1 - i]);
				//sprite.setTextureRect(sf::Rect <int>(32, 32, 32, 32)); Он думает, что текстура задаётся целиком, но она включена в тайлсет
				sprite.setTextureRect(subRects[subRectToUse]); // использует тайлсет, активный для конкретного объекта
				sprite.setPosition(x, y);
				// взятие ширины и высоты объекта(не используется)
				/* 
				if (objectElement->Attribute("width") != NULL) {
					width = atoi(objectElement->Attribute("width"));
					height = atoi(objectElement->Attribute("height"));
				}
				else{
					width = subRects[atoi(objectElement->Attribute("gid")) - firstTileID].width;
					height = subRects[atoi(objectElement->Attribute("gid")) - firstTileID].height;
					sprite.setTextureRect(subRects[atoi(objectElement->Attribute("gid")) - firstTileID]);
				}
				*/
				// вместо этого 
				height = 32;
				width = 32;

				objects_from_map* object = new objects_from_map();
				object->name = objectName;
				object->type = objectType;
				object->sprite = sprite;
				object->health_bar = sprite;
				object->nof_tile = sf::Vector2i(subRects[subRectToUse].left/32, subRects[subRectToUse].top/32);
				sf::Rect <int> objectRect;
				objectRect.top = y-32;
				objectRect.left = x;
				objectRect.height = height;
				objectRect.width = width;
				object->rect = objectRect;

				//у моих объектов нет пользовательский свойств
				/*
				XMLElement* properties;
				properties = objectElement->FirstChildElement("properties");
				if (properties != NULL) {
					XMLElement* prop;
					prop = properties->FirstChildElement("property");
					if (prop != NULL) {
						while (prop) {
							std::string propertyName = prop->Attribute("name");
							std::string propertyValue = prop->Attribute("value");
							object->properties[propertyName] = propertyValue;
							prop = prop->NextSiblingElement("property");
						}
					}
				}
				*/

				objects.push_back(object);
				objectElement = objectElement->NextSiblingElement("object");
			}
			objectGroupElement = objectGroupElement->NextSiblingElement("objectgroup");

		}
	}
	else {
		std::cout << "No object layers found..." << std::endl;
	}
	return true;
}

objects_from_map* Level::GetObject(std::string type_name){
	// Только первый объект с заданным именем
	for (int i = 0; i < objects.size(); i++)
		if (objects[i]->type == type_name)
			return objects[i];
}

std::vector<objects_from_map*> Level::GetObjects(std::string type_name){
	// Все объекты с заданным именем
	std::vector<objects_from_map*> vec;
	for (int i = 0; i < objects.size(); i++)
		if (objects[i]->type == type_name)
			vec.push_back(objects[i]);

	return vec;
}

sf::Vector2i Level::GetTileSize()
{
	return sf::Vector2i(tileWidth, tileHeight);
}

void Level::clear(){
	objects.clear();
}

void Level::Draw(sf::RenderWindow& window)
{
	// Рисуем все тайлы (объекты НЕ рисуем!)
	for (int layer = 0; layer < layers.size(); layer++)
		for (int tile = 0; tile < layers[layer].tiles.size(); tile++)
			window.draw(layers[layer].tiles[tile]);
}
