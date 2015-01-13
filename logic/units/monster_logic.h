#pragma once

#include <vector>
#include <string>
#include <sstream>

#include <cmath>

#include "../../data/data.h"
#include "../../data/map_data/map_data.h"
#include "../../data/objects/obj_data.h"

#include "../../data/objects/items/item.h"		//possibly not needed
#include "../../data/objects/units/unit.h"

// Deals with and conrtols monster turn movement.



  int direction_to(unit& target, unit& source) {
	if (target.y < source.y) {
		if (target.x < source.x) return 7;
		else if (target.x == source.x) return 8;
		else return 9;
	}
	else if (target.y == source.y) {
		if (target.x < source.x) return 4;
		//else if (target.x == source.x) return 5;
		else return 6;
	}
	else {
		if (target.x < source.x) return 1;
		else if (target.x == source.x) return 2;
		else return 3;
	}
	return 5;
  }

  bool try_move(map_data& MAP_DATA, unit& UNIT, int dy, int dx) {
	if (MAP_DATA.map[UNIT.y+dy][UNIT.x+dx].get_passability() && !MAP_DATA.map[UNIT.y+dy][UNIT.x+dx].get_has_unit()) {
			MAP_DATA.map[UNIT.y][UNIT.x].set_has_unit(false);
			UNIT.y += dy;	
			UNIT.x += dx;
			MAP_DATA.map[UNIT.y][UNIT.x].set_has_unit(true);
			return true;
		}
	return false;
  }

  void attack(unit& target, unit& source) {
	//target.hp--;
	target.hp[2] -= (source.str[1]-target.def[1]);
	return;
  }

  void move_to_hero(data& DATA, unit& UNIT) {
	int direction = direction_to(DATA.OBJ_DATA.hero, UNIT);
	switch (direction) {
		case 1:	
			if (try_move(DATA.MAP_DATA, UNIT, 1, -1)) return;
			if (try_move(DATA.MAP_DATA, UNIT, 1, 0)) return;
			if (try_move(DATA.MAP_DATA, UNIT, 0, -1)) return;
		break;
		case 2:
			if (try_move(DATA.MAP_DATA, UNIT, 1, 0)) return;
			if (try_move(DATA.MAP_DATA, UNIT, 1, -1)) return;
			if (try_move(DATA.MAP_DATA, UNIT, 1, 1)) return;
		break;
		case 3:
			if (try_move(DATA.MAP_DATA, UNIT, 1, 1)) return;
			if (try_move(DATA.MAP_DATA, UNIT, 1, 0)) return;
			if (try_move(DATA.MAP_DATA, UNIT, 0, 1)) return;
		break;
		case 6:
			if (try_move(DATA.MAP_DATA, UNIT, 0, 1)) return;
			if (try_move(DATA.MAP_DATA, UNIT, -1, 1)) return;
			if (try_move(DATA.MAP_DATA, UNIT, 1, 1)) return;
		break;
		case 9:	
			if (try_move(DATA.MAP_DATA, UNIT, -1, 1)) return;
			if (try_move(DATA.MAP_DATA, UNIT, -1, 0)) return;
			if (try_move(DATA.MAP_DATA, UNIT, 0, 1)) return;
		break;
		case 8:
			if (try_move(DATA.MAP_DATA, UNIT, -1, 0)) return;
			if (try_move(DATA.MAP_DATA, UNIT, -1, -1)) return;
			if (try_move(DATA.MAP_DATA, UNIT, -1, 1)) return;
		break;
		case 7:
			if (try_move(DATA.MAP_DATA, UNIT, -1, -1)) return;
			if (try_move(DATA.MAP_DATA, UNIT, 0, -1)) return;
			if (try_move(DATA.MAP_DATA, UNIT, -1, 0)) return;
		break;
		case 4:	
			if (try_move(DATA.MAP_DATA, UNIT, 0, -1)) return;
			if (try_move(DATA.MAP_DATA, UNIT, -1, -1)) return;
			if (try_move(DATA.MAP_DATA, UNIT, 1, -1)) return;
		break;
	}

  }


	//----------------------------------------------------------------------
	//
	//		M O N S T E R   D E A T H   H A N D L E R
	//
	//----------------------------------------------------------------------
  bool monster_death(data& DATA, int index) {
	unit * monster;
	monster = &(DATA.OBJ_DATA.monster_list[index]);
	// what if monster has revival effect?

	if (monster->hp[2] < 1) {			
		DATA.MAP_DATA.map[DATA.OBJ_DATA.monster_list[index].y][DATA.OBJ_DATA.monster_list[index].x].set_has_unit(false);
		DATA.add_message(DATA.parser(DATA.OBJ_DATA.monster_list[index].get_name() + " has been slain!"));
		DATA.OBJ_DATA.monster_list.erase(DATA.OBJ_DATA.monster_list.begin()+index);
		monster = NULL;
		return true;
	}
	monster = NULL;
	return false;
  }
	//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	


  void L_MON_move_monsters(data& DATA) {
	int i = 0;	
	while ( i < DATA.OBJ_DATA.monster_list.size() ) {

		int dy, dx;
		dy = abs(DATA.OBJ_DATA.monster_list[i].y-DATA.OBJ_DATA.hero.y);
		dx = abs(DATA.OBJ_DATA.monster_list[i].x-DATA.OBJ_DATA.hero.x);
		if ( DATA.OBJ_DATA.monster_list[i].hp[2] < 1 ) {	// if hp < 0    do monster death
			if (monster_death(DATA,i) )	continue;	
		}

		if ( (pow(dx,2) + pow(dy,2)) <= pow(DATA.OBJ_DATA.monster_list[i].vis[1],2) ) {	//if outside of view radius
			if (dx < 2 && dy < 2) attack(DATA.OBJ_DATA.hero, DATA.OBJ_DATA.monster_list[i]); //attack!
			else
			move_to_hero(DATA, DATA.OBJ_DATA.monster_list[i]);
		}
	i++;
	}
	return;
  }




