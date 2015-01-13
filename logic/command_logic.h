#pragma once

#include <vector>
#include <string>
#include <sstream>

#include <cmath>

#include "../data/data.h"
#include "../data/map_data/map_data.h"
#include "../data/objects/obj_data.h"

#include "../data/objects/items/item.h"
#include "../data/objects/units/unit.h"


	//----------------------------------------------------------------------
	//
	//		H E R O		E Q U I P      I T E M
	//
	//----------------------------------------------------------------------
  bool CL_equip_item(data& DATA) {
	if ( DATA.OBJ_DATA.equipment[DATA.selector.selector_1].is_cursed() ) 	{
		if ( DATA.OBJ_DATA.equipment[DATA.selector.selector_1].is_known() ) 
			DATA.selector.display_message = " Can not remove CURSED equipment. ";
		else
			DATA.selector.display_message = " Can not remove equipment. ";
		DATA.selector.T_F_bool_0 = true;	// needs confirmation/error message
		return false;
	}
	int current_size = DATA.OBJ_DATA.inventory.size();
	int max_size = DATA.OBJ_DATA.inventory_size;
	if ( current_size > max_size  ) 	{
		DATA.selector.T_F_bool_0 = true;	// needs confirmation/error message
		DATA.selector.display_message = " No space in inventory. ";
		return false;
	}
	if ( DATA.selector.selector_1 == 6 && DATA.OBJ_DATA.equipment[4].is_both() ) {
		DATA.selector.T_F_bool_0 = true;	// needs confirmation/error message
		DATA.selector.display_message = " Currently wielding a 2 Handed weapon. ";
		return false;
	}
	if ( DATA.selector.selector_1 == 4 && DATA.OBJ_DATA.inventory[DATA.selector.item].is_both() ) {	//equiping 2handed weapon
		if ( DATA.OBJ_DATA.equipment[6].get_id() != 0  ) 	{									// and have items in both slots
			if ( current_size >= max_size ) {
				DATA.selector.T_F_bool_0 = true;	// needs confirmation/error message
				DATA.selector.display_message = " No space in inventory. ";
				return false;
			}
			DATA.OBJ_DATA.inventory.push_back(DATA.OBJ_DATA.equipment[6]);
		}
	}

	if ( DATA.selector.selector_1 == 4 && DATA.OBJ_DATA.equipment[4].is_both() ) {	// zero out 2 handed weapons
		DATA.OBJ_DATA.equipment[6].set_name("None"); 
	}

//	item temp = item();
	item temp = DATA.OBJ_DATA.equipment[DATA.selector.selector_1];
	DATA.OBJ_DATA.equipment[DATA.selector.selector_1] = DATA.OBJ_DATA.inventory[DATA.selector.item];	//equip item
	if ( DATA.OBJ_DATA.inventory[DATA.selector.item].is_both() ) {
		item lefty = item();
		std::string fill_me = " ";
		for (int i = 1; i < DATA.OBJ_DATA.inventory[DATA.selector.item].get_name().size(); i++ )
			fill_me.push_back('-');
		lefty.set_name(fill_me);
		DATA.OBJ_DATA.equipment[6] = lefty;	
	}
	DATA.OBJ_DATA.inventory.erase(DATA.OBJ_DATA.inventory.begin()+DATA.selector.item);				//remove item from inventory
	DATA.selector.item--;
	//DATA.OBJ_DATA.identify(DATA.OBJ_DATA.equipment[DATA.selector.selector_1]);		// identify equipment
	if (temp.get_id() != 0) DATA.OBJ_DATA.inventory.push_back(temp);	//place old item back in inventory
	//DATA.add_message(" EQUIP ");
	return true;
  }

	//----------------------------------------------------------------------
	//
	//		H E R O		U S E	T I L E
	//
	//----------------------------------------------------------------------
  void CL_use_tile(data& DATA, int dy, int dx) {
	int tile_y = DATA.OBJ_DATA.hero.y+dy;
	int tile_x = DATA.OBJ_DATA.hero.x+dx;
	bool self_tile = false;
	if (dy==0 && dx==0) self_tile = true;
	char tile = DATA.MAP_DATA.map[tile_y][tile_x].get_char();
	switch (tile) {
		case '/':
			if (DATA.MAP_DATA.map[tile_y][tile_x].get_is_door()) {
				if (self_tile) return;
				if (DATA.MAP_DATA.map[tile_y][tile_x].get_key_locked())
					DATA.add_message(DATA.parser("|c3Locked."));
				else {
					DATA.MAP_DATA.map[tile_y][tile_x].set_passability(true);
					DATA.MAP_DATA.map[tile_y][tile_x].set_char('_');
					DATA.MAP_DATA.map[tile_y][tile_x].set_blocks_vision(false);
					DATA.add_message(DATA.parser("You open the |c3DOOR."));
				}
			}
			break; 
		case '_':
			if (DATA.MAP_DATA.map[tile_y][tile_x].get_is_door()) {
				if (self_tile) return;
				if (DATA.MAP_DATA.map[tile_y][tile_x].get_key_locked())
					DATA.add_message(DATA.parser("It is |c3Locked."));
				else {
					DATA.MAP_DATA.map[tile_y][tile_x].set_passability(false);
					DATA.MAP_DATA.map[tile_y][tile_x].set_char('/');
					DATA.MAP_DATA.map[tile_y][tile_x].set_blocks_vision(true);
					DATA.add_message(DATA.parser("You close the |c3DOOR."));
				}
			}
			break; 
		default:;
	}
	return;
  }


	//----------------------------------------------------------------------
	//
	//		H E R O		A T T A C K     M O N S T E R
	//
	//----------------------------------------------------------------------
  void CL_hero_attack(data& DATA, int index) {
	unit * monster;
	monster = &(DATA.OBJ_DATA.monster_list[index]);
	unit * hero;
	hero = &(DATA.OBJ_DATA.hero);
			//damage algorithm

	int damage = 0;
	int damage_matrix[6];
	for (int i = 0; i < 6; i++)	damage_matrix[i] = 0;
	hero->get_damage_matrix(damage_matrix);
	monster->resist_damage_matrix(damage_matrix);
	for (int i = 0; i < 6; i++) damage += damage_matrix[i];
	damage = float(damage)*float(rand()%21+90)/float(100);

	if ( rand()%101 < monster->eva[1] )	{		// hit or miss
		damage = float(damage)*0;
		DATA.add_message(DATA.parser("You strike the "+monster->get_name()+" but miss."));
	}
	else {
	if ( rand()%101 > 95 ) {		//5% chance to critical
		damage = float(damage)*1.5;
		DATA.add_message(DATA.parser("You criticaly strike the "+monster->get_name()+" for |cB"+ to_str(damage) +"|cb damage."));
	}
	else
		DATA.add_message(DATA.parser("You strike the "+monster->get_name()+" for "+ to_str(damage) +" damage."));	
	}

	monster->hp[2] -= damage;
	hero = NULL;
	monster = NULL;

	return;
  }

	//----------------------------------------------------------------------
	//
	//		H E R O		C A S T	  S P E L L
	//
	//----------------------------------------------------------------------
  void CL_cast_spell(data& DATA, int dy, int dx, int target_y, int target_x) {
	//DATA.MAP_DATA.map[target_y][target_x].set_color(3);	//for debug
	//DATA.MAP_DATA.map[target_y][target_x].set_char('!');//for debug
	unit * target = NULL;
	if ( DATA.MAP_DATA.map[target_y][target_x].get_has_unit() ) {
		for (int i = 0; i < DATA.OBJ_DATA.monster_list.size(); i++) {
			int y = 0;
			int x = 0;
			if (DATA.OBJ_DATA.monster_list[i].check_location(target_y, target_x) ) {
				target = &(DATA.OBJ_DATA.monster_list[i]);
				break;
			}				
		}
	}
DATA.add_message("in casting main");
	if (target == NULL) {
		DATA.add_message("DEBUG No target");
		return;
	}
	target->hp[2] -= 100;
	//target = &(DATA.OBJ_DATA.monster_list[index]);
	//set temp item
	//if item 
		//remove
		//if durable, see if survided, if so, drop, if not room, destroy.	use drop logic... with some additions
	//run effects

	//return	

	//DATA.OBJ_DATA.spell_line_cast	dont need to reference for spell has already traveled
  }
