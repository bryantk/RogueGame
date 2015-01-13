#pragma once

#include <vector>
#include <string>
#include <sstream>

#include <cmath>

#include "../data/data.h"
#include "../data/map_data/map_data.h"
#include "../data/objects/obj_data.h"

//#include "../data/objects/effect.h"
#include "../data/objects/items/item.h"
#include "../data/objects/units/unit.h"

  bool spawn_item(data& DATA, item ITEM, int y=-1, int x=-1, int quantity=1) {
	if (y<0 || x<0 || y>256 || x>256) return false;
	if ( DATA.MAP_DATA.map[y][x].get_has_item() ) return false;
	if ( !DATA.MAP_DATA.map[y][x].get_passability() ) return false;
	int q = ITEM.find_effect_index("quantity");
	if (quantity >1 && q>=0 )  ITEM.get_effect(q).strength = quantity;
	ITEM.set_location(y,x);
	DATA.OBJ_DATA.item_list.push_back(ITEM);
	DATA.MAP_DATA.map[y][x].set_has_item(true);

	return true;
  }

  void spawn_monster(data& DATA, int id, int y, int x) {
	unit MONSTER = unit(id);
	MONSTER.set_location(y,x);
	DATA.OBJ_DATA.monster_list.push_back(MONSTER);	
	DATA.MAP_DATA.map[y][x].set_has_unit(true);
  }



  void load_monster(data& DATA, std::string in) {				// Work in progress		needs effects
	// string will come from saved data. most likly in the form of id,stat,stat....stat,last_hero_y, last_hero_x, begin:,effects,:end
	int stats[28];
	for (int i = 0; i < 26; i++) stats[i] = 0;
	std::string token = "";
	std::istringstream iss(in);
	int ctr = 0;
	std::getline( iss, token, ',');
	while (token != "begin:") {
		stats[ctr] = atoi(token.c_str());
		ctr++;
		std::getline( iss, token, ',');
	}

	unit MONSTER = unit(stats[0]);
	MONSTER.set_location(stats[1],stats[2]);					//1 2
	MONSTER.level = stats[3];	
	MONSTER.exp = stats[4];
	MONSTER.gold = stats[5];
	for (int i = 0; i < 3; i++) MONSTER.hp[i] = stats[i+6];		//6 7 8 
	for (int i = 0; i < 2; i++) MONSTER.str[i] = stats[i+9];	//9 
	for (int i = 0; i < 2; i++) MONSTER.def[i] = stats[i+11];	//11
	for (int i = 0; i < 2; i++) MONSTER.wis[i] = stats[i+14];	//13
	for (int i = 0; i < 2; i++) MONSTER.Int[i] = stats[i+15];	//15
	for (int i = 0; i < 2; i++) MONSTER.eva[i] = stats[i+17];	//17
	for (int i = 0; i < 2; i++) MONSTER.vis[i] = stats[i+19];	//19
	for (int i = 0; i < 2; i++) MONSTER.hearing[i] = stats[i+21];	//21
	for (int i = 0; i < 2; i++) MONSTER.smell[i] = stats[i+23];	//23
	MONSTER.last_hero_y = stats[26];
	MONSTER.last_hero_x = stats[27];

	int y, x;
	MONSTER.get_location(y,x);
	DATA.OBJ_DATA.monster_list.push_back(MONSTER);
	DATA.MAP_DATA.map[y][x].set_has_unit(true);
  }
