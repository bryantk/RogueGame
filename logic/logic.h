#pragma once


#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include <cmath>


#include "drawing/map_drawing.h"

#include "command_logic.h"

#include "../data/data.h"
#include "../data/map_data/map_data.h"
#include "../data/objects/obj_data.h"

#include "../data/objects/items/item.h"
#include "../data/objects/units/unit.h"

	bool is_vowel(char c);
	std::string L_to_upper(std::string in);

  void drop_logic(data& DATA, int dy, int dx, int quantity = 1, bool destroyed=false) {
	DATA.log_file("	  dropping at " + to_str(dy) + "," + to_str(dy) + ".\n");
	// temperary item to correctly display the quantity.
	item temp_item = DATA.OBJ_DATA.inventory[DATA.selector.item];
	if ( temp_item.find_effect("quantity") )
		temp_item.get_effect(temp_item.find_effect_index("quantity")).set_strength(quantity);
	std::string temp = DATA.OBJ_DATA.item_name(temp_item);
		//std::string temp = DATA.OBJ_DATA.item_name(DATA.OBJ_DATA.inventory[DATA.selector.item]);
	if ( !destroyed) {
		if (is_vowel(temp[0]))
			temp = "You drop an " + L_to_upper(temp) + " upon the floor.";		
		else
			temp = "You drop a " + L_to_upper(temp) + " upon the floor.";		
	}
	else  {
		temp = L_to_upper(temp) + " shatters.";	
		DATA.log_file("	  SHATTERED.\n");	
	}
	DATA.add_message(DATA.parser(temp));

 	if ( !(DATA.OBJ_DATA.inventory[DATA.selector.item].find_effect("quantity")) ) {	// if not a stackable item
		DATA.log_file("	  non-stackable item.\n");	
		DATA.OBJ_DATA.inventory[DATA.selector.item].set_location(dy, dx);
		if ( !destroyed ) {
			DATA.MAP_DATA.map[dy][dx].set_has_item(true);
			DATA.OBJ_DATA.item_list.push_back(DATA.OBJ_DATA.inventory[DATA.selector.item]);
		}
		DATA.OBJ_DATA.inventory.erase(DATA.OBJ_DATA.inventory.begin()+DATA.selector.item);
		if (DATA.OBJ_DATA.spell_info.fallback_id == DATA.selector.item) DATA.OBJ_DATA.spell_info.fallback_id= -1;
		DATA.selector.item--;
	DATA.selector.bool_1 = false;
	DATA.selector.bool_0 = false;
		// item stack has been dropped
		if (DATA.OBJ_DATA.spell_info.fallback_id >	DATA.selector.item) {	// decrement quick ability reference
			DATA.OBJ_DATA.spell_info.fallback_id--;				
		}	

	}
	else {
		// if last item of stack
		DATA.log_file("	  stackable item.\n");	
		if (DATA.OBJ_DATA.inventory[DATA.selector.item].get_effect( DATA.OBJ_DATA.inventory[DATA.selector.item].find_effect_index("quantity") ).strength==quantity ) {
			DATA.OBJ_DATA.inventory[DATA.selector.item].set_location(dy, dx);
			if ( !destroyed ) {
				DATA.MAP_DATA.map[dy][+dx].set_has_item(true);
				DATA.OBJ_DATA.item_list.push_back(DATA.OBJ_DATA.inventory[DATA.selector.item]);
			}
			DATA.OBJ_DATA.inventory.erase(DATA.OBJ_DATA.inventory.begin()+DATA.selector.item);
			if (DATA.OBJ_DATA.spell_info.fallback_id == DATA.selector.item) DATA.OBJ_DATA.spell_info.fallback_id= -1;
			DATA.selector.item--;
	DATA.selector.bool_1 = false;
	DATA.selector.bool_0 = false;
			// item stack has been dropped
			if (DATA.OBJ_DATA.spell_info.fallback_id >	DATA.selector.item) {	// decrement quick ability reference
				DATA.OBJ_DATA.spell_info.fallback_id--;				
			}		
			DATA.log_file("	  Exit (all stack) dropping.\n");		
			return;
		}
		

		item temp = DATA.OBJ_DATA.inventory[DATA.selector.item];
		temp.get_effect( temp.find_effect_index("quantity") ).set_strength(quantity);
		temp.set_location(dy, dx);
		if ( !destroyed ) {
			DATA.MAP_DATA.map[dy][dx].set_has_item(true);
			DATA.OBJ_DATA.item_list.push_back(temp);			// push back new single item
		}
		DATA.OBJ_DATA.inventory[DATA.selector.item].get_effect( temp.find_effect_index("quantity") ).add_strength(-1*quantity);		//remove one from quantity
	//	DATA.log_file("	  " + to_str(DATA.OBJ_DATA.inventory[DATA.selector.item].get_effect( temp.find_effect_index("quantity") ).strength) + " remaining.\n");	
		DATA.log_file("	  Exit (stack) dropping.\n");	
		return;
	}
		DATA.log_file("	  Exit (sinlge item) dropping.\n");	
		return;
  }

  void drop_item(data& DATA, int target_y, int target_x, int quantity) {
	DATA.action_performed = true;
	int y = DATA.OBJ_DATA.hero.y;
	int x = DATA.OBJ_DATA.hero.x;
	if (target_y != -1  && target_x != -1) {
	//	DATA.add_message("thrown item");
		y = target_y;
		x = target_x;
	}
	// if space to drop
	if ( !DATA.MAP_DATA.map[y+0][x+0].get_has_item()  && DATA.MAP_DATA.map[y][x].get_passability()&& (target_y != -2) )		//target_y = -2 signifies we must destoy the item.
		drop_logic(DATA, y, x, quantity, false);
	else if ( !DATA.MAP_DATA.map[y-1][x+0].get_has_item() && DATA.MAP_DATA.map[y-1][x+0].get_passability() && (target_y != -2) )	//target_y = -2 signifies we must destoy the item.
		drop_logic(DATA, y-1, x, quantity, false);
	else if ( !DATA.MAP_DATA.map[y-1][x+1].get_has_item() && DATA.MAP_DATA.map[y-1][x+1].get_passability() && (target_y != -2) )
		drop_logic(DATA, y-1, x+1, quantity, false);
	else if ( !DATA.MAP_DATA.map[y-0][x+1].get_has_item() && DATA.MAP_DATA.map[y-0][x+1].get_passability() && (target_y != -2) )
		drop_logic(DATA, y, x+1, quantity, false);
	else if ( !DATA.MAP_DATA.map[y+1][x+1].get_has_item() && DATA.MAP_DATA.map[y+1][x+1].get_passability() && (target_y != -2) )
		drop_logic(DATA, y+1, x+1, quantity, false);
	else if ( !DATA.MAP_DATA.map[y+1][x+0].get_has_item() && DATA.MAP_DATA.map[y+1][x+0].get_passability() && (target_y != -2) )
		drop_logic(DATA, y+1, x, quantity, false);
	else if ( !DATA.MAP_DATA.map[y+1][x-1].get_has_item() && DATA.MAP_DATA.map[y+1][x-1].get_passability() && (target_y != -2) )
		drop_logic(DATA, y+1, x-1, quantity, false);
	else if ( !DATA.MAP_DATA.map[y-0][x-1].get_has_item() && DATA.MAP_DATA.map[y-0][x-1].get_passability() && (target_y != -2) )
		drop_logic(DATA, y, x-1, quantity, false);
	else if ( !DATA.MAP_DATA.map[y-1][x-1].get_has_item() && DATA.MAP_DATA.map[y-1][x-1].get_passability() && (target_y != -2) )
		drop_logic(DATA, y-1, x-1, quantity, false);
	else {
		if (target_y != -1  && target_x != -1)
			drop_logic(DATA, 6, 8, quantity, true);
		else {
			

			item temp_item = DATA.OBJ_DATA.inventory[DATA.selector.item];
			if ( temp_item.find_effect("quantity") )
				temp_item.get_effect(temp_item.find_effect_index("quantity")).set_strength(quantity);
			std::string temp = DATA.OBJ_DATA.item_name(temp_item);

		//	std::string temp = DATA.OBJ_DATA.item_name(DATA.OBJ_DATA.inventory[DATA.selector.item]);
			if (is_vowel(temp[0]))
				temp = "You attempt to drop an " + L_to_upper(temp) + ", but there is no room.";		
			else
				temp = "You attempt to drop a " + L_to_upper(temp) + ", but there is no room.";	
			DATA.add_message(DATA.parser(temp));
		}
	DATA.selector.bool_1 = false;
	DATA.selector.bool_0 = false;
		DATA.state = GAME;	// leave menu
	}
//	if (DATA.selector.item < 0 ) DATA.selector.item = 0;
//	clear flags	//***^
	//DATA.selector.one_time_bool = false;
	//DATA.selector.one_time_bool_2 = false;		
	//DATA.selector.quantity = 1;
  }

  char get_direction(data& DATA) {
	if (DATA.input == DATA.NW_KEY ) {									return 7;	}
	else if (DATA.input == DATA.N_KEY || DATA.input == DATA.N_ARROW ) {	return 8;	}
	else if (DATA.input == DATA.NE_KEY ) {								return 9;	}
	else if (DATA.input == DATA.W_KEY || DATA.input == DATA.W_ARROW ) {	return 4;	}
	else if (DATA.input == DATA.CENTER_KEY) {							return 5;	}
	else if (DATA.input == DATA.E_KEY || DATA.input == DATA.E_ARROW ) {	return 6;	}
	else if (DATA.input == DATA.SW_KEY ) {								return 1;	}
	else if (DATA.input == DATA.S_KEY || DATA.input == DATA.S_ARROW ) {	return 2;	}
	else if (DATA.input == DATA.SE_KEY ) {								return 3;	}

	return 0;
  }

  void get_direction(data& DATA, int& y, int& x) {
		 if (DATA.input == DATA.NW_KEY ) {								y = -1;	x = -1;	}
	else if (DATA.input == DATA.N_KEY || DATA.input == DATA.N_ARROW ) {	y = -1;	x =  0;	}
	else if (DATA.input == DATA.NE_KEY ) {								y = -1;	x =  1;	}
	else if (DATA.input == DATA.W_KEY || DATA.input == DATA.W_ARROW ) {	y =  0;	x = -1;	}
	else if (DATA.input == DATA.CENTER_KEY) {							y =  0;	x =  0;	}
	else if (DATA.input == DATA.E_KEY || DATA.input == DATA.E_ARROW ) {	y =  0;	x =  1;	}
	else if (DATA.input == DATA.SW_KEY ) {								y =  1;	x = -1;	}
	else if (DATA.input == DATA.S_KEY || DATA.input == DATA.S_ARROW ) {	y =  1;	x =  0;	}
	else if (DATA.input == DATA.SE_KEY ) {								y =  1;	x =  1;	}

	return;
  }

  void center_map(data& DATA) {
	DATA.MAP_DATA.map_x = DATA.OBJ_DATA.hero.x - DATA.map_width/2; 				//center map on player
	if (DATA.OBJ_DATA.hero.x <= DATA.map_width/2) DATA.MAP_DATA.map_x = 0;
	else if (DATA.OBJ_DATA.hero.x > DATA.map_max_width-(DATA.map_width/2)) DATA.MAP_DATA.map_x =DATA.map_max_width-DATA.map_width; 
	DATA.MAP_DATA.map_y = DATA.OBJ_DATA.hero.y - DATA.map_height/2; 
	if (DATA.OBJ_DATA.hero.y <= DATA.map_height/2) DATA.MAP_DATA.map_y = 0;
	else if (DATA.OBJ_DATA.hero.y >= DATA.map_max_height-(DATA.map_height/2)) DATA.MAP_DATA.map_y = DATA.map_max_height-DATA.map_height; 

	return;
  }

  void L_looking(data& DATA, int& dy, int& dx) {
	int y, x;
	get_direction(DATA, y, x);
	dy+=y;
	dx+=x;
	if (dy < 0) dy = 0;
	if (dy > 24) dy = 24;
	if (dx < 0) dx = 0;
	if (dx > 49) dx = 49;
  }

   bool step_hero(data& DATA, int dy, int dx) {		//used for auto-move
	if (DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y+dy][DATA.OBJ_DATA.hero.x+dx].get_passability() && !DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y+dy][DATA.OBJ_DATA.hero.x+dx].get_has_unit()) {
			DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y][DATA.OBJ_DATA.hero.x].set_has_unit(false);
			DATA.OBJ_DATA.hero.y+=dy;
			DATA.OBJ_DATA.hero.x+=dx;
			DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y][DATA.OBJ_DATA.hero.x].set_has_unit(true);
			center_map(DATA);
			return true;
	}
	return false;
  }

  void hero_attack(data& DATA, int dy, int dx) {
	int i = 0;
	while ( i < DATA.OBJ_DATA.monster_list.size() ) {
		if (DATA.OBJ_DATA.monster_list[i].y == (DATA.OBJ_DATA.hero.y+dy) && DATA.OBJ_DATA.monster_list[i].x == (DATA.OBJ_DATA.hero.x+dx)) {	
			break;
		} 
		i++;
	}
	CL_hero_attack(DATA, i);
  }

  void move_hero(data& DATA) {
	if ( DATA.OBJ_DATA.hero_data.immobilized ) return;	//if immobilized
	int dy, dx;
	get_direction(DATA, dy, dx);
	if (DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y+dy][DATA.OBJ_DATA.hero.x+dx].get_passability() || DATA.OBJ_DATA.hero.find_effect("wall_walking")) {	// or wall walking
		if (!DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y+dy][DATA.OBJ_DATA.hero.x+dx].get_has_unit() &&
				!DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y+dy][DATA.OBJ_DATA.hero.x+dx].get_indestructability()) {
			DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y][DATA.OBJ_DATA.hero.x].set_has_unit(false);
			DATA.OBJ_DATA.hero.y+=dy;
			DATA.OBJ_DATA.hero.x+=dx;
			DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y][DATA.OBJ_DATA.hero.x].set_has_unit(true);
		}
		else {	//attack!
			if (dx==0 && dy == 0) return;
			if ( DATA.OBJ_DATA.hero_data.cant_act ) { DATA.add_message(DATA.OBJ_DATA.hero_data.cant_act_message);	DATA.action_performed = true;  return;}	//if cant take action
			hero_attack(DATA, dy, dx);
		}
	}
	center_map(DATA);

	return;
  }

  int get_item(data& DATA) {	//returns index of item hero is standing on.
	int item_y, item_x;
	for (int i = 0; i < DATA.OBJ_DATA.item_list.size(); i++) {
		DATA.OBJ_DATA.item_list[i].get_location(item_y, item_x);
		if (item_y == DATA.OBJ_DATA.hero.y && item_x == DATA.OBJ_DATA.hero.x) return i;
	}
	return -1;
  }

  int inventory_weight(data& DATA) {
	int total = 0;
	for (int i = 0; i < DATA.OBJ_DATA.inventory.size(); i ++) {
		int effect_location = DATA.OBJ_DATA.inventory[i].find_effect_index("quantity");
		if (effect_location == -1) 		//if non-stackable
			total += DATA.OBJ_DATA.inventory[i].get_weight();
		else	
			total += ( DATA.OBJ_DATA.inventory[i].get_weight() * DATA.OBJ_DATA.inventory[i].get_effect(effect_location).strength );
	}	
	return total;
  }

  void L_use(data& DATA) {
	if ( DATA.OBJ_DATA.hero_data.cant_act ) { DATA.add_message(DATA.OBJ_DATA.hero_data.cant_act_message);	DATA.action_performed = true;  return;}	//if cant take action
	int dy, dx;
	DATA.action_performed = true;
	if (DATA.is_confirm()) {dy = 0; dx =0;}
	else get_direction(DATA,dy,dx);
	if (dy==0 && dx == 0) {
		if (DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y+dy][DATA.OBJ_DATA.hero.x+dx].get_has_item()) {
			//attempt to pick up item
			int index = get_item(DATA);
			int effect_location = DATA.OBJ_DATA.item_list[index].find_effect_index("quantity");
			if ( effect_location >= 0 ) {	// if is stackable
				for (int i = 0; i < DATA.OBJ_DATA.inventory.size(); i++) {	//check all inventory
					if ( DATA.OBJ_DATA.item_list[index].get_id() == DATA.OBJ_DATA.inventory[i].get_id() ) {	// for same item type
						int effect_location_inv = DATA.OBJ_DATA.inventory[i].find_effect_index("quantity");
						if ( DATA.OBJ_DATA.inventory[i].get_effect(effect_location_inv).strength < DATA.OBJ_DATA.inventory[i].get_effect(effect_location_inv).range ) {
							// if stackable item exists in inventory with room for extra stack
							int max = DATA.OBJ_DATA.inventory[i].get_effect(effect_location).range;
							int current = DATA.OBJ_DATA.inventory[i].get_effect(effect_location).strength;
							int amount = DATA.OBJ_DATA.item_list[index].get_effect(effect_location).strength;

							DATA.OBJ_DATA.inventory[i].get_effect(effect_location).add_strength(amount);
							if ( (current + amount) > max ) {
								//if more item than stack can contain, set to max and leave rest on floor
								
								DATA.OBJ_DATA.inventory[i].get_effect(effect_location).strength = max;
								DATA.OBJ_DATA.item_list[index].get_effect(effect_location).strength = (current+amount)-max;
								DATA.OBJ_DATA.inventory_weight = inventory_weight(DATA);
								return;
							}
							//DATA.add_message(DATA.OBJ_DATA.inventory[i].get_effect(effect_location).get_tag() + ": " + to_str(DATA.OBJ_DATA.inventory[i].get_effect(effect_location).strength) );
							DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y+dy][DATA.OBJ_DATA.hero.x+dx].set_has_item(false);	//remove item from tile
							DATA.OBJ_DATA.item_list.erase(DATA.OBJ_DATA.item_list.begin()+index);	//remove item from map
							DATA.OBJ_DATA.inventory_weight = inventory_weight(DATA);
							return;
						}
					}
				}
			}

			if (DATA.OBJ_DATA.inventory.size() >= DATA.OBJ_DATA.inventory_size)
				DATA.add_message(DATA.parser("Your inventory is full."));
			else {			
				DATA.OBJ_DATA.inventory.push_back( DATA.OBJ_DATA.item_list[index] );	//add item to inv
				DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y+dy][DATA.OBJ_DATA.hero.x+dx].set_has_item(false);	//remove item from tile
				DATA.OBJ_DATA.item_list.erase(DATA.OBJ_DATA.item_list.begin()+index);	//remove item from map
				DATA.OBJ_DATA.inventory_weight = inventory_weight(DATA);
			}
		}
		else CL_use_tile(DATA, dy, dx);
	}
	else {
		if (DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y+dy][DATA.OBJ_DATA.hero.x+dx].get_has_unit()) hero_attack(DATA, dy, dx);	//attack
		else if (DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y+dy][DATA.OBJ_DATA.hero.x+dx].get_is_trap()) ; //disarm trap
		else CL_use_tile(DATA, dy, dx);//use tile, door is possible
	}
  }

  void L_look_at(data& DATA, int y, int x) {			// needs more work
	// TO DO
	//	"You see a green dragon. It stands upon an ITEM."
	// ***^
	std::string temp = "You see a ";
	int y2, x2;
	int target = -1;
	if (DATA.MAP_DATA.map[y][x].get_has_unit()) {
		for (int i = 0; i < DATA.OBJ_DATA.monster_list.size(); i++) {
			DATA.OBJ_DATA.monster_list[i].get_location(y2, x2);
			if (y2 == y && x2 == x) {	target = i; break;	}
		}
		// if MONSTER 'target' is at location
		temp += DATA.OBJ_DATA.monster_list[target].get_name();
	}
	else {
		for (int i = 0; i < DATA.OBJ_DATA.item_list.size(); i++) {
			DATA.OBJ_DATA.item_list[i].get_location(y2, x2);
			if (y2 == y && x2 == x) {	target = i; break;	}
		}
		if (target != -1) {	// if ITEM 'target' is at location
			temp+= DATA.OBJ_DATA.item_list[target].get_name();
		}
		else {	// just ground
			temp+= DATA.MAP_DATA.map[y][x].get_char();
			if (DATA.MAP_DATA.map[y][x].get_indestructability()) temp+= " INde";
			else temp+= " dest";
		}
	}

	DATA.add_message(DATA.parser(temp));
	return;
  }

  void L_spell_casting_tooltip(data& DATA) {
		DATA.add_message(DATA.parser("Cast spell TOOL TIP."));
	return;
  }

  void L_cast_item(data& DATA, int y, int x) {
	DATA.selector.item = DATA.OBJ_DATA.spell_info.spell_id;
	bool item_breaks = false;

	int chance = -1;
	// check if item survives
	if (DATA.OBJ_DATA.inventory[DATA.OBJ_DATA.spell_info.spell_id].find_effect("fragile") )
		chance = DATA.OBJ_DATA.inventory[DATA.OBJ_DATA.spell_info.spell_id].find_effect_strength("fragile");
	if ( (rand()%101) <= chance)
		item_breaks = true;

	if ( item_breaks ) 
		drop_item(DATA, -2, x, 1);
	else
		drop_item(DATA, y, x, 1);
	DATA.OBJ_DATA.spell_info.spell_id= -1;
  }

  void L_cast_spell(data& DATA, int dy, int dx, bool location_target) {
	if ( DATA.OBJ_DATA.hero_data.cant_act ) { 	//if cant take action
		DATA.add_message(DATA.OBJ_DATA.hero_data.cant_act_message);	
		DATA.action_performed = true;
		return;
	}	
	if (location_target) {
		//if locational targeting: Get Y/X location of spell
		dy += DATA.MAP_DATA.map_y;
		dx += DATA.MAP_DATA.map_x;
		CL_cast_spell(DATA, 0, 0, dy, dx);
		//DATA.add_message("Locational cast NOW!");
	}
	else {
		//Directional Cast: trace line and find location that spell/item hits
		bool valid_location = true;
		int deltaY = dy;
		int deltaX = dx;
		int range = 2;
		int target_y, target_x;
		item * ability;
		ability = NULL;
		if (DATA.OBJ_DATA.spell_info.is_spell) ability = &(DATA.OBJ_DATA.spell_book[DATA.OBJ_DATA.spell_info.spell_id]);
		else	ability = &(DATA.OBJ_DATA.inventory[DATA.OBJ_DATA.spell_info.spell_id]);

										//set spell range
		if ( ability->find_effect("range") ) {
			range = ability->find_effect_strength("range");
		}
		else {
			range = 5 - ( (ability->get_weight()/10)- DATA.OBJ_DATA.hero.str[1] / 20) /5;	//!!! Throwing range formula
			if (range < 0 ) range = 0;
		}

		if (range <= 1 ) valid_location = false;
		if ( !DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y+deltaY+dy][DATA.OBJ_DATA.hero.x+deltaX+dx].get_passability() ) valid_location = false;
		if (DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y+deltaY][DATA.OBJ_DATA.hero.x+deltaX].get_has_unit() || !DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y+deltaY][DATA.OBJ_DATA.hero.x+deltaX].get_passability()) valid_location = false;
		while (valid_location) {	//travel item in line until hits unit, wall, or ground
			deltaY += dy;			
			deltaX += dx;
			range--;
			if(range <= 1) valid_location = false;
			if ( !DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y+deltaY+dy][DATA.OBJ_DATA.hero.x+deltaX+dx].get_passability() ) valid_location = false;
			if (DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y+deltaY][DATA.OBJ_DATA.hero.x+deltaX].get_has_unit() ) valid_location = false;
		}

		//If object is thrown/cast at a directly adjacent wall
		if ( !DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y+dy][DATA.OBJ_DATA.hero.x+dx].get_passability() ) {
			// run code at hero location
			target_y = DATA.OBJ_DATA.hero.y;
			target_x = DATA.OBJ_DATA.hero.x;
		}
		else {
			//else run code at target location
			target_y = DATA.OBJ_DATA.hero.y+deltaY;
			target_x = DATA.OBJ_DATA.hero.x+deltaX;
		}
		CL_cast_spell(DATA, dy, dx, target_y, target_x);
		if ( !DATA.OBJ_DATA.spell_info.is_spell )	//if is a thrown/use item
			L_cast_item(DATA, target_y, target_x);
	}
	//std::string temp = "dY = ";
	//temp += DATA.to_str(dy) + " dX= " + DATA.to_str(dx);
	//DATA.add_message(temp);
	DATA.action_performed = true;
  }

  bool L_can_see_monster(data& DATA) {
	int item_y, item_x;
	for (int i = 0; i < DATA.OBJ_DATA.monster_list.size(); i++) {
		DATA.OBJ_DATA.monster_list[i].get_location(item_y, item_x);
		if (LOS(DATA.OBJ_DATA.hero, item_y, item_x, DATA)) {			//if visible to hero
			if (!DATA.OBJ_DATA.monster_list[i].find_effect("invisible"))			
				return true;
		}
	}
	return false;
  }

  void reassign_stats(data& DATA) {
	unit *hero = &(DATA.OBJ_DATA.hero);
	hero->hp[1]+= DATA.OBJ_DATA.hero_data.armor_array[0];
	hero->str[1]+= DATA.OBJ_DATA.hero_data.armor_array[1];
	hero->def[1]+= DATA.OBJ_DATA.hero_data.armor_array[2];
	hero->wis[1]+= DATA.OBJ_DATA.hero_data.armor_array[3];
	hero->Int[1]+= DATA.OBJ_DATA.hero_data.armor_array[4];
	hero->vis[1]+= DATA.OBJ_DATA.hero_data.armor_array[5];
	hero->eva[1]+= DATA.OBJ_DATA.hero_data.armor_array[6];

	hero->fire_res[1]+= DATA.OBJ_DATA.hero_data.armor_array[7];
	hero->water_res[1]+= DATA.OBJ_DATA.hero_data.armor_array[8];
	hero->earth_res[1]+= DATA.OBJ_DATA.hero_data.armor_array[9];
	hero->air_res[1]+= DATA.OBJ_DATA.hero_data.armor_array[10];
  }

  void apply_equipment(data& DATA) {
	std::vector<effect> list;
	for (int i = 0; i < 7; i++) {	//for each piece of equipment
		if ( DATA.OBJ_DATA.equipment[i].get_id() != 0 ) {
			std::vector<effect> temp_list = DATA.OBJ_DATA.equipment[i].get_equ_effects();
			for (int j = 0; j < temp_list.size(); j++) 
				list.push_back(temp_list[j]);
		}
	}
	for (int j = 0; j < list.size(); j++) 
		DATA.OBJ_DATA.hero.add_effect(list[j]);
  }

  void L_armor_effects(data& DATA) {
	DATA.OBJ_DATA.hero.reset_stats();
	DATA.OBJ_DATA.hero.strip_armor_effects();
	apply_equipment(DATA);
	DATA.OBJ_DATA.hero.apply_equ_effects();
  }

// Family name and such
  void L_load_family(data& DATA, std::string path) {
	if (path == "") {
		std::vector<std::string> temp_name = tokens("data/bin/data.dat", "last_names");
		DATA.family_info.name = temp_name[rand()%temp_name.size()];

		DATA.family_info.age = rand() % 8 + 20;
		DATA.family_info.bloodline_age = DATA.family_info.age*3 + rand()%11;
		DATA.family_info.attempt = 1;
	}
	else {	//load info

	}
	return;
  }

  std::string L_to_upper(std::string in) {
	int i = 0;
	std::string temp = "";
	while (in[i]!=0) {
		if (in[i] == 'x' && in[i-1] == ' ') temp += in[i];
		else
			temp += toupper(in[i]);
		i++;
	}
	return temp;
  }

  bool is_vowel(char c) {
	switch (c) {
		case 'a':
		case 'e':
		case 'i':
		case 'o':
		case 'u':	return true;
		default: return false;
	}
	return false;
  }


