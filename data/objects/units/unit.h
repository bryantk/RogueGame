
#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include "../effect.h"

#include "../items/item.h"

class unit {
	char symbol;
	unsigned char color;
public:
	//status
	int id_tag;

	int level;
	int exp;
	int gold;

	int hp[3];		//max	effective max	current
	int str[2];
	int def[2];
	int wis[2];
	int Int[2];
	int eva[2];
	int vis[2];
	
	// elemntal resistances
	int water_res[2];
	int fire_res[2];
	int air_res[2];
	int earth_res[2];

	short int hearing[2];
	short int smell[2];

	std::vector<effect> effects;
	std::string name;
	int x;
	int y;

	short int last_hero_y;
	short int last_hero_x;

	item held_item;
	bool holding_item;
	//void add_effect(effect EFFECT);

  unit() {
	id_tag = -1;

	level = 0;
	exp = 0;
	gold = 0;

	name = "NULL";
	symbol = '!';
	color = 0;
	x = -1;
	y = -1;

		hp[3] = 1;
	for (int i = 0; i < 2; i++) {
		hp[i] = 1;
		str[i] = 0;
		def[i] = 0;
		wis[i] = 0;
		Int[i] = 0;
		eva[i] = 0;
		vis[i] = 0;
		water_res[i]=0;
		fire_res[i]=0;
		air_res[i]=0;
		earth_res[i]=0;	
	}

	holding_item = false;	

  }

  unit(int ID) {
	std::cout << "  PRE  -";
	x = 0;
	y = 0;
	last_hero_y = -1;
	last_hero_x = -1;
	std::string word = "";
	std::ifstream readFile;
	readFile.open("data/objects/units/monsters.dat");
	if (readFile.is_open()) {
		while (!readFile.eof()) {	// while there is still data
			readFile >> word;
			if (word == "id:") {
				readFile >> word;
				if ( ID == atoi(word.c_str())) {
					readFile >> word;	readFile >> word;
				name = word;
					readFile >> word;	readFile >> word;
				symbol = word[0];
					readFile >> word;	readFile >> word;
				color = atoi(word.c_str());
					readFile >> word;	readFile >> word;
				level = atoi(word.c_str());
					readFile >> word;	readFile >> word;
				exp = atoi(word.c_str());
					readFile >> word;	readFile >> word;
				gold = atoi(word.c_str());
					readFile >> word;	readFile >> word;
				for (int i = 0;i<4;i++) hp[i] = atoi(word.c_str());
					readFile >> word;	readFile >> word;
				for (int i = 0;i<2;i++) str[i] = atoi(word.c_str());
					readFile >> word;	readFile >> word;
				for (int i = 0;i<2;i++) def[i] = atoi(word.c_str());
					readFile >> word;	readFile >> word;
				for (int i = 0;i<2;i++) wis[i] = atoi(word.c_str());
					readFile >> word;	readFile >> word;
				for (int i = 0;i<2;i++) Int[i] = atoi(word.c_str());
					readFile >> word;	readFile >> word;
				for (int i = 0;i<2;i++) eva[i] = atoi(word.c_str());
					readFile >> word;	readFile >> word;
				for (int i = 0;i<2;i++) vis[i] = atoi(word.c_str());
					readFile >> word;	readFile >> word;	
				for (int i = 0;i<2;i++) water_res[i] = atoi(word.c_str());
					readFile >> word;	readFile >> word;	
				for (int i = 0;i<2;i++) fire_res[i] = atoi(word.c_str());
					readFile >> word;	readFile >> word;	
				for (int i = 0;i<2;i++) air_res[i] = atoi(word.c_str());
					readFile >> word;	readFile >> word;	
				for (int i = 0;i<2;i++) earth_res[i] = atoi(word.c_str());
					readFile >> word;	readFile >> word;	
				for (int i = 0;i<2;i++) hearing[i] = atoi(word.c_str());
					readFile >> word;	readFile >> word;	
				for (int i = 0;i<2;i++) smell[i] = atoi(word.c_str());
					readFile >> word;	readFile >> word;	//search effects
					while (word != ":end") {
						std::string name = word;
						readFile >> word;
						int pri = atoi(word.c_str());
						readFile >> word;
						int str = atoi(word.c_str());
						readFile >> word;
						int range = atoi(word.c_str());
						readFile >> word;
						int dur = atoi(word.c_str());
						readFile >> word;
						int flag = atoi(word.c_str());
						add_effect(effect(pri, name, str, range, dur, flag));

						readFile >> word;
						std::cout<< word << " ";
					}
					std::cout << "\n";
				}
				
			}
			//std::cout << word;
		}
		word = "";
	}

	readFile.close();
	holding_item = false;	

	std::cout << "  POST file" << std::endl;

  }

  unit(std::string n, int my_y, int my_x, int my_hp) {	// hero temp
	name = n;
	level = 21;
	gold = 0;
	exp = 10;
	x = my_x;
	y = my_y;
	hp[2] = my_hp;
	for (int i = 0; i < 2; i++) {
		hp[i] = my_hp;
		str[i] = 10;
		def[i] = 0;
		wis[i] = 0;
		Int[i] = 0;
		eva[i] = 0;
		vis[i] = 5;
		water_res[i]=0;
		fire_res[i]= 0;
		air_res[i]=  0;
		earth_res[i]=0;	
	}
	holding_item = false;	
  }

  ~unit() {
  }

  std::string get_name() {
	// add a parser tha looks for color words and colors the texxt accoringly
	return name;
  }
  char get_char() {
	return symbol;
  }
    int get_color() {
	return int(color);
  }

  void get_damage_matrix(int (&array)[6] ) {
	array[0] = str[1];
	if (find_effect("fire_attack")) array[1] = find_effect_strength("fire_attack");
	if (find_effect("water_attack")) array[2] = find_effect_strength("water_attack");
	if (find_effect("earth_attack")) array[3] = find_effect_strength("earth_attack");
	if (find_effect("air_attack")) array[4] = find_effect_strength("air_attack");
	if (find_effect("undefendable_attack")) array[4] = find_effect_strength("undefendable_attack");
	//array[1] = 100;
	return;
  }

  void resist_damage_matrix(int (&array)[6] ) {
	array[0] -= def[1]/4;
	if (array[0]<0) array[0] = 0;	//physical attacks cannot deal less than 0
	array[1] = array[1] * (float(100-(-50))/float(100));	//fire
	return;
  }

  void tick_effects() {
	std::vector<effect>::iterator it = effects.begin();
	int i = 0;
	while (i < effects.size()) {
		// do effect
		if (effects[i].duration == 0) {
			effects.erase(effects.begin()+i);
			if (i+1 >= effects.size()) return;
		}
		if (effects[i].duration > 0) {
			effects[i].duration--;
		}
		if (effects[i].overrides()) return;	//condition halts progress of those after it
		i++;
	}
	return;
  }

  void add_effect(effect EFFECT) {
	for (int i = 0; i < effects.size(); i++) {
		if (effects[i].get_tag() == EFFECT.get_tag()) {
			if (effects[i].additive_str()) {
				effects[i].strength+= EFFECT.strength;
			}
			if (effects[i].additive_dur()) {
				effects[i].duration+= EFFECT.duration;
			}	
			return;
		}
	}
	int i = 0;
	if (effects.size() > 0) {
		while (effects[i].priority >= EFFECT.priority && i < effects.size() ) i++;
	}
	effects.insert(effects.begin()+i, EFFECT);
	return;
  }
	
  bool find_effect(std::string tag) {
	for (int i = 0; i < effects.size(); i++) { 
		if (effects[i].get_tag() == tag) return true; 
	}
	return false;
  }

  int find_effect_strength(std::string tag) {
	for (int i = 0; i < effects.size(); i++) {
		if (effects[i].get_tag() == tag) return effects[i].strength; 
	}
	return 0;
  }



  void set_location(const int& item_y, const int& item_x) {
	y= item_y;
	x= item_x;
	return;
  }

  bool check_location(const int& dy, const int& dx){
	if (dy == y && dx == x) return true;
	return false;
  }
  void get_location(int& item_y, int& item_x) {
	item_y = y;
	item_x = x;
	return;
  }

  virtual void tick() {
	//do nothing	(override in child classes)
  	return;
  }

// needed? what about tick_effects?
  void apply_effects() {

  }

//applys all equipment specifict effects/buffs.
//Mainly called for/by the hero
  void apply_equ_effects() {
	hp[1] += find_effect_strength("equ_hp");
	str[1] +=find_effect_strength("equ_str");
	def[1] +=+find_effect_strength("equ_def");
	wis[1] += find_effect_strength("equ_wis");
	Int[1] += find_effect_strength("equ_int");
	vis[1] += find_effect_strength("equ_vis");
	eva[1] += find_effect_strength("equ_eva");
	fire_res[1] += find_effect_strength("equ_fire");
	water_res[1] += find_effect_strength("equ_water");
	earth_res[1] += find_effect_strength("equ_earth");
	air_res[1] += find_effect_strength("equ_air");
  }

  void strip_armor_effects() {
	int i = 0;
	while (i < effects.size() ) {
		if (effects[i].equipment() ) {
			effects.erase(effects.begin()+i);
			i--;
		} 
		i++;
	}
  }

  void reset_stats() {			// can also be used for monsters
	hp[1] = hp[0];
	str[1] = str[0];
	def[1] = def[0];
	wis[1] = wis[0];
	Int[1] = Int[0];
	vis[1] = vis[0];
	eva[1] = eva[0];

	fire_res[1] = fire_res[0];
	water_res[1] = water_res[0];
	earth_res[1] = earth_res[0];
	air_res[1] = air_res[0];
  }


};


/* TO DO

monster system
 int last seen player x, y
 int last heard player x, y
 int hearing
 int smell
	int smell/hearing rating (how large a vallue will trigger)



monsters:
  spawn new ones
     via time?
     when two similar species interact?


 percistent/ learning ai



roguhttp://roguebasin.roguelikedevelopment.org/index.php?title=Articles#Programming_languagesebasin
*/

