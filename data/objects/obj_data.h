//to do:
// 	IMPORTANT DESCISION: Does a valid but blocked command count as an action?
//	When hero completes an action, turn the action_completed flag on so monsters can process
#pragma once
#ifndef OBJ_DATA_H
#define OBJ_DATA_H

#include <vector>


#include "items/item.h"
#include "units/unit.h"
#include "effect.h"

#include "../../open.h"

class obj_data {

public:
	std::vector<item> item_list;
	std::vector<item> inventory;
	std::vector<item> spell_book;
	int inventory_size;
	int inventory_weight;
	std::vector<unit> monster_list;
	unit hero;					// temp hero
	struct hero_dat {
		bool immobilized;
		bool cant_act;
		std::string cant_act_message;
		int armor_array[11];
		unsigned int current_exp;
		unsigned int next_exp;
		int knowledge_threshold;
	} hero_data;

	struct cast_spell {
		 int spell_id;
		 int fallback_id;
		bool is_spell;
		bool fallback_is_spell;
	} spell_info;
	bool spell_line_cast;
	int spell_hash[10];	//***^ TO Implement

	item equipment[7];

	struct item_lookup {
		std::vector<std::string> adj;
		std::vector<std::string> table;	
	} item_lookup_table;



  obj_data() {

	hero_data.immobilized = false;
	hero_data.cant_act = false;
	hero_data.cant_act_message = "You cannot take action now.";
	hero_data.current_exp = 10;
	hero_data.next_exp = 20;
	hero_data.knowledge_threshold = 200;

	inventory_size = 20;
	inventory_weight = 0;
	//Define Quick Cast initialization
	spell_info.spell_id = -1;
	spell_info.fallback_id = 0;
	spell_info.is_spell = false;
	spell_info.fallback_is_spell = true;
	spell_line_cast = false;

	for (int i = 0; i < 10; i++)
		spell_hash[i] = -1;
	
	equipment[0] = item();	//head
	equipment[1] = item();	//neck
	equipment[2] = item();	//chest
	equipment[3] = item("1");	//ring 1
	equipment[4] = item("3");	//R.Hand
	equipment[5] = item();	//ring 2
	equipment[6] = item();	//L.Hand

	//temperary hero		needs to load stored hero, otherwise default to new
	hero = unit("HERO",5,8,50);
	
		//hero.add_effect(effect(100, "1337", 50, 0, -1));
		//hero.add_effect(effect(100, "dyslexic", 50, 0, -1));
		hero.add_effect(effect(100, "illiterate", 100, 0, -1));
		hero.add_effect(effect(100, "hard_hearing", 20, 0, -1));
		//hero.add_effect(effect(100, "rose_world", 50, 0, -1));

		hero.add_effect(effect(110, "fire_attack", 10, 0, 11));
		hero.add_effect(effect(110, "water_attack", 2, 0, -1));

		hero.add_effect(effect(100, "wall_walking", 10, 0, 11));

		inventory.push_back(item("1"));

		std::vector<effect> list;
		inventory.push_back(item("3"));

		for (int i = 0; i < 7; i++) {
			inventory.push_back(item("2"));
			inventory.push_back(item("4"));
		}	
/*		inventory.push_back(item("3"));
		inventory.push_back(item("3"));	inventory.push_back(item("3"));	inventory.push_back(item("3"));
		inventory.push_back(item("4"));
	*/	inventory.push_back(item("5"));	inventory.push_back(item("5"));
		inventory.push_back(item("6"));	inventory.push_back(item("6"));
/*		***^ to delete
		for (int i = 0; i < 2; i++) {
			std::vector<effect> list2;
			list2.push_back(effect(10, "enchanted", -10, 0, -1) );
			inventory.push_back(item(3, "Cursed Potion", "Test item", 'p', 100, 5, 6, 15, list2));
		}
		for (int i = 0; i < 2; i++) {
			std::vector<effect> list2;
			list2.push_back(effect(10, "enchanted", 10, 0, -1) );
			inventory.push_back(item(4, "Blessed Potion", "Test item", 'p', 100, 5, 6, 15, list2));
		}
		for (int i = 0; i < 2; i++) {
			std::vector<effect> list2;
			list2.push_back(effect(10, "enchanted", -10, 0, -1) );
			inventory.push_back(item(5, "Scroll of Bad", "Test item", 's', 100, 5, 6, 15, list2));
		}
		inventory.push_back(item(6, "Iron Weight", "A massive hunk of metal that really doesn't weigh much.", 'a', 511, 96, 6, 15));
*/
		spell_book.push_back(item(1, "FireBolt", "A massive hunk of fire that really burns.", 'f', 511, 96, 0, 0));
																		//   type  $$$ lbs  y  x
												// for magic: type, power, ?, god, index
		//***^ work in progress... Need to hash the randomly aquired spell_book to the corrisponding magics
		int y = 0;
		int x = 0;
		spell_book.back().get_location(y,x);
		spell_hash[0] = y*8+x;

	item_lookup_table.adj = tokens("data/bin/data.dat", "adj");
	item_lookup_table.table.push_back("NULL");	//Table[0] is null
	setup_table();
  }

  ~obj_data() {
	//Clean up data usage / save?
  }

	void setup_table() {
		srand (time(NULL));
		//create bool arrays matching each name vector and set them to false
		bool used[item_lookup_table.adj.size()];
		for (int i = 0; i < item_lookup_table.adj.size(); i++) used[i] = false;
		//populate look up table based on item type, set used item names to true

		for (int i = 0; i < 7; i++) {
			if ( true ) {		//Gab item from creation list (1-lsit end) then assign name based on type
				int pick = rand()%item_lookup_table.adj.size();
				while ( used[pick] ) {	// find un-used name			BROKEN....
					pick = (pick+1)%item_lookup_table.adj.size();
				}
				item_lookup_table.table.push_back( item_lookup_table.adj[pick] );
				used[pick] = true;
			}
		}
	}

  std::string to_str (int in) {std::ostringstream stream;	stream << in;	return stream.str();	} 

  std::string item_name(item name) {
	std::string temp = name.get_name();
	//***^
	//temp = "|c5"+temp;
	//****************
	if (name.get_id() == 0) return name.get_name();
	if (item_lookup_table.table[name.get_id()] == "known") name.is_known(true); 
	if ( name.is_known()  ) {
		if (  name.is_cursed() ) temp = "|c1"+temp;

		if (  name.find_effect("enchanted") ) {
			int power = name.find_effect_strength("enchanted");
			if ( power > 0 ) {
				temp += "|c4+" + to_str(power) +"|c0";	//4
			}
			else {
				temp += "|c2" + to_str(power) + "|c0";	//6
			}
		}
	}
	else {
		if ( name.get_type() == 't' || name.get_type() == 'a' || name.get_type() == 'w') {
			temp = "?-" + name.get_name();
		}
		else {
			if ( name.get_type_name() == "scroll" )
				temp = name.get_type_name()+" of "+item_lookup_table.table[name.get_id()];
			else
				temp = item_lookup_table.table[name.get_id()] + " " + name.get_type_name();
		}
	}
	//****************

	//Item with quantity
	int effect_location = name.find_effect_index("quantity");
	if (effect_location >= 0 ) {
		int number = name.get_effect(effect_location).strength;
		if (number > 1)
			temp += " x"+to_str(number);
	}
	temp += "|c0";
	return temp;	
  }

  std::string find_description (item name) {
	if ( name.is_known() ) {
		std::string desc = name.get_description();
		desc += "|B";
		desc += name.effect_desc();
		// construct sting and effect list
		return desc;
	}
	if ( name.get_type() == 'p' ) return ( "An unknown " + item_lookup_table.table[name.get_id()] + " Potion."  );
	if ( name.get_type() == 's' ) return ( "An unknown Scroll of " + item_lookup_table.table[name.get_id()] + "."  );
	return name.get_description();
  }
	
  void identify(item& ITEM) {
	ITEM.is_known(true);
	if ( ITEM.shared_name() ) item_lookup_table.table[ITEM.get_id()]="known";//call on use/inspection
  }

  void identify_of_type(item& ITEM) {
	ITEM.is_known(true);
	if ( !ITEM.is_equipment() ) item_lookup_table.table[ITEM.get_id()]="known";//call on use/inspection
  }

};
#endif
