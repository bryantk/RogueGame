#include <iostream>
#include <fstream>

#include "ncurses_helper.h"

#include "data/data.h"
#include "data/map_data/map_data.h"
#include "data/objects/obj_data.h"

#include "data/objects/items/item.h"
#include "data/objects/units/unit.h"

#include "logic/drawing/map_drawing.h"
#include "logic/logic.h"
#include "logic/creation_logic.h"

#include "logic/units/monster_logic.h"

#include "temp.h"

#include <vector>


#include <string>
#include <sstream>

#include <iostream>

//map area:  20 =y  50 =x

// TO DO / BUGS
/*
	X: DONE		=: In progress		-: to do	

	X droping items sometimes does not acctualy run the drop_item() logic
	X sometimes (when movinf and clicking fast) picking up stackable items does not stack them when they should.
	= Need to log events to log file better.	Working on it
	- Implement "Equip" command
	-	X	Use navigate_equipment() to filter out impossible choices  i.e. no equiping heagear on arms.
	-	Equip said item
	-	when 2Handed, compare stat change of new vs. left AND right hands
	-	Apply equipment stats to hero. in MAIN and INV_DRAW
	- code USE command functionality
*/

using namespace std;

  void resize_terminal(int x, int y) {
	ostringstream stream;
	string resize = "resize -s ";				//reset terminal to original size
	stream << y;	resize.append(stream.str());
	resize+=" ";
	stream.str("");	stream << x;	resize.append(stream.str());
	system(resize.c_str());
 	system("clear");
  }

  string to_string (int in) {ostringstream stream;	stream << in;	return stream.str();	} 

  void up_down_input(data& DATA, int& value);
  void command_selection(data& DATA, int& value, bool& equipment);
  void cycle_value(int& value, item& ITEM, bool increasing );
  void navigate_equipment(data& DATA, int& value);
  void first_selection(data& DATA, int& value, bool& equipment);

int main ()
{	
	int terminal_original_x, terminal_original_y =6;
	getmax(terminal_original_x, terminal_original_y);		//save terminal's original size
	if (initialize()==1) return 1;							//start up nCurses

	int dx, dy;
	data DATA;
		DATA.log_file("--Game started.\n");
	//temp testing
		srand (time(NULL));
	DATA.parser_random_seed = rand()%1000;
		TEMP_load_map(DATA.MAP_DATA);

		spawn_item(DATA, item("2"), 5, 7, 5);
		spawn_item(DATA, item("2"), 5, 9, 5);
		spawn_item(DATA, item("2"), 5, 10);

		spawn_monster(DATA, 1, 4,15);
		spawn_monster(DATA, 2, 4,17);


/*	tokens("data/bin/data.dat", "file1");
		vector<string> meh = tokens("data/bin/data2.dat", ("item1") );
			//tokens("data/bin/data2.dat", ("item"+tag) );
		string teh = "";
		for (int i = 0; i < meh.size(); i++) {
			teh += meh[i];
			teh += " / ";
		}
		DATA.add_message(teh);

		DATA.add_message(DATA.parser("TEST item is NOT 1337 the right one, loser."));
	*/
		L_load_family(DATA, "");
		DATA.log_file("	DATA initialized.\n");
	//DATA.add_message("You descend into the depths of the dungeon.");
//################################################################################
//         GAME LOOP
//################################################################################
	while (DATA.running) {

		if (!DATA.force_update) {	//skip input and processing to force update
			
			DATA.input = get_input();								//Poll for user input
	NC_clear();
	// temp ***^
	if (DATA.input == 'q') {DATA.running = false; DATA.input = 0;}
	if (DATA.input == 'n') {DATA.state = static_cast<Game_State>((DATA.state+1)%8);}
	if (DATA.input == 'i') {
		DATA.debug[0] = true;
		if ( DATA.state == DEBUG)	DATA.state = GAME;
		else DATA.state = DEBUG;
	}
	//***^ end temp

			//*********************************************************
			if (DATA.state == GAME) {	//	GAME STATE	input
										//---------------------
				if (DATA.is_direction()) {
					if(DATA.looking || DATA.targeting) {
						if (DATA.selecting_direction) {
							get_direction(DATA, dy, dx);
							DATA.flags_off();
							L_cast_spell(DATA, dy, dx, false);
						}
						else {
							L_looking(DATA, dy, dx);
						}
					}
					else if (DATA.selecting_direction) {
						if (DATA.going_to_use) {
							L_use(DATA);
							DATA.flags_off();
						}
						else if (DATA.alt_selection) {
							get_direction(DATA, dy, dx);
							DATA.automate_movement = true;
							if (dy == 0 && dx == 0) {
								DATA.flags_off();
								DATA.automate_movement = false;
							}
							if (!step_hero(DATA, dy, dx) ) { 
								DATA.automate_movement = false; 
								DATA.flags_off();}	
						}
					}
					else  {
					move_hero(DATA);
					if (DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y][DATA.OBJ_DATA.hero.x].get_has_item()) {		//alert player if on item
						string alert = "";	
						alert += L_to_upper( DATA.OBJ_DATA.item_name(DATA.OBJ_DATA.item_list[get_item(DATA)]) );	
						int index = DATA.OBJ_DATA.item_list[get_item(DATA)].find_effect_index("quantity");
						if (index != -1 && DATA.OBJ_DATA.item_list[get_item(DATA)].get_effect(index).strength != 1) 
							alert = alert + " lay upon the floor.";
						else {
							if (is_vowel(alert[0])) alert = "An " + alert;
							else alert = "A " + alert;
							alert += " lays upon the floor.";
						}
						DATA.add_message(DATA.parser(alert));

					}
					DATA.action_performed = true;
					}
				}
				else if (DATA.is_confirm()) {
					if (DATA.going_to_use) {
							L_use(DATA);
							DATA.flags_off();
						}
					else if (DATA.looking || DATA.targeting) {
						if (DATA.looking) 
							L_look_at(DATA, dy+DATA.MAP_DATA.map_y, dx+DATA.MAP_DATA.map_x);
						else {
							if (DATA.OBJ_DATA.spell_line_cast)
								L_cast_spell(DATA, 0, 0, false);
							else
								L_cast_spell(DATA, dy, dx, true);
//***^
					//		DATA.add_message("Location cast NOW!");
						}
						DATA.looking = false;
						DATA.targeting = false;
						DATA.flags_off();
					}
					else {	// more else ifs   if there is more states
						DATA.add_message(DATA.parser("Use what?"));
						dx = DATA.OBJ_DATA.hero.x-DATA.MAP_DATA.map_x;	dy =DATA.OBJ_DATA.hero.y-DATA.MAP_DATA.map_y;
						DATA.going_to_use = true;
						DATA.selecting_direction = true;
					}
				}
				else if (DATA.is_cancel()) {	// if cancel key pressed

					if (DATA.looking) {
						DATA.flags_off();
					}
					else if (DATA.targeting) {
						DATA.flags_off();
						DATA.OBJ_DATA.spell_info.spell_id= -1;
						//DATA.action_performed = true;
					}
					else if (DATA.going_to_use) {
						DATA.flags_off();
					}
					else if (DATA.selecting_direction) {
						DATA.flags_off();
					}
					else {
						DATA.flags_off();
						DATA.clear_selection();

						DATA.state = DATA.saved_menu_state;
					}
				}
				else if (DATA.is_alt1()) {
					if (!DATA.looking && !DATA.targeting && !DATA.selecting_direction && !DATA.going_to_use && !DATA.alt_selection) {
						DATA.flags_off();
						DATA.looking = true;
						DATA.add_message(DATA.parser("Look where?"));
						dx = DATA.OBJ_DATA.hero.x-DATA.MAP_DATA.map_x;	dy =DATA.OBJ_DATA.hero.y-DATA.MAP_DATA.map_y;
					}
					if (DATA.looking) dx = DATA.OBJ_DATA.hero.x-DATA.MAP_DATA.map_x;	dy =DATA.OBJ_DATA.hero.y-DATA.MAP_DATA.map_y;
				}
				else if (DATA.is_alt2()) {	//cast spell
					if (!DATA.looking && !DATA.targeting && !DATA.selecting_direction && !DATA.going_to_use && !DATA.alt_selection) {
			// TO DO *********************
						DATA.flags_off();
						if (DATA.OBJ_DATA.spell_info.fallback_id >= 0 && DATA.OBJ_DATA.spell_info.spell_id < 0) {
							DATA.OBJ_DATA.spell_info.spell_id = DATA.OBJ_DATA.spell_info.fallback_id;
							DATA.OBJ_DATA.spell_info.is_spell = DATA.OBJ_DATA.spell_info.fallback_is_spell;
						}
						if (DATA.OBJ_DATA.spell_info.spell_id >= 0 ) {
			L_spell_casting_tooltip(DATA);	//*********
							DATA.targeting = true;
							dy = 0;
							dx = 0;
							dx = DATA.OBJ_DATA.hero.x-DATA.MAP_DATA.map_x;	dy =DATA.OBJ_DATA.hero.y-DATA.MAP_DATA.map_y;
							if (DATA.OBJ_DATA.spell_line_cast) DATA.selecting_direction = true;
						}
						else {
							DATA.add_message(DATA.parser("No Quick Cast set."));
						}
					}
				}
				else if (DATA.is_scroll_up()) {
					DATA.scoll_messages(true);
				}
				else if (DATA.is_scroll_down()) {
					DATA.scoll_messages(false);
				}
				else if (DATA.is_alternate()) {
					if (DATA.alt_selection)
						DATA.flags_off();
					else {
						DATA.flags_off();
						DATA.alt_selection = true;
	DATA.selecting_direction = true;
						DATA.add_message(DATA.parser("Run where?"));
					}
				}
			}
			//*********************************************************
			else if (DATA.state == INVENTORY) {	//	INVENTORY	input
												//---------------------
				if ( DATA.is_direction() ) {
					if ( DATA.selector.T_F_bool_0 ) DATA.selector.T_F_bool_0 = false;
					else if ( DATA.selector.bool_1 ) {
						switch ( DATA.selector.selector_0 ) {
							case 1:	// EQUIP command
									navigate_equipment(DATA, DATA.selector.selector_1);
								break;
							case 4:	// DROP command
									up_down_input(DATA, DATA.selector.selector_1);
								break;
						}
					}
					else if (  DATA.selector.bool_0 ) {	// after first selection		i.e. Command selection
						command_selection(DATA, DATA.selector.selector_0, DATA.selector.is_equipment);
					}
					else	//clean menu choices
						first_selection(DATA, DATA.selector.item, DATA.selector.is_equipment);

				}
				//=======================
				//	' c ' C O N F I R M
				//=======================
				else if ( DATA.is_confirm() ) {
					if ( DATA.selector.T_F_bool_0 ) DATA.selector.T_F_bool_0 = false;
					else if ( DATA.selector.bool_1 ) {
						switch ( DATA.selector.selector_0 ) {
							case 1:	// EQUIP command
								if ( CL_equip_item(DATA) ) {
									// if succesfuly added equipment, alter stats	then re-set flags
									reassign_stats(DATA); // working ?????? ***^
									DATA.selector.selector_1 = 1;
									DATA.selector.bool_0 = false;
									DATA.selector.bool_1 = false;
								}	
								break;
							case 4:	// DROP command
								DATA.log_file("	in INV, attempting to drop " + to_str(DATA.selector.selector_1) + " items\n");
								drop_item(DATA, -1, -1, DATA.selector.selector_1);
								DATA.selector.selector_1 = 1;
								break;
						}
					}
					else if ( DATA.selector.bool_0 ) {		// after first selection		i.e. Command selection
						switch ( DATA.selector.selector_0 ) {
						case 0:	// USE command

								break;
						case 1:	// EQUIP command
								if ( !DATA.OBJ_DATA.inventory[DATA.selector.item].is_equipment() )
									break;
								DATA.selector.selector_1 = 0;
								cycle_value(DATA.selector.selector_1, DATA.OBJ_DATA.inventory[DATA.selector.item], true );
								DATA.selector.bool_1 = true;	// needs further input
								break;
						case 2:	// SET Q.USE command
								//Set item as quick throw/use
								DATA.OBJ_DATA.spell_info.spell_id = DATA.selector.item;
								DATA.OBJ_DATA.spell_info.is_spell = false;
								DATA.OBJ_DATA.spell_line_cast = true;
								if ( DATA.OBJ_DATA.inventory[DATA.selector.item].find_effect("cast_target") ) DATA.OBJ_DATA.spell_line_cast = false;

								DATA.OBJ_DATA.spell_info.fallback_id = DATA.selector.item;
								DATA.OBJ_DATA.spell_info.fallback_is_spell = false;
								break;
						case 3:	// THROW command
								DATA.OBJ_DATA.spell_info.spell_id = DATA.selector.item;
								DATA.OBJ_DATA.spell_info.is_spell = false;
								DATA.OBJ_DATA.spell_line_cast = true;
								if ( DATA.OBJ_DATA.inventory[DATA.selector.item].find_effect("cast_target") ) DATA.OBJ_DATA.spell_line_cast = false;

								DATA.targeting = true;
								dy = 0;
								dx = 0;
								dx = DATA.OBJ_DATA.hero.x-DATA.MAP_DATA.map_x;	dy =DATA.OBJ_DATA.hero.y-DATA.MAP_DATA.map_y;
								if (DATA.OBJ_DATA.spell_line_cast) DATA.selecting_direction = true;
								DATA.state = GAME;
								DATA.selector.bool_0 = false;
							//	DATA.selector.bool_0_2 = false;
								break;
						case 4:	// DROP command			//***^ works?
								if ( DATA.selector.is_equipment ) {
									if ( DATA.OBJ_DATA.equipment[DATA.selector.item].is_cursed() ) {			//Attempting to remove cursed item
										DATA.selector.T_F_bool_0 = true;	// needs confirmation/error message
										DATA.selector.display_message = " Can not remove CURSED equipment. ";
										break;
									}
									if ( DATA.selector.item == 4 &&  DATA.OBJ_DATA.equipment[4].is_both() ) 
										DATA.OBJ_DATA.equipment[6].set_name("None"); 
									DATA.OBJ_DATA.inventory.push_back(DATA.OBJ_DATA.equipment[DATA.selector.item]);	//add to end of inventory
									DATA.OBJ_DATA.equipment[DATA.selector.item] = item();						//replace with null
									int temp = DATA.selector.item;
									DATA.selector.item = DATA.OBJ_DATA.inventory.size()-1;
									drop_item(DATA, -1, -1, 1);
									DATA.selector.item = temp;
								}
								else
									DATA.selector.bool_1 = true;	// needs further input
								break;
						case 5:	// REMOVE command
								if ( DATA.OBJ_DATA.inventory.size() >= DATA.OBJ_DATA.inventory_size ) {	//no room in inventory
									DATA.selector.T_F_bool_0 = true;	// needs confirmation/error message
									DATA.selector.display_message = " No room in inventory. ";
									break;
								}
								if ( DATA.OBJ_DATA.equipment[DATA.selector.item].is_cursed() ) {			//Attempting to remove cursed item
									DATA.selector.T_F_bool_0 = true;	// needs confirmation/error message
									DATA.selector.display_message = " Can not remove CURSED equipment. ";
									break;
								}
								if ( DATA.selector.item == 4 && DATA.OBJ_DATA.equipment[4].is_both() ) 
									DATA.OBJ_DATA.equipment[6].set_name("None"); 
								DATA.OBJ_DATA.inventory.push_back(DATA.OBJ_DATA.equipment[DATA.selector.item]);	//add to end of inventory
								DATA.OBJ_DATA.equipment[DATA.selector.item] = item();						//replace with null
								break;
						}						
					}
					else {		//if presented a clean menu
						if ( (DATA.selector.is_equipment && DATA.OBJ_DATA.equipment[DATA.selector.item].get_id()!= 0) || (!DATA.selector.is_equipment && DATA.OBJ_DATA.inventory.size() > 0 ) ) {
							DATA.selector.bool_0 = true;
							DATA.selector.selector_0 = 0;
							if ( DATA.selector.is_equipment ) DATA.selector.selector_0 = 4;
						}
					}
				}
				//=====================
				//	' x ' C A N C E L
				//=====================
				else if ( DATA.is_cancel() ) {
					if ( DATA.selector.T_F_bool_0 ) DATA.selector.T_F_bool_0 = false;
					else if ( DATA.selector.bool_1 ) {
						DATA.selector.bool_1 = false;
						DATA.selector.selector_1 = 1;
					}
					else if (  DATA.selector.bool_0 ) {	// after first selection		i.e. Command selection
						DATA.selector.bool_0 = false;
					}
					else			//if presented a clean menu
						DATA.state = GAME;
				}
				else if ( DATA.is_scroll_up() ) {
					if ( DATA.selector.T_F_bool_0 ) DATA.selector.T_F_bool_0 = false;
					else if ( DATA.selector.bool_1 ) {
						
					}
					else if (  DATA.selector.bool_0 ) {	// after first selection		i.e. Command selection
						DATA.selector.item++;
						first_selection(DATA, DATA.selector.item, DATA.selector.is_equipment);
					}
					else	{		//if presented a clean menu
						//cancel out old flags / dis-allow if flags set
						DATA.state = MAGIC;
						DATA.clear_selection();
					}
				}
				else if ( DATA.is_scroll_down() ) {
					if ( DATA.selector.T_F_bool_0 ) DATA.selector.T_F_bool_0 = false;
					else if ( DATA.selector.bool_1 ) {
						
					}
					else if (  DATA.selector.bool_0 ) {	// after first selection		i.e. Command selection
						DATA.selector.item--;
						first_selection(DATA, DATA.selector.item, DATA.selector.is_equipment);
					}
					else	{		//if presented a clean menu
						//cancel out old flags / dis-allow if flags set
						DATA.state = OPTIONS;
						DATA.clear_selection();
					}
				}
//=======================================================================================================
// CUT ME
//=======================================================================================================
	/*		//temp
		if (DATA.input == 'o') {
			DATA.OBJ_DATA.identify(DATA.OBJ_DATA.inventory[DATA.selector.item]);	//.is_known(true);
			//if ( !DATA.OBJ_DATA.inventory[DATA.selector.item].is_equipment() )DATA.OBJ_DATA.item_lookup_table.table[DATA.OBJ_DATA.inventory[DATA.selector.item].get_id()]="known";//call on use/inspection
		}
			//end temp
				if (DATA.is_direction()) {
					if (!DATA.selector.bool_0) {
						if (DATA.is_up() ) DATA.selector.item-=2;
						if (DATA.is_down() ) DATA.selector.item +=2;
						if (DATA.is_right() ) DATA.selector.item++; //(DATA.selector.item+1)%DATA.OBJ_DATA.inventory.size();
						if (DATA.is_left() ) DATA.selector.item--;

						if (DATA.selector.item < 0) DATA.selector.item = DATA.OBJ_DATA.inventory.size()-1;
						if ( DATA.selector.item >= DATA.OBJ_DATA.inventory.size() )	DATA.selector.item = 0;
					}
					else if (!DATA.selector.bool_0_2) {
						if (DATA.is_right() ) DATA.selector.one_time = (DATA.selector.one_time+1)%5;
						if (DATA.is_left() ) DATA.selector.one_time--;
						if (DATA.selector.one_time < 0 ) DATA.selector.one_time = 4;
					}
					else {
						if (DATA.selector.one_time == 1) {	// equip item selector
							int delta = 0;
							if (DATA.is_up() ) {
								DATA.selector.equipment--;
								if (DATA.selector.equipment < 0) DATA.selector.equipment = 6;
								while (true) {
									if ( DATA.selector.equipment == 0 && DATA.OBJ_DATA.inventory[DATA.selector.item].is_head() ) break;
									if ( DATA.selector.equipment == 1 && DATA.OBJ_DATA.inventory[DATA.selector.item].is_chest() ) break;
									if ( DATA.selector.equipment == 2 && DATA.OBJ_DATA.inventory[DATA.selector.item].is_arm() ) break;
									if ( DATA.selector.equipment == 3 && DATA.OBJ_DATA.inventory[DATA.selector.item].is_arm() ) break;
									if ( DATA.selector.equipment == 4 && DATA.OBJ_DATA.inventory[DATA.selector.item].is_neck() ) break;
									if ( DATA.selector.equipment == 5 && DATA.OBJ_DATA.inventory[DATA.selector.item].is_ring() ) break;
									if ( DATA.selector.equipment == 6 && DATA.OBJ_DATA.inventory[DATA.selector.item].is_ring() ) break;
		//	if ( DATA.selector.equipment == 7 && DATA.OBJ_DATA.inventory[DATA.selector.item].is_both() ) break;
									DATA.selector.equipment--;
									if (DATA.selector.equipment < 0) DATA.selector.equipment = 6;
								}
							}
							if (DATA.is_down() ) {
								DATA.selector.equipment++;
								if (DATA.selector.equipment > 6) DATA.selector.equipment = 0;
								while (true) {
									if ( DATA.selector.equipment == 0 && DATA.OBJ_DATA.inventory[DATA.selector.item].is_head() ) break;
									if ( DATA.selector.equipment == 1 && DATA.OBJ_DATA.inventory[DATA.selector.item].is_chest() ) break;
									if ( DATA.selector.equipment == 2 && DATA.OBJ_DATA.inventory[DATA.selector.item].is_arm() ) break;
									if ( DATA.selector.equipment == 3 && DATA.OBJ_DATA.inventory[DATA.selector.item].is_arm() ) break;
									if ( DATA.selector.equipment == 4 && DATA.OBJ_DATA.inventory[DATA.selector.item].is_neck() ) break;
									if ( DATA.selector.equipment == 5 && DATA.OBJ_DATA.inventory[DATA.selector.item].is_ring() ) break;
									if ( DATA.selector.equipment == 6 && DATA.OBJ_DATA.inventory[DATA.selector.item].is_ring() ) break;
		//	if ( DATA.selector.equipment == 7 && DATA.OBJ_DATA.inventory[DATA.selector.item].is_both() ) break;
									DATA.selector.equipment++;
									if (DATA.selector.equipment > 6) DATA.selector.equipment = 0;
								}
							}

						}
						else if (DATA.selector.one_time == 2) {		// remove equipment selector
							if (DATA.is_up() ) DATA.selector.equipment--;
							if (DATA.is_down() ) DATA.selector.equipment = (DATA.selector.equipment+1)%7;
							if (DATA.is_right() && DATA.selector.equipment < 3) DATA.selector.equipment = (DATA.selector.equipment+4)%7;
							if (DATA.is_left() && DATA.selector.equipment > 3) DATA.selector.equipment = (DATA.selector.equipment-4);
							if (DATA.selector.equipment < 0) DATA.selector.equipment = 6;
						}
						else if (DATA.selector.one_time != 4)	{ 
							if (DATA.selector.T_F_bool_0)	DATA.selector.T_F_bool_0 = false;
							else DATA.selector.T_F_bool_0 = true;
						}
						else	{
							if (DATA.is_up() ) DATA.selector.quantity++;
							if (DATA.is_down() ) DATA.selector.quantity--;
						}
					}
				}
				else if ( DATA.is_confirm() ) {
					if (DATA.OBJ_DATA.inventory.size() == 0) {
						DATA.selector.bool_0 = false;
						DATA.selector.bool_0_2 = false;
					}
					else if (!DATA.selector.bool_0)	
						DATA.selector.bool_0 = true;
					else if (DATA.selector.bool_0_2) {
						 if (!DATA.selector.T_F_bool_0) {
							if (DATA.selector.one_time == 3) {
								//Set item as quick throw/use
								DATA.OBJ_DATA.spell_info.spell_id = DATA.selector.item;
								DATA.OBJ_DATA.spell_info.is_spell = false;
								DATA.OBJ_DATA.spell_line_cast = true;
								if ( DATA.OBJ_DATA.inventory[DATA.selector.item].find_effect("cast_target") ) DATA.OBJ_DATA.spell_line_cast = false;

								DATA.OBJ_DATA.spell_info.fallback_id = DATA.selector.item;
								DATA.OBJ_DATA.spell_info.fallback_is_spell = false;
							}
							DATA.selector.bool_0_2 = false;
						}
						else {
							if (DATA.selector.one_time == 0) {
								DATA.add_message(" USE ");

								DATA.state = GAME;
								DATA.selector.bool_0 = false;
								DATA.selector.bool_0_2 = false;
	DATA.action_performed = true; // needed here?
							}
							else if (DATA.selector.one_time == 1) {		//equip
								if ( !DATA.OBJ_DATA.equipment[DATA.selector.equipment].is_cursed() ) 	{
									CL_equip_item(DATA);
									reassign_stats(DATA);
									DATA.selector.bool_0 = false;
									DATA.selector.bool_0_2 = false;
									DATA.selector.equip = false;
								}		
							}
							else if (DATA.selector.one_time == 2) {		//remove
								if (DATA.OBJ_DATA.equipment[DATA.selector.equipment].get_id() != 0 ) {	//if not empty
									if ( !DATA.OBJ_DATA.equipment[DATA.selector.equipment].is_cursed() ) {
										if ( DATA.OBJ_DATA.inventory.size() < DATA.OBJ_DATA.inventory_size ) {
											DATA.OBJ_DATA.inventory.push_back(DATA.OBJ_DATA.equipment[DATA.selector.equipment]);
											DATA.OBJ_DATA.equipment[DATA.selector.equipment] = item();
											reassign_stats(DATA);
										}
									}
								}
								//DATA.add_message(" REMOVE ");
							}
							else if (DATA.selector.one_time == 3) {
								//DATA.add_message(" THROW ");
								DATA.OBJ_DATA.spell_info.spell_id = DATA.selector.item;
								DATA.OBJ_DATA.spell_info.is_spell = false;
								DATA.OBJ_DATA.spell_line_cast = true;
								if ( DATA.OBJ_DATA.inventory[DATA.selector.item].find_effect("cast_target") ) DATA.OBJ_DATA.spell_line_cast = false;

								DATA.targeting = true;
								dy = 0;
								dx = 0;
								dx = DATA.OBJ_DATA.hero.x-DATA.MAP_DATA.map_x;	dy =DATA.OBJ_DATA.hero.y-DATA.MAP_DATA.map_y;
								if (DATA.OBJ_DATA.spell_line_cast) DATA.selecting_direction = true;
								DATA.state = GAME;
								DATA.selector.bool_0 = false;
								DATA.selector.bool_0_2 = false;

								

							}
							else {
								drop_item(DATA, -1, -1);
								DATA.selector.quantity = 1;
							}
						}
					}
					else {
						DATA.selector.bool_0_2 = true;
						DATA.selector.T_F_bool_0 = true;
						if (DATA.selector.one_time == 1 ) {
							if ( DATA.OBJ_DATA.inventory[DATA.selector.item].is_equipment() ) {
								DATA.selector.equip = true;	//***^
								if ( DATA.OBJ_DATA.inventory[DATA.selector.item].is_head() ) DATA.selector.equipment = 0;
								if ( DATA.OBJ_DATA.inventory[DATA.selector.item].is_chest() ) DATA.selector.equipment = 1;
								if ( DATA.OBJ_DATA.inventory[DATA.selector.item].is_arm() ) DATA.selector.equipment = 2;
								if ( DATA.OBJ_DATA.inventory[DATA.selector.item].is_neck() ) DATA.selector.equipment = 4;
								if ( DATA.OBJ_DATA.inventory[DATA.selector.item].is_ring() ) DATA.selector.equipment = 5;
							}
							else {
								DATA.selector.bool_0_2 = false;
								DATA.selector.T_F_bool_0 = false;
							}
						}
						if (DATA.selector.one_time == 2) DATA.selector.remove = true;	//***^
					}
				}
				else if ( DATA.is_cancel() ) {
					if (!DATA.selector.bool_0)	
						DATA.state = GAME;
					else if (DATA.selector.bool_0_2) {
						DATA.selector.bool_0_2 = false;
						DATA.selector.equip = false;
						DATA.selector.remove = false;
					}
					else {
						DATA.selector.bool_0 = false;
						DATA.selector.one_time = 0;
					}
				}
				else if (DATA.is_alt1()) {
				
				}
				else if (DATA.is_alt2()) {
				
				}
				else if ( DATA.is_scroll_up() ) {
					//cancel out old flags / dis-allow if flags set
					DATA.state = MAGIC;
					DATA.selector.one_time = 0;
					DATA.selector.bool_0 = false;
					DATA.selector.T_F_bool_0 = true;
				}
				else if ( DATA.is_scroll_down() ) {
					//cancel out old flags / dis-allow if flags set
					DATA.state = OPTIONS;
					DATA.selector.one_time = 0;
					DATA.selector.bool_0 = false;
					DATA.selector.T_F_bool_0 = true;
				}



			}
	*/
//=======================================================================================================
// CUT ME			^^^
//=======================================================================================================
			}
			//*********************************************************
			else if (DATA.state == OPTIONS) {	//	OPTIONS	input
												//---------------------
				if (DATA.is_direction()) {
					if (!DATA.selector.bool_0) {	// nothing selected
						if (DATA.is_up()) 	{
							DATA.selector.item--;
							if (DATA.selector.item < 0) DATA.selector.item = 4;
						}
						if (DATA.is_down()) 	DATA.selector.item = (DATA.selector.item+1)%5;
					}
					else {				// something selected
						if (DATA.selector.item == 4) {			// Y/N save and quit
							if (DATA.is_left())  DATA.selector.T_F_bool_0 = false;
							if (DATA.is_right())  DATA.selector.T_F_bool_0 = true;
						}
						else if (DATA.selector.item == 1 ) {		//key config

						}
					}
				}
				else if (DATA.is_confirm()) {
					if (DATA.selector.bool_0) {
						if (DATA.selector.item == 4) {
							if (!DATA.selector.T_F_bool_0) DATA.selector.bool_0 = false;
							else {	//save and quit
								DATA.state = SAVE_QUIT;
							}
						}
						else if (DATA.selector.item == 1) {	// if key config
							//if nothing selected
							//select key to change
							//else 	save key as selection.
						}
						else	DATA.selector.bool_0 = false;
					}
					else DATA.selector.bool_0 = true;
				
				}
				else if (DATA.is_cancel()) {
					if (DATA.selector.bool_0) {
						// if key change sleceted 	do nothing
						// else 
						DATA.selector.bool_0 = false;
					}
					else DATA.state = GAME;
				}
				else if (DATA.is_alt1()) {
				
				}
				else if (DATA.is_alt2()) {
				
				}
				else if (DATA.is_scroll_up()) {
					//cancel out old flags / dis-allow if flags set
					DATA.state = INVENTORY;
					DATA.clear_selection();
				}
				else if (DATA.is_scroll_down()) {
					//cancel out old flags / dis-allow if flags set
					DATA.state = MAGIC;
					DATA.clear_selection();
				}
			}
			//*********************************************************
			else if (DATA.state == MAGIC) {	//	MAGIC	input
												//---------------------
				if (DATA.is_direction()) {
					
				}
				else if (DATA.is_confirm()) {

				}
				else if (DATA.is_cancel()) {
					DATA.state = GAME;
				}
				else if (DATA.is_alt1()) {
				
				}
				else if (DATA.is_alt2()) {
				
				}
				else if (DATA.is_scroll_up()) {
					//cancel out old flags / dis-allow if flags set
					DATA.state = OPTIONS;
					DATA.clear_selection();
				}
				else if (DATA.is_scroll_down()) {
					//cancel out old flags / dis-allow if flags set
					DATA.state = INVENTORY;
					DATA.clear_selection();
				}
			}
			//*********************************************************
			else if (DATA.state == DEATH) {		//	INTRO	input
												//---------------------
				if (DATA.is_direction()) {
					
				}
				else if (DATA.is_confirm()) {
					DATA.state = GAME;
				}
				else if (DATA.is_cancel()) {

				}
				else if (DATA.is_alt1()) {
				
				}
				else if (DATA.is_alt2()) {
				
				}
				else if (DATA.is_scroll_up()) {

				}
				else if (DATA.is_scroll_down()) {

				}
			}
			//*********************************************************
			else if (DATA.state == SHOP) {		//	SHOP	input
												//---------------------
				if (DATA.is_direction()) {
					
				}
				else if (DATA.is_confirm()) {

				}
				else if (DATA.is_cancel()) {

				}
				else if (DATA.is_alt1()) {
				
				}
				else if (DATA.is_alt2()) {
				
				}
				else if (DATA.is_scroll_up()) {

				}
				else if (DATA.is_scroll_down()) {

				}
			}
			//*********************************************************
			else if (DATA.state == INTRO) {		//	INTRO	input
												//---------------------
				if (DATA.is_direction()) {
					
				}
				else if (DATA.is_confirm()) {
					DATA.state = GAME;
				}
				else if (DATA.is_cancel()) {

				}
				else if (DATA.is_alt1()) {
				
				}
				else if (DATA.is_alt2()) {
				
				}
				else if (DATA.is_scroll_up()) {

				}
				else if (DATA.is_scroll_down()) {

				}
			}
			//*********************************************************
			else if (DATA.state == SAVE_QUIT) {		//	QUIT	input
												//---------------------
				DATA.running = false;
			}
		}
		//*********************************************************
		//end input section
		//*********************************************************

	DATA.input=0;


		//*********************************************************
		if (DATA.state == GAME) {		//	Draw GAME window
										//-----------------------		
			if (DATA.automate_movement && DATA.state == GAME) {										//if running
				DATA.action_performed = true;
				if (L_can_see_monster(DATA)) {
					DATA.automate_movement = false;
					DATA.add_message("You notice a monster");
					DATA.flags_off();
				}
				if (DATA.MAP_DATA.map[DATA.OBJ_DATA.hero.y][DATA.OBJ_DATA.hero.x].get_has_item()) {
					DATA.automate_movement = false;	
					string alert = "";		
					alert += L_to_upper( DATA.OBJ_DATA.item_name(DATA.OBJ_DATA.item_list[get_item(DATA)]) );
					if (is_vowel(alert[0])) alert = "An " + alert;
					else alert = "A " + alert;
					alert += " lays upon the floor";

					alert += ".";
					DATA.add_message(DATA.parser(alert));
					DATA.flags_off();
				}		
				if (DATA.automate_movement)	if (!step_hero(DATA, dy, dx) ) { 
					DATA.automate_movement = false; DATA.flags_off(); }							
			}

			L_armor_effects(DATA);	//removes all equ buffs, re-applys those on current equipment, and then sets stats to reflect
			//strip armor buffs
			//add new armor buffs
				
			if (DATA.action_performed) {	// turn taken
				DATA.turn_counter++;
				DATA.OBJ_DATA.hero_data.immobilized = false;
				DATA.OBJ_DATA.hero_data.cant_act = false;
				DATA.OBJ_DATA.hero.tick_effects();
				
				L_MON_move_monsters(DATA);
				//***^
					L_MAP_draw_parsed_string(34,40,"Action taken");	
				//Map
				//	update map math
				//	tick
				//items
				//	tick	
	//***^
	for (int i = 0; i < DATA.OBJ_DATA.inventory.size(); i++) {
		if ( DATA.OBJ_DATA.inventory[i].is_known() ) continue;
		if ( DATA.OBJ_DATA.inventory[i].get_knowledge_of() >= DATA.OBJ_DATA.hero_data.knowledge_threshold ) {
			DATA.OBJ_DATA.inventory[i].is_known(true);
			DATA.add_message(  DATA.OBJ_DATA.item_name( DATA.OBJ_DATA.inventory[i] )+ "is now known.");
		}
		else
			DATA.OBJ_DATA.inventory[i].add_knowledge_of(1);
	}
	for (int i = 0; i < 7; i++) {
		if ( DATA.OBJ_DATA.equipment[i].is_known() ) continue;
		if ( DATA.OBJ_DATA.equipment[i].get_knowledge_of() >= DATA.OBJ_DATA.hero_data.knowledge_threshold ) {
			DATA.OBJ_DATA.equipment[i].is_known(true);
			
		}
		else
			DATA.OBJ_DATA.equipment[i].add_knowledge_of(4);
	}
				//monsters
				//	tick
				//	take action
				

				DATA.action_performed = false;
			}
			L_MAP_draw_game(DATA);
			//debug stuff
				attron(A_UNDERLINE);
				L_MAP_draw_parsed_string(23,56,"Turn: " );
				attroff(A_UNDERLINE);
				L_MAP_draw_parsed_string(24,56,"    ");
				int offset = 0;
				DATA.turn_counter = DATA.turn_counter%10000;
				if (DATA.turn_counter < 1000) offset++;
				if (DATA.turn_counter < 100) offset++;
				if (DATA.turn_counter < 10) offset++;
				L_MAP_draw_parsed_string(24,56+offset,to_string(DATA.turn_counter));
				

			if (DATA.looking) L_MAP_draw_parsed_string(1,55,"LOOKING");
			if (DATA.targeting) L_MAP_draw_parsed_string(2,55,"TARGETING");
			if (DATA.going_to_use) L_MAP_draw_parsed_string(3,55,"USING");
			if (DATA.selecting_direction) L_MAP_draw_parsed_string(4,55,"SELECTING");
			if (DATA.automate_movement) L_MAP_draw_parsed_string(5,52,"AUTOrun");
			if (DATA.alt_selection) L_MAP_draw_parsed_string(6,52,"ALT");
			
				NC_move(DATA.window_height-1,59);

			if (DATA.looking || DATA.targeting || DATA.going_to_use) {
				attron(A_REVERSE);
				NC_move(dy, dx+DATA.map_x);
				attroff(A_REVERSE | A_UNDERLINE);	
			}
			NC_refresh();
		}
		//*********************************************************
		else if (DATA.state == INVENTORY) {	//	Draw INV window
										//-----------------------
			L_MAP_draw_inv(DATA);
			DATA.saved_menu_state = INVENTORY;
		}
		//*********************************************************
		else if (DATA.state == OPTIONS) {	//	Draw MENU window
										//-----------------------
			L_MAP_draw_options(DATA);
			DATA.saved_menu_state = OPTIONS;
		}
		//*********************************************************
		else if (DATA.state == MAGIC) {	//	Draw MAGIC window
										//-----------------------
			L_MAP_draw_magic(DATA);
			DATA.saved_menu_state = MAGIC;
		}
			//*********************************************************
		else if (DATA.state == DEATH) {	//	Draw DEATH window
										//-----------------------
			L_MAP_draw_parsed_string(1,20,"Another one bites the dust");
		}	
		//*********************************************************
		else if (DATA.state == SHOP) {	//	Draw SHOP window
										//-----------------------
			L_MAP_draw_parsed_string(1,20,"Welcome to my shop");
		}	
		//*********************************************************
		else if (DATA.state == INTRO) {	//	Draw INTRO window
										//-----------------------
			L_MAP_draw_introduction(DATA);
		}	
		//*********************************************************
		else if (DATA.state == SAVE_QUIT) {	//	Draw QUIT window
										//-----------------------
			L_MAP_draw_parsed_string(1,20,"Farewell!");
		}	
		else if (DATA.state == DEBUG) {	//	Draw DEBUG window
										//-----------------------
			L_MAP_draw_debug(DATA);
		}	
		


		DATA.force_update = false;
		if (DATA.automate_movement && DATA.state == GAME) {	
			DATA.force_update = true;
		}
	}// end RUNNING loop
//################################################################################
//         END GAME LOOP
//################################################################################
	exit();	//clean up NCurses	
	DATA.log_file("^^Clean EXIT.\n\n");
	resize_terminal(terminal_original_x, terminal_original_y);

  return 0;
}

  void up_down_input(data& DATA, int& value) {
	int max = DATA.OBJ_DATA.inventory[DATA.selector.item].find_effect_index("quantity");
	if (max == -1) 
		max = 1;
	else
		max = DATA.OBJ_DATA.inventory[DATA.selector.item].get_effect(max).strength;
	if ( DATA.is_up() ) 	value+=10;
	else if ( DATA.is_down() ) 	value-=10;
	else if ( DATA.is_right() ) 	value++;
	else if ( DATA.is_left() ) 	value--;
	if ( value < 1 ) 
		value = 1;
	if (value > max)
		value = max;	
	return;
  }

  void command_selection(data& DATA, int& value, bool& equipment) {
	if ( DATA.is_right() ) 	value++;
	else if ( DATA.is_left() ) 	value--;
	if ( !equipment ) {
		if ( DATA.is_up() ) 		value= value%3;
		else if ( DATA.is_down() ) 	value= value+3;
		if ( value < 0 ) value = 0;
		if ( value > 4 ) value = 4;
	}
	else {
		if ( value < 4 ) value = 5;
		if ( value > 5 ) value = 4;
	}
	return;
  }

  void cycle_value(int& value, item& ITEM, bool increasing ) {
	while (increasing) {
		if ( value == 0 && ITEM.is_head() ) {
			return;
		}
		if ( value == 1 && ITEM.is_neck() ) {
			return;
		}
		if ( value == 2 && ITEM.is_chest() ) {
			return;
		}
		if ( value == 3 && ITEM.is_ring() ) {
			return;
		}
		if ( value == 4 && (ITEM.is_arm() || ITEM.is_both() ) ) {
			return;
		}
		if ( value == 5 && ITEM.is_ring() ) {
			return;
		}
		if ( value == 6 ) {

			if ( ITEM.is_arm() ) {
				if ( ITEM.is_both() ) {
					value = 4;
				}
				return;
			}
		}
		value++;
		if (value > 6 )
			value = 0;
	}

	while (!increasing) {
		if ( value == 6 ) {
			if ( ITEM.is_arm() ) {
				if ( ITEM.is_both() ) {
					value = 4;
				}
			return;
			}
		}
		if ( value == 5 && ITEM.is_ring() ) {
			return;
		}
		if ( value == 4 && (ITEM.is_arm() || ITEM.is_both() ) ) {
			return;
		}
		if ( value == 3 && ITEM.is_ring() ) {
			return;
		}
		if ( value == 2 && ITEM.is_chest() ) {
			return;
		}
		if ( value == 1 && ITEM.is_neck() ) {
			return;
		}
		if ( value == 0 && ITEM.is_head() ) {

			return;
		}
		value--;
		if (value < 0 )
			value = 6;
	}
	return;
  }
/*

   is_head()		0
   is_neck()		1
   is_chest()		2
	//ring 1		3
	//r hand		4
	//ring 2		5
	// l hand		6
   is_arm()
   is_both() 
   is_ring()

*/


  void navigate_equipment(data& DATA, int& value) {
	bool up = false;
	if ( DATA.is_up() ) {
		value-=1;
		
	}
	else if ( DATA.is_down() ) {
		value+=1;
		up =true;
	}
	else if ( DATA.is_right() ) 	{ 
		//if ( value%2 == 0) {
			value++; 
			up =true;
		//}
	}
	else if ( DATA.is_left() ) 	{ 
		//if ( value%2 == 1) {
			value--; 

		//}
	}
//	if ( value >= 6 ) 
//		value = 6;
//	if ( value < 0 ) value = 0;
	cycle_value( value, DATA.OBJ_DATA.inventory[DATA.selector.item], up);
	return;
  }

  void first_selection(data& DATA, int& value, bool& equipment) {
	if ( DATA.is_up() ) 	value-=2;
	else if ( DATA.is_down() ) 	value+=2;
	else if ( DATA.is_right() ) 	{ if ( value%2 == 0) value++; }
	else if ( DATA.is_left() ) 	{ if ( value%2 == 1) value--; }
	if ( value < 0 ) {	
		if ( equipment ) {
			value= (value+1)%2;
			value = DATA.OBJ_DATA.inventory.size()-1 + value ;
			equipment = false;
		}
		else
			value = 0;
	}
	if ( value >= DATA.OBJ_DATA.inventory.size() && !equipment) {
		value = value%2;
		equipment = true;
	}
	if ( value >= 6 && equipment) {
		value = 6;
	}
	if ( value < 0 ) value = 0;
	if ( equipment ) {
		if ( value == 6 && DATA.OBJ_DATA.equipment[4].is_both() )
			value = 4;
	}
	return;
  }


