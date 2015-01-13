#pragma once

#include <string>
#include <sstream>

#include <stdlib.h> //temp for rand

#include <math.h>

#include "../../data/data.h"
#include "../../data/map_data/map_data.h"
#include "../../data/objects/obj_data.h"

#include "../../data/objects/items/item.h"
#include "../../data/objects/units/unit.h"

#include "../../ncurses_helper.h"

#define PI 3.14159265

  void L_MAP_draw_parsed_string(int y, int x, std::string str);
  void L_MAP_draw_parsed_string(int y, int x, std::string str, data& DATA);
  float slope(float y1, float x1, float y2, float x2);
  bool LOS(const unit& source, const int& y, const int& x, const data& DATA);
  std::string to_str (int in) {std::ostringstream stream;	stream << in;	return stream.str();	} 
  std::string to_str (float in) {std::ostringstream stream;	stream << in;	return stream.str();	} 

  std::string append_digit(int i) {
	if (i == 1) return "1st";
	else if (i == 2) return "2nd";
	else if (i == 3) return "3rd";
	else if (i >= 4) return to_str(i) +"th";
  }

  std::string break_str_at(std::string& str, int index) {		//breaks to newline if "|B" encountered
	std::string temp = "";									
	int count = 0;
	int commands = 0;
	while ((count-3*commands) != index) {
		if (str[count] == '|' && str[count+1] == 'B') {
			count++;
			temp = str.substr(0, count-1);
			str = str.substr(count+1);
			return temp;
		}
		if (str[count] == '|') {
			commands++;
		}
		count++;
	}
	commands*=3;
	if (index >= (str.size()-commands ) ) {
		temp = str;
		str = "";
		return temp;
	}
	index = index + commands;
	while ( str[index] != ' ' && index >= 5 ) {
		index--;
	}
	temp = str.substr(0, index);
	str = str.substr(index+1);
	return temp;
  }

  std::string to_upper(std::string in) {
	int i = 0;
	std::string temp = "";
	while (in[i]!=0) {
		temp += toupper(in[i]);
		i++;
	}
	return temp;
  }

  void L_draw_exp_bar(data& DATA, int y, int x) {
	NC_mvaddstr(y,x,"LVL " + to_str(DATA.OBJ_DATA.hero.level));			
	NC_mvaddstr(y,x+7,"[               ]");		// EXP bar
	int range = float(15)*( float(DATA.OBJ_DATA.hero_data.current_exp)/float(DATA.OBJ_DATA.hero_data.next_exp) );

	int test = DATA.OBJ_DATA.hero_data.current_exp;
	int remainder = test % 10;
	for (int i = 14; i >= 0; i--) {
		if (range >= i) 
			attron(A_REVERSE);
		else
			attroff(A_REVERSE);
		switch (i) {
			case (13): NC_mvaddstr(y,x+8+i,"P");	break;
			case (12): NC_mvaddstr(y,x+8+i,"X");	break;
		//	case (11): NC_mvaddstr(y,x+8+i," ");	break;
			default: NC_mvaddstr(y,x+8+i," ");
		}
		if ( (i>0) && (i<11) && (test>0) ) {
			NC_mvaddstr(y,x+8+i,to_str(remainder) );
			test /= 10;
			remainder = test % 10;
		}
	}
	attroff(A_REVERSE);
  }

 void L_draw_quick_stats(data& DATA, int y) {
	NC_mvaddstr(y-1,0,"------------------------------------------------------------");		// draw stats
	std::string qUse = "NONE";							
	if(DATA.OBJ_DATA.spell_info.fallback_id >= 0) {
		if ( DATA.OBJ_DATA.spell_info.fallback_is_spell )
			qUse = DATA.OBJ_DATA.spell_book[DATA.OBJ_DATA.spell_info.fallback_id].get_name();	//	if is spell, set to spell name
		else {
			qUse = DATA.OBJ_DATA.item_name(DATA.OBJ_DATA.inventory[DATA.OBJ_DATA.spell_info.fallback_id]);
			//qUse = DATA.OBJ_DATA.inventory[DATA.OBJ_DATA.spell_info.fallback_id].get_name();
			if ( DATA.OBJ_DATA.inventory[DATA.OBJ_DATA.spell_info.fallback_id].find_effect("quantity") )  {
				int number = DATA.OBJ_DATA.inventory[DATA.OBJ_DATA.spell_info.fallback_id].get_effect( DATA.OBJ_DATA.inventory[DATA.OBJ_DATA.spell_info.fallback_id].find_effect_index("quantity") ).strength;
					if (number > 1) 	qUse += "x"+ to_str( number );	
			}	
		}
	}	
	L_MAP_draw_parsed_string(y-1,30,"Quick Use:-" + qUse);			

//DATA.family_info.name + " the " +append_digit(DATA.family_info.attempt)
	NC_mvaddstr(y,0,"Sir "+DATA.family_info.name + " the " +append_digit(DATA.family_info.attempt) );
//	NC_mvaddstr(y,0,DATA.OBJ_DATA.hero.get_name());
	L_draw_exp_bar(DATA, y, 35);

	NC_mvaddstr(y+1,0,"HP: "+to_str(DATA.OBJ_DATA.hero.hp[2]) +" / "+ to_str(DATA.OBJ_DATA.hero.hp[1]) );
	int life = 40 * (float(DATA.OBJ_DATA.hero.hp[2])/float(DATA.OBJ_DATA.hero.hp[1]));				// Life and HP
	for (int i = 0; i < 40; i++) {
		NC_color_set(9);
		if (i < life) {
			NC_color_set(21);
			NC_mvaddch(y+1,15+i,' ');
		}
		else NC_mvaddch(y+1,15+i,' ');
	}
	int max = 100 * (float(DATA.OBJ_DATA.hero.hp[2])/float(DATA.OBJ_DATA.hero.hp[1]));
	if (max >= 100) {NC_color_set(21);	NC_mvaddstr(y+1,30,"1  0  0  %");}
	else {
		int tens = max/10;
		max = max%10;
		if (life > 18) NC_color_set(21);
		else NC_color_set(9);
		if (tens != 0 )NC_mvaddstr(y+1,33,to_str(tens));
		if (life > 21) NC_color_set(21);
		else NC_color_set(9);
		NC_mvaddstr(y+1,36,to_str(max));
		if (life > 24) NC_color_set(21);
		else NC_color_set(9);
		NC_mvaddch(y+1,39,'%');
	}
	NC_color_set(0);
	NC_mvaddstr(y+2,0,"FLOOR "+ to_str(DATA.game_data.floor) );
	NC_mvaddstr(y+2,15,"GOLD " + to_str(DATA.OBJ_DATA.hero.gold));
	NC_mvaddstr(y+2,42,"SOURCE");		//***^ will be removed
			// source status
  }

  void L_MAP_draw_game_window(data& DATA) {
	int x = 0, y = 0;
	NC_color_set(0);
	for (y=0;y<DATA.map_height;y++) {
		for (x=0;x<DATA.window_width;x++) {	
			if (x < DATA.map_x || x >= (DATA.window_width - DATA.map_x)) {
				NC_color_set(0);
				if(x == DATA.map_x-1 || x == (DATA.window_width -DATA.map_x)) NC_mvaddch(y, x, '|');
				else NC_mvaddch(y, x, '#');
			}	
			else if ((x >= DATA.map_x) && (x < (DATA.window_width - DATA.map_x))) {		//draw map	1-50  1-20
				// MAP DRAWING

			if (LOS(DATA.OBJ_DATA.hero, DATA.MAP_DATA.map_y+y, DATA.MAP_DATA.map_x+x-DATA.map_x, DATA)) {		//if clear LOS
					DATA.MAP_DATA.map[DATA.MAP_DATA.map_y+y][DATA.MAP_DATA.map_x+x-DATA.map_x].set_has_seen(true);
					if (DATA.MAP_DATA.map[DATA.MAP_DATA.map_y+y][DATA.MAP_DATA.map_x+x-DATA.map_x].get_color()==9) NC_color_set(9);//color 2 -> color 4 in fog		
					else NC_color_set(DATA.MAP_DATA.map[DATA.MAP_DATA.map_y+y][DATA.MAP_DATA.map_x+x-DATA.map_x].get_color());	// set color 
			}
			else {															//not in LOS
				if (DATA.MAP_DATA.map[DATA.MAP_DATA.map_y+y][DATA.MAP_DATA.map_x+x-DATA.map_x].get_has_seen()) {	// has seen
					if (DATA.MAP_DATA.map[DATA.MAP_DATA.map_y+y][DATA.MAP_DATA.map_x+x-DATA.map_x].get_color()==9) NC_color_set(10);//color 2 -> color 4 in fog		
					else NC_color_set(1); 
				}
				else NC_color_set(8);		// never has seen
			}
				NC_mvaddch(y, x, DATA.MAP_DATA.map[DATA.MAP_DATA.map_y+y][DATA.MAP_DATA.map_x+x-DATA.map_x].get_char());
				NC_color_set(0);
				// END MAP DRAWING
			}
			else NC_mvaddch(y, x, ' ');
		}
	}
	NC_color_set(0);
	//draw objects
	NC_mvaddstr(DATA.map_height,0,"------------------------------------------------------------");
	//draw stats here
	L_draw_quick_stats(DATA, DATA.map_height+1);
	//draw messages
	NC_mvaddstr(DATA.map_height+4,0,"------------------------------------------------------------");
	for (int i = 0; i < DATA.MESSAGE_DISPLAY_HEIGHT; i++) {
		move(DATA.window_height-DATA.MESSAGE_DISPLAY_HEIGHT+i,0);					// clear old lines
		NC_deleteln();
		if (DATA.messages.size() > i) L_MAP_draw_parsed_string(DATA.window_height-(DATA.MESSAGE_DISPLAY_HEIGHT-i),0,DATA.messages[i+DATA.messages_position].c_str()   );
	}
	color_set(0, NULL);
	if (DATA.messages_position!=0) NC_mvaddstr(30, (DATA.window_width)-2, "/\\");
	else  NC_mvaddstr(30, (DATA.window_width)-2, "--");
	NC_mvaddstr(32, (DATA.window_width)-2, "||");
	if (DATA.messages_position+DATA.MESSAGE_DISPLAY_HEIGHT<DATA.messages.size()) NC_mvaddstr(34, (DATA.window_width)-2, "\\/");
	else  NC_mvaddstr(34, (DATA.window_width)-2, "--");

	return;
  }

  void L_MAP_draw_hero(data& DATA) {
		NC_color_set(3);
		NC_mvaddch(DATA.OBJ_DATA.hero.y-DATA.MAP_DATA.map_y, DATA.OBJ_DATA.hero.x-DATA.MAP_DATA.map_x+DATA.map_x, '@');
	return;
  }

  void L_MAP_draw_items(data& DATA) {
	int item_y, item_x;
	for (int i = 0; i < DATA.OBJ_DATA.item_list.size(); i++) {
		DATA.OBJ_DATA.item_list[i].get_location(item_y, item_x);
		if (LOS(DATA.OBJ_DATA.hero, item_y, item_x, DATA)) {			//if visible to hero
			item_y -= DATA.MAP_DATA.map_y;
			item_x -= DATA.MAP_DATA.map_x-DATA.map_x;
			int color = DATA.OBJ_DATA.item_list[i].get_color();
			char character = DATA.OBJ_DATA.item_list[i].get_display_char();
				//apply hero color blind and such....
			NC_color_set(color);
			NC_mvaddch(item_y, item_x, character);
			}
	}
	return;
  }
  void L_MAP_draw_monsters(data& DATA) {
	int item_y, item_x;
	for (int i = 0; i < DATA.OBJ_DATA.monster_list.size(); i++) {
		DATA.OBJ_DATA.monster_list[i].get_location(item_y, item_x);
		DATA.MAP_DATA.map[item_y][item_x].set_has_unit(true);
		if (LOS(DATA.OBJ_DATA.hero, item_y, item_x, DATA)) {			//if visible to hero
			item_y -= DATA.MAP_DATA.map_y;
			item_x -= DATA.MAP_DATA.map_x-DATA.map_x;
			int color = DATA.OBJ_DATA.monster_list[i].get_color();
			char character = DATA.OBJ_DATA.monster_list[i].get_char();
				//apply hero color blind and such....
				// or if invisble monster
			NC_color_set(color);
			NC_mvaddch(item_y, item_x, character);
		}
	}
	return;
  }

  void L_MAP_draw_game(data& DATA) {
		L_MAP_draw_game_window(DATA);
		L_MAP_draw_items(DATA);
		L_MAP_draw_monsters(DATA);
		L_MAP_draw_hero(DATA);
	return;
  }

  void L_MAP_draw_debug(data& DATA) {			// DEBUG information
	NC_color_set(21);
	NC_mvaddstr(1,10,"DEBUG MENU");
	NC_color_set(0);
	NC_mvaddstr(2,2,"HERO effects:");
	L_MAP_draw_parsed_string(2,16," size: " + to_str(int(DATA.OBJ_DATA.hero.effects.size()))+"  ");
	for (int i =0; i < DATA.OBJ_DATA.hero.effects.size();i++) 
		L_MAP_draw_parsed_string(3+i,5,DATA.OBJ_DATA.hero.effects[i].get_tag() + " D:" + to_str(int(DATA.OBJ_DATA.hero.effects[i].duration))+"  S:"+ to_str(int(DATA.OBJ_DATA.hero.effects[i].strength))+"  ");
	if (DATA.OBJ_DATA.spell_info.spell_id >= 0) {
		int number = DATA.OBJ_DATA.spell_info.spell_id;
		NC_mvaddstr(2,30,"ITEM effects:");
		L_MAP_draw_parsed_string(2,44," size: " + to_str(int(DATA.OBJ_DATA.inventory[number].effects_size()))+"  ");
		for (int i =0; i < DATA.OBJ_DATA.inventory[number].effects_size();i++) 
			L_MAP_draw_parsed_string(3+i,32,DATA.OBJ_DATA.inventory[number].get_effect(i).get_tag() + " D:" + to_str(int(DATA.OBJ_DATA.inventory[number].get_effect(i).duration))+ "  R:" + to_str(int(DATA.OBJ_DATA.inventory[number].get_effect(i).range))+"  ");
	}

	NC_mvaddstr(16,3,"Item List");
	L_MAP_draw_parsed_string(16,12,": " + to_str(int(DATA.OBJ_DATA.item_list.size()))+"  ");
	for (int i =0; i < DATA.OBJ_DATA.item_list.size();i++) 
		L_MAP_draw_parsed_string(17+i,3,DATA.OBJ_DATA.item_list[i].get_name() );

	NC_mvaddstr(16,33,"Monster List:" + to_str(int(DATA.OBJ_DATA.monster_list.size())) );
	for (int i =0; i < DATA.OBJ_DATA.monster_list.size();i++) 
		L_MAP_draw_parsed_string(17+i,33,DATA.OBJ_DATA.monster_list[i].get_name() );

	NC_mvaddstr(13,2,"Q.Cast: " + to_str(DATA.OBJ_DATA.spell_info.spell_id) + "  is spell? " +((DATA.OBJ_DATA.spell_info.is_spell)?"true":"false") + "  " +  ((DATA.OBJ_DATA.spell_line_cast )?"Line":"target") );
	NC_mvaddstr(14,1,"2Q.Cast: " + to_str(DATA.OBJ_DATA.spell_info.fallback_id) + "  is spell? " +((DATA.OBJ_DATA.spell_info.fallback_is_spell)?"true":"false") );
	
  }

  void L_draw_equipment(data& DATA, int y) {
	attron(A_UNDERLINE);
	NC_mvaddstr(y,0,"####|---Equipment--------------------------------------|####");
	attroff(A_UNDERLINE);

	NC_mvaddstr(y+1,0,"Head:");	
	if ( DATA.selector.item== 0 && DATA.selector.is_equipment ) attron(A_REVERSE);
	if ( DATA.selector.selector_1== 0 && DATA.selector.bool_1 ) attron(A_REVERSE);
	L_MAP_draw_parsed_string(y+1,8,DATA.OBJ_DATA.item_name(DATA.OBJ_DATA.equipment[0]) );
	attroff(A_REVERSE);

	NC_mvaddstr(y+1,30,"Neck:");	
	if ( DATA.selector.item== 1 && DATA.selector.is_equipment ) attron(A_REVERSE);
	if ( DATA.selector.selector_1== 1 && DATA.selector.bool_1 ) attron(A_REVERSE);
	L_MAP_draw_parsed_string(y+1,38,DATA.OBJ_DATA.item_name(DATA.OBJ_DATA.equipment[1]) );
	attroff(A_REVERSE);	

	NC_mvaddstr(y+2,0,"Chest:");	
	if ( DATA.selector.item== 2 && DATA.selector.is_equipment ) attron(A_REVERSE);
	if ( DATA.selector.selector_1== 2 && DATA.selector.bool_1 ) attron(A_REVERSE);
	L_MAP_draw_parsed_string(y+2,8,DATA.OBJ_DATA.item_name(DATA.OBJ_DATA.equipment[2]) );
	attroff(A_REVERSE);

	NC_mvaddstr(y+2,30,"Ring 1:");	
	if ( DATA.selector.item== 3 && DATA.selector.is_equipment ) attron(A_REVERSE);
	if ( DATA.selector.selector_1== 3 && DATA.selector.bool_1 ) attron(A_REVERSE);
	L_MAP_draw_parsed_string(y+2,38,DATA.OBJ_DATA.item_name(DATA.OBJ_DATA.equipment[3]) );
	attroff(A_REVERSE);

	NC_mvaddstr(y+3,0,"R.Hand:");	
	if ( DATA.selector.item== 4 && DATA.selector.is_equipment ) attron(A_REVERSE);
	if ( DATA.selector.selector_1== 4 && DATA.selector.bool_1 ) attron(A_REVERSE);
	L_MAP_draw_parsed_string(y+3,8,DATA.OBJ_DATA.item_name(DATA.OBJ_DATA.equipment[4]) );
	attroff(A_REVERSE);

	NC_mvaddstr(y+3,30,"Ring 2:");	
	if ( DATA.selector.item== 5 && DATA.selector.is_equipment ) attron(A_REVERSE);
	if ( DATA.selector.selector_1== 5 && DATA.selector.bool_1 ) attron(A_REVERSE);
	L_MAP_draw_parsed_string(y+3,38,DATA.OBJ_DATA.item_name(DATA.OBJ_DATA.equipment[5]) );
	attroff(A_REVERSE);

	NC_mvaddstr(y+4,0,"L.Hand:");	
	if ( DATA.selector.item== 6 && DATA.selector.is_equipment ) attron(A_REVERSE);
	if ( (DATA.selector.selector_1== 6 || DATA.OBJ_DATA.inventory[DATA.selector.item].is_both() ) && DATA.selector.bool_1 ) attron(A_REVERSE);
		// in progress
		if ( DATA.selector.is_equipment  ) {
			if ( DATA.selector.item == 4 && DATA.OBJ_DATA.equipment[4].is_both() ) {
				attron(A_REVERSE);
			}
		}
	L_MAP_draw_parsed_string(y+4,8,DATA.OBJ_DATA.item_name(DATA.OBJ_DATA.equipment[6]) );
	attroff(A_REVERSE);

  }

  void L_draw_stats(data& DATA, int y) {
	//STR
	NC_mvaddstr(y,0,"STR "+to_str(DATA.OBJ_DATA.hero.str[1]));

	//DEF
	NC_mvaddstr(y,15,"DEF "+to_str(DATA.OBJ_DATA.hero.def[1]));
	int temp = 0;
	//WIS
	NC_mvaddstr(y,30,"WIS "+to_str(DATA.OBJ_DATA.hero.wis[1]));
	//INT
	NC_mvaddstr(y,45,"INT "+to_str(DATA.OBJ_DATA.hero.Int[1]));

	NC_mvaddstr(y+1,0,"Vision         Evasion");
	NC_mvaddstr(y+1,34,"Status");
	//Vis
	NC_mvaddstr(y+1,7,to_str(DATA.OBJ_DATA.hero.vis[1]));
	// Eva
	NC_mvaddstr(y+1,24,to_str(DATA.OBJ_DATA.hero.eva[1])+"%");


	//	STATUS
	NC_color_set(0);
  }

  void L_draw_resistances(data& DATA, int y) {
	NC_mvaddstr(y,0,"---Elemental Resistances------------------------------------");
	NC_color_set(2); NC_mvaddstr(y+1,0,"FIRE");
				int res_temp = DATA.OBJ_DATA.hero.fire_res[1];
				if (res_temp>100) NC_color_set(4);	else if (res_temp<0) NC_color_set(2);	else NC_color_set(0);
				NC_mvaddstr(y+1,5,to_str(res_temp)+"%");
	NC_color_set(5); NC_mvaddstr(y+1,15,"WATER");
				res_temp = DATA.OBJ_DATA.hero.water_res[1];
				if (res_temp>100) NC_color_set(4);	else if (res_temp<0) NC_color_set(2);	else NC_color_set(0);
				NC_mvaddstr(y+1,21,to_str(res_temp)+"%");
	NC_color_set(3); NC_mvaddstr(y+1,30,"EARTH");
				res_temp = DATA.OBJ_DATA.hero.earth_res[1];
				if (res_temp>100) NC_color_set(4);	else if (res_temp<0) NC_color_set(2);	else NC_color_set(0);
				NC_mvaddstr(y+1,36,to_str(res_temp)+"%");
	NC_color_set(6); NC_mvaddstr(y+1,45,"AIR");
				res_temp = DATA.OBJ_DATA.hero.air_res[1];
				if (res_temp>100) NC_color_set(4);	else if (res_temp<0) NC_color_set(2);	else NC_color_set(0);
				NC_mvaddstr(y+1,49,to_str(res_temp)+"%");
	NC_color_set(0);
  }

  void L_draw_equ_change(data& DATA, int y, int target=-1) {
	//NC_mvaddstr(y,13,"LOCATION");	//	HP
	bool display = true;
	int array[11];
	for (int i = 0; i < 11; i++)	array[i]=0;
//***^ needs edits
	if ( target == -1 )	item().compare(DATA.OBJ_DATA.equipment[DATA.selector.item],array);	//remove equipment, compare to NULL
	else	{
		if ( !DATA.OBJ_DATA.inventory[DATA.selector.item].is_known() ) display = false;
		else 	{
			DATA.OBJ_DATA.inventory[DATA.selector.item].compare(DATA.OBJ_DATA.equipment[DATA.selector.selector_1],array);
			if ( DATA.OBJ_DATA.inventory[DATA.selector.item].is_both() )
				DATA.OBJ_DATA.inventory[DATA.selector.item].grab_minuses(DATA.OBJ_DATA.equipment[DATA.selector.selector_1],array);
		}
	}

	int temp = array[0];
	if (temp>0) NC_color_set(4);	else if (temp<0) NC_color_set(2);	else NC_color_set(0);
	if ( display ) NC_mvaddstr(y,12,"("+to_str(temp)+")");	//	HP
	else			NC_mvaddstr(y,12,"(?)");	//	HP

	temp = array[1];
	if (temp>0) NC_color_set(4);	else if (temp<0) NC_color_set(2);	else NC_color_set(0);
	if ( display )	NC_mvaddstr(y+2,8,"("+to_str(temp)+")");	//	STR
	else				NC_mvaddstr(y+2,8,"(?)");	//	STR
	
	temp = array[2];
	if (temp>0) NC_color_set(4);	else if (temp<0) NC_color_set(2);	else NC_color_set(0);
	if ( display )	NC_mvaddstr(y+2,23,"("+to_str(temp)+")");	//	DEF
	else			NC_mvaddstr(y+2,23,"(?)");	//	DEF
	
	temp = array[3];
	if (temp>0) NC_color_set(4);	else if (temp<0) NC_color_set(2);	else NC_color_set(0);
	if ( display )	NC_mvaddstr(y+2,38,"("+to_str(temp)+")");	//WIS
	else			NC_mvaddstr(y+2,38,"(?)");	//WIS

	temp = array[4];
	if (temp>0) NC_color_set(4);	else if (temp<0) NC_color_set(2);	else NC_color_set(0);
	if ( display )	NC_mvaddstr(y+2,53,"("+to_str(temp)+")");	//	INT
	else			NC_mvaddstr(y+2,53,"(?)");	//INT

	temp = array[5];
	if (temp>0) NC_color_set(4);	else if (temp<0) NC_color_set(2);	else NC_color_set(0);
	if ( display )	NC_mvaddstr(y+3,10,"("+to_str(temp)+")");	//	Vis
	else			NC_mvaddstr(y+3,10,"(?)");	//Vis
	//evasion
	temp = array[6];
	if (temp>0) NC_color_set(4);	else if (temp<0) NC_color_set(2);	else NC_color_set(0);
	if ( display )	NC_mvaddstr(y+3,27,"("+to_str(temp)+"%)");	// evasion
	else			NC_mvaddstr(y+3,27,"(?)");	// evasion

	// move y to resistnaces
	y += 5;
	temp = array[7];
	if (temp>0) NC_color_set(4);	else if (temp<0) NC_color_set(2);	else NC_color_set(0);
	if ( display )	NC_mvaddstr(y,10,"("+to_str(temp)+")");	//	fire
	else 		NC_mvaddstr(y,10,"(?)");	//	fire
	temp = array[8];
	if (temp>0) NC_color_set(4);	else if (temp<0) NC_color_set(2);	else NC_color_set(0);
	if ( display )	NC_mvaddstr(y,25,"("+to_str(temp)+")");	//	water
	else			NC_mvaddstr(y,25,"(?)");	//	water
	temp = array[9];
	if (temp>0) NC_color_set(4);	else if (temp<0) NC_color_set(2);	else NC_color_set(0);
	if ( display )	NC_mvaddstr(y,40,"("+to_str(temp)+")");	//	earth
	else			NC_mvaddstr(y,40,"(?)");	//	earth
	temp = array[10];
	if (temp>0) NC_color_set(4);	else if (temp<0) NC_color_set(2);	else NC_color_set(0);
	if ( display )	NC_mvaddstr(y,55,"("+to_str(temp)+")");	//	air
	else			NC_mvaddstr(y,55,"(?)");	//	air
	NC_color_set(0);
	for (int i = 0; i < 11; i++ )DATA.OBJ_DATA.hero_data.armor_array[i] = array[i];
  }

  void L_draw_favor(data& DATA, int y) {
	NC_mvaddstr(y,0,"---Favor of the gods----------------------------------------");
  }

  std::string swap_menu_colors(std::string in) {
	int i = 0;
	while (in[i] != 0) {
		if (in[i] == '|') {
			//in[i] = 'C';
			i++;
			if (in[i] == 'c') {
				i++;
				switch ( in[i] ) {
				case '1':		//invert purple  1 -> 15
					in.insert(in.begin()+i+1, '5');
					in.insert(i+2, "CURSED ");
					break;
				case '2':		//invert red     2 -> 12
					in[i] = '1';
					in.insert(in.begin()+i+1, '2');
					break;
				case '4':		//invert green	  4 -> 16
					in[i] = '1';
					in.insert(in.begin()+i+1, '6');
					break;
				i++;
				}
			}
		}
		i++;
	}
	return in;
  }

  void draw_command_box(data& DATA) {
	//if ( !DATA.selector.bool_0 ) return;
	attron(A_REVERSE);
	int i = 9;
	NC_mvaddstr(i,15,"                           ");
	if (DATA.selector.is_equipment) {
		if (DATA.selector.selector_0 == 4 ) attron(A_BOLD);
		NC_mvaddstr(i,17," Drop ");	
		attroff(A_BOLD);
		if (DATA.selector.selector_0 == 5 ) attron(A_BOLD);
		NC_mvaddstr(i,33," Remove ");
	}
	else {
		NC_mvaddstr(i+1,15,"                            ");
		if (DATA.selector.selector_0 == 0 ) attron(A_BOLD);
		NC_mvaddstr(i,15," Use ");
		attroff(A_BOLD);
		if (DATA.selector.selector_0 == 1 ) attron(A_BOLD);
		if ( DATA.OBJ_DATA.inventory[DATA.selector.item].is_equipment() ) 
			NC_mvaddstr(i,25," Equip ");
		else
			NC_mvaddstr(i,25," ----- ");
		attroff(A_BOLD);
		if (DATA.selector.selector_0 == 2 ) attron(A_BOLD);
		NC_mvaddstr(i++,35," Q.use ");
		attroff(A_BOLD);
		if (DATA.selector.selector_0 == 3 ) attron(A_BOLD);
		NC_mvaddstr(i,15," Throw ");
		attroff(A_BOLD);
		if (DATA.selector.selector_0 == 4 ) attron(A_BOLD);
		NC_mvaddstr(i,25," Drop ");
	}
	attroff(A_BOLD);	

	attroff(A_REVERSE);
	i = 10;
	NC_color_set(10);
	NC_mvaddstr(i,42," ");
	if (DATA.selector.is_equipment) i--;
	NC_mvaddstr(i+1,16,"                           ");
	NC_color_set(0);
  }

  void inv_menu(data& DATA) {
	//if ( !DATA.selector.bool_0 ) return;
			// if (in shop) game_mode = SHOP; in shop, ask for quantity and confirmation.  ***^
	attron(A_REVERSE);
	for (int i = 0; i < 7; i++) {
		if ( i == 6 ) attron(A_UNDERLINE);
		NC_mvaddstr(2+i,10,"                                        "); 	//in-efficiently draw white box
	}
	attroff(A_UNDERLINE);
	std::string item_name = "";
	item* ITEM = &( DATA.OBJ_DATA.inventory[DATA.selector.item] );
	if (DATA.selector.is_equipment) {
		ITEM = &( DATA.OBJ_DATA.equipment[DATA.selector.item] );
		item_name = DATA.OBJ_DATA.item_name( *ITEM );
	}
	else {		
		item_name = DATA.OBJ_DATA.item_name( *ITEM );
	}
	attron(A_UNDERLINE);	
	L_MAP_draw_parsed_string(2,12,swap_menu_colors(item_name) );
	L_MAP_draw_parsed_string(2,42, to_str(ITEM->get_weight()/10)+ " lbs" );
	attroff(A_UNDERLINE);	
	item_name = "Nothing Equipped.";
	if (ITEM->get_id() != 0 ) { 	// nothing equiped
		item_name = DATA.OBJ_DATA.find_description(*ITEM);
	}
	//item_name = swap_menu_colors(item_name);
	int i = 0;
	while (item_name.size() != 0 && i != 6) {
		if ( i == 5 ) attron(A_UNDERLINE);
		L_MAP_draw_parsed_string(3+i,11,swap_menu_colors(break_str_at(item_name, 38) ) );
		i++;
	}
	attroff(A_UNDERLINE);
	attroff(A_REVERSE);		// draw shadow
	for (int i = 0; i < 7; i++) {
		NC_color_set(10);
		if (i == 6)
			NC_mvaddstr(3+i,11,"                                        "); 
		else
			NC_mvaddch(3+i,50,' '); 
	}
	NC_color_set(0);
	return;
  }

  void draw_quantity_window(data& DATA) {
	int max = DATA.OBJ_DATA.inventory[DATA.selector.item].find_effect_index("quantity");
	if (max == -1) 
		max = 1;
	else
		max = DATA.OBJ_DATA.inventory[DATA.selector.item].get_effect(max).strength;
	attron(A_REVERSE);
	L_MAP_draw_parsed_string(9,15," Drop how many:             ");
	attron(A_BOLD | A_UNDERLINE);
	L_MAP_draw_parsed_string(9,32, " " + to_str(DATA.selector.selector_1) + " " );
	attroff(A_BOLD | A_UNDERLINE);
	L_MAP_draw_parsed_string(9,37, "/ "+to_str(max) + " " );
	attroff(A_REVERSE);
	NC_color_set(10);
	NC_mvaddstr(10,16,"                            "); 
	NC_mvaddch(9,43,' '); 
	NC_color_set(0);
	return;
  }

  void draw_error_mesage(data& DATA) {
	attron(A_REVERSE | A_BOLD);
	L_MAP_draw_parsed_string(9,12,DATA.selector.display_message);
	attroff(A_REVERSE | A_BOLD);
	NC_color_set(10);
	int i = 1;
	for ( ; i < DATA.selector.display_message.size()+1; i++) {
		NC_mvaddch(10,12+i,' '); 
	}
	NC_mvaddch(9,11+i,' '); 
	NC_color_set(0);
	return;
  }

// ------------------------------------
// DRAW INV MENU
// ------------------------------------
  void L_MAP_draw_inv(data& DATA) {
	for (int i = 0; i < ((DATA.OBJ_DATA.inventory_size+1) / 2)+1; i++) {NC_mvaddstr(i,0,"####|");	NC_mvaddstr(i,DATA.window_width-5,"|####");}
	DATA.OBJ_DATA.inventory_weight = 0;
	for (int i = 0; i < 7; i++) DATA.OBJ_DATA.inventory_weight+= DATA.OBJ_DATA.equipment[i].get_weight();

	attron(A_UNDERLINE);	// Draw item list
	NC_mvaddstr(0,7," Items: " +to_str(int(DATA.OBJ_DATA.inventory.size()) )+"/"+to_str(int(DATA.OBJ_DATA.inventory_size))+ " " );
	attroff(A_UNDERLINE);
	for (int i = 0; i < DATA.OBJ_DATA.inventory.size(); i ++) {
		if (i == DATA.selector.item && !DATA.selector.is_equipment ) attron(A_REVERSE);
		int effect_location = DATA.OBJ_DATA.inventory[i].find_effect_index("quantity");
		if (i%2 == 0) {
			L_MAP_draw_parsed_string(1+i/2,6, DATA.OBJ_DATA.item_name(DATA.OBJ_DATA.inventory[i])  );
			if (effect_location == -1)		//if non-stackable
				DATA.OBJ_DATA.inventory_weight += DATA.OBJ_DATA.inventory[i].get_weight();
			else	{
				int number = DATA.OBJ_DATA.inventory[i].get_effect(effect_location).strength;
				DATA.OBJ_DATA.inventory_weight += ( DATA.OBJ_DATA.inventory[i].get_weight() * DATA.OBJ_DATA.inventory[i].get_effect(effect_location).strength );
			}
		}
		else {
			L_MAP_draw_parsed_string(1+i/2,30, DATA.OBJ_DATA.item_name(DATA.OBJ_DATA.inventory[i]) );
			if (effect_location == -1)	{	//if non-stackable
				DATA.OBJ_DATA.inventory_weight += DATA.OBJ_DATA.inventory[i].get_weight();
			}
			else	{
				int number = DATA.OBJ_DATA.inventory[i].get_effect(effect_location).strength;
				DATA.OBJ_DATA.inventory_weight += ( DATA.OBJ_DATA.inventory[i].get_weight() * DATA.OBJ_DATA.inventory[i].get_effect(effect_location).strength );
			}
		}
		attroff(A_REVERSE);
	}
	attron(A_UNDERLINE);	// Draw weight of inventory
	NC_mvaddstr(0,37, to_str(float(DATA.OBJ_DATA.inventory_weight)/10)+ " lbs" );
	attroff(A_UNDERLINE);

	int inv_offset = 15-((DATA.OBJ_DATA.inventory_size+1) / 2);

	L_draw_equipment(DATA, 16-inv_offset);
	
	L_draw_quick_stats(DATA, 23-inv_offset);
	L_draw_stats(DATA, 26-inv_offset);

	L_draw_resistances(DATA, 28-inv_offset);
	L_draw_favor(DATA, 30-inv_offset);

	if ( DATA.selector.selector_0 == 1 && DATA.selector.bool_1 )		// equip
		L_draw_equ_change(DATA, 24-inv_offset, DATA.selector.item );
	if ( DATA.selector.is_equipment && DATA.selector.bool_0 )		// remove
		L_draw_equ_change(DATA, 24-inv_offset);

	if ( DATA.selector.bool_0 )	inv_menu( DATA) ;
	if ( DATA.selector.bool_0 && !DATA.selector.bool_1 )	draw_command_box( DATA );
	if ( DATA.selector.bool_1 && DATA.selector.selector_0 == 4 ) 	draw_quantity_window(DATA);
	if ( DATA.selector.T_F_bool_0) draw_error_mesage(DATA);

	NC_mvaddstr(DATA.window_height-2,0,"------------------------------------------------------------");
	NC_mvaddstr(DATA.window_height-1,0," << Options                                        Magic >> ");


		// debug ***^
	NC_mvaddstr(0,50,"item: "+to_str(DATA.selector.item) );
	if (DATA.selector.is_equipment) NC_mvaddstr(1,50,"Is_Equipment");
	if (DATA.selector.bool_0) NC_mvaddstr(2,50,"bool 0"); 
	if (DATA.selector.bool_1) NC_mvaddstr(3,50,"bool 1"); 
	if (DATA.selector.T_F_bool_0) NC_mvaddstr(4,50,"T/F 1"); 
	NC_mvaddstr(5,50,"sel_0: "+to_str(DATA.selector.selector_0) );
	NC_mvaddstr(6,50,"sel_1: "+to_str(DATA.selector.selector_1) );

	NC_move(DATA.window_height-1,59);
  }


// --------------------------------------------------------------------
//
// --------------------------------------------------------------------
  void L_MAP_draw_options(data& DATA) {
	for (int i = 0; i < 31; i++) {
		if ( i == 1) NC_mvaddstr(i,0,"####|__________________________________________________|####");
		else if (i > 2 && i < 11 ) {
			NC_mvaddstr(i,0,"###|");
			NC_mvaddstr(i,DATA.window_width-4,"|###");
		}
		else if (i==2) {
			NC_mvaddstr(i,0,"####/");
			NC_mvaddstr(i,DATA.window_width-5,"\\####");
		}
		else if (i==11) {
			NC_mvaddstr(i,0,"####\\__________________________________________________/####");
			//NC_mvaddstr(i,DATA.window_width-5,"/####");
		}
		else {
			NC_mvaddstr(i,0,"####|");
			NC_mvaddstr(i,DATA.window_width-5,"|####");
		}
	}
	NC_mvaddstr(2,5,"         ___                            __  ");
	NC_mvaddstr(3,5,"        | . \\ ___  ___  _ _  ___       _\\ \\__    ");
	NC_mvaddstr(4,5,"        |   // . \\/ . || | |/ ._]      \\_____\\    ");
	NC_mvaddstr(5,5,"        |_\\_\\\\___/\\_, |'___|\\___.         \\ \\ ");
	NC_mvaddstr(6,5,"       ___        (___'        __          \\ \\   ");
	NC_mvaddstr(7,5,"      / _ \\  ___  _______ ___ / /_________  \\ \\  ");
	NC_mvaddstr(8,5,"     / __  \\/ _ \\/ __/ -_|_-</ __/ __/ / /   \\ \\  ");
	NC_mvaddstr(9,5,"    /_/  \\_/_//_/\\__/\\__/___/\\__/_/  \\_ /     \\ | ");
	NC_mvaddstr(10,5,"                          Ver. 0.1  <__/       \\| ");

	if (DATA.selector.bool_0) {		// item selected
		if (DATA.selector.item == 0) {
			NC_mvaddstr(14,8,"For over " +to_str(int(DATA.family_info.bloodline_age)) + " years, the " + DATA.family_info.name + " family has");
			NC_mvaddstr(15,6,"descended into the deptsh of the Dunwich ruins.");
			NC_mvaddstr(16,6,"Some have done so for rumors of great riches.");
			NC_mvaddstr(17,6,"Others, on shadows of a dark presence.");
			NC_mvaddstr(18,6,"But most seek loved ones whom never returned...");
			
			std::string temp = "Now you, Sir " + DATA.family_info.name + " the " +append_digit(DATA.family_info.attempt) + " descend, to seek";
			NC_mvaddstr(20,6,temp );
			NC_mvaddstr(21,6,"fame, fortune, and your father." );

			L_MAP_draw_parsed_string(23, 6, "You are the |c3@|c0 sign.");
		}
		else if (DATA.selector.item == 1) {
			int i = 13;
			NC_mvaddstr(i++,10,"Movement Keys");
			NC_mvaddstr(i++,12," NW ");
			NC_mvaddstr(i++,12," N ");
			NC_mvaddstr(i++,12," NE ");
			NC_mvaddstr(i++,12," W ");
			NC_mvaddstr(i++,10," Center ");
			NC_mvaddstr(i++,12," E ");
			NC_mvaddstr(i++,12," SW ");
			NC_mvaddstr(i++,12," S ");
			NC_mvaddstr(i++,12," SE ");
		}
		else if (DATA.selector.item == 2) {
			NC_mvaddstr(12,DATA.window_width/2-12,"Dreamed up and Coded by:");
			NC_mvaddstr(13,DATA.window_width/2-5,"Kyle Bryant");

			NC_mvaddstr(15,DATA.window_width/2-9,"Special Thanks to:");
			NC_mvaddstr(16,DATA.window_width/2-7,"Rachael Bryant");
			NC_mvaddstr(17,DATA.window_width/2-6,"Greg Bayles");

			NC_mvaddstr(19,DATA.window_width/2-14,"Email comments and bugs to:");
			NC_mvaddstr(20,DATA.window_width/2-12,"Email here@not here.com");

			NC_mvaddstr(22,DATA.window_width/2-10,"I hope you enjoy it!");
		}
		else if (DATA.selector.item == 3) {		// stats
			attron(A_UNDERLINE);
			NC_mvaddstr(13,8,"Sir " + DATA.family_info.name + " the " + append_digit(DATA.family_info.attempt));
			attroff(A_UNDERLINE);
			NC_mvaddstr(14,8,"Deepest floor: ");
			NC_mvaddstr(15,8,"Gold hoarded:  ");
			NC_mvaddstr(16,8,"Family deaths: "+to_str(int(DATA.family_info.attempt-1)) );
			NC_mvaddstr(17,8,"Max level:     ");
		}
		else {				 						//save and quit
			attron(A_REVERSE);
			NC_mvaddstr(26,DATA.window_width/2-7," SAVE & QUIT ");
			attroff(A_REVERSE);
			
			NC_mvaddstr(18,DATA.window_width/2-6,"Are you sure?");
			if (DATA.selector.T_F_bool_0) attron(A_REVERSE);
			NC_mvaddstr(19,DATA.window_width/2+1," YES ");
			attroff(A_REVERSE);
			if (!DATA.selector.T_F_bool_0) attron(A_REVERSE);
			NC_mvaddstr(19,DATA.window_width/2-5," NO ");
			attroff(A_REVERSE);
		}


		if (DATA.selector.item != 4) {		
			attron(A_REVERSE);
			NC_mvaddstr(DATA.window_height-7,43," BACK ");
			attroff(A_REVERSE);
		}
	}
	else {		// nothing selected
		if (DATA.selector.item == 0) attron(A_REVERSE);
		NC_mvaddstr(14,DATA.window_width/2-3," HELP ");
		attroff(A_REVERSE);
		if (DATA.selector.item == 1) attron(A_REVERSE);
		NC_mvaddstr(17,DATA.window_width/2-6," KEY CONFIG ");
		attroff(A_REVERSE);
		if (DATA.selector.item == 2) attron(A_REVERSE);
		NC_mvaddstr(20,DATA.window_width/2-3," ABOUT ");
		attroff(A_REVERSE);
		if (DATA.selector.item == 3) attron(A_REVERSE);
		NC_mvaddstr(23,DATA.window_width/2-3," STATS ");
		attroff(A_REVERSE);
		if (DATA.selector.item == 4) attron(A_REVERSE);
		NC_mvaddstr(26,DATA.window_width/2-7," SAVE & QUIT ");
		attroff(A_REVERSE);
	}
	

	NC_mvaddstr(DATA.window_height-6,0,"------------------------------------------------------------");
	L_draw_quick_stats(DATA, 30);
	NC_mvaddstr(DATA.window_height-2,0,"------------------------------------------------------------");
	NC_mvaddstr(DATA.window_height-1,0," << Magic                                          Items >> ");
  }

  void L_MAP_draw_magic(data& DATA) {
int i = 0;
NC_mvaddstr(i++,0,"+---------------------------AIR----------------------------+");
NC_mvaddstr(i++,0,"|                                                          |");
NC_mvaddstr(i++,0,"|                          god 1                           |");
NC_mvaddstr(i++,0,"|     -70%              ------------                       |");
NC_mvaddstr(i++,0,"|    god 2              ------------              god 3    |");
NC_mvaddstr(i++,0,"|  ------------         ------------         ------------  |");
NC_mvaddstr(i++,0,"|  ------------         ------------         ------------  |");
NC_mvaddstr(i++,0,"|  ------------         ------------         ------------  |");
NC_mvaddstr(i++,0,"|  ------------         ------------         ------------  |");
NC_mvaddstr(i++,0,"|  ------------         ------------         ------------  |");
NC_mvaddstr(i++,0,"|  ------------         ------------         ------------  |");
NC_mvaddstr(i++,0,"F                                                          W");
NC_mvaddstr(i++,0,"I                            /\\                            A");
NC_mvaddstr(i++,0,"R                           <  >                           T");
NC_mvaddstr(i++,0,"E                            \\/                            E");
NC_mvaddstr(i++,0,"|                                                          R");
NC_mvaddstr(i++,0,"|  ------------         ------------         ------------  |");
NC_mvaddstr(i++,0,"|  ------------         ------------         ------------  |");
NC_mvaddstr(i++,0,"|  ------------         ------------         ------------  |");
NC_mvaddstr(i++,0,"|  ------------         ------------         ------------  |");
NC_mvaddstr(i++,0,"|  ------------         ------------         ------------  |");
NC_mvaddstr(i++,0,"|  ------------         ------------         ------------  |");
NC_mvaddstr(i++,0,"|    god 2'             ------------              god 3'   |");
NC_mvaddstr(i++,0,"|     100%              ------------                       |");
NC_mvaddstr(i++,0,"|                          god 1'                          |");
NC_mvaddstr(i++,0,"|                                                          |");
NC_mvaddstr(i++,0,"+--------------------------EARTH---------------------------+");
NC_color_set(21); //red
NC_mvaddch(11,0,'F');
NC_mvaddch(12,0,'I');
NC_mvaddch(13,0,'R');
NC_mvaddch(14,0,'E');

NC_color_set(2); //red
for (int i = 1; i < 26; i++) {
	if (i >10 && i < 15) continue;
	NC_mvaddch(i,0,'|');
}
NC_color_set(5); //blue
for (int i = 1; i < 26; i++) {
	if (i >10 && i < 16) continue;
	NC_mvaddch(i,59,'|');
}
NC_color_set(3); //yellow
NC_mvaddstr(26,1,"--------------------------EARTH---------------------------");
NC_color_set(22); //yellow
NC_mvaddstr(26,27,"EARTH");
NC_color_set(20); //blue
NC_mvaddch(11,59,'W');
NC_mvaddch(12,59,'A');
NC_mvaddch(13,59,'T');
NC_mvaddch(14,59,'E');
NC_mvaddch(15,59,'R');
NC_color_set(6); //cyan
NC_mvaddstr(0,1,"---------------------------AIR----------------------------");
NC_color_set(23); //cyan
NC_mvaddstr(0,28,"AIR");

// work in progress

NC_color_set(0); //cyan
int k = rand()%20+1;
int set = rand()%51;
for (int i =0;i<58;i++) {
	if (i>=27 && i<=30) continue;
	int j = 0;
	j = int(1.1*sin((i+set)/k));
	NC_mvaddch(13+j, i+1, '-');
}
	NC_mvaddstr(33,33,to_str(k));
// end sine wave work...

NC_mvaddstr(DATA.window_height-6,0,"------------------------------------------------------------");
L_draw_quick_stats(DATA, 30);
NC_mvaddstr(DATA.window_height-2,0,"------------------------------------------------------------");
	NC_mvaddstr(DATA.window_height-1,0," << Items                                        Options >> ");
//end
}

//------------------------------------------------------------------------------------
//		I N T R O D U C T I O N
//------------------------------------------------------------------------------------
  void L_MAP_draw_introduction(data& DATA) {
	NC_mvaddstr(0,4,"+--------------------------------------------------+");
	NC_mvaddstr(1,4,"|         ___                            __        |");
	NC_mvaddstr(2,4,"|        | . \\ ___  ___  _ _  ___       _\\ \\__     |");
	NC_mvaddstr(3,4,"|        |   // . \\/ . || | |/ ._]      \\_____\\    |");
	NC_mvaddstr(4,4,"|        |_\\_\\\\___/\\_, |'___|\\___.         \\ \\     |");
	NC_mvaddstr(5,4,"|       ___        (___'        __          \\ \\    |");
	NC_mvaddstr(6,4,"|      / _ \\  ___  _______ ___ / /_________  \\ \\   |");
	NC_mvaddstr(7,4,"|     / __  \\/ _ \\/ __/ -_|_-</ __/ __/ / /   \\ \\  |");
	NC_mvaddstr(8,4,"|    /_/  \\_/_//_/\\__/\\__/___/\\__/_/  \\_ /     \\ | |");
	NC_mvaddstr(9,4,"|                          Ver. 0.1  <__/       \\| |");
	NC_mvaddstr(10,4,"+--------------------------------------------------+");
	NC_mvaddstr(11,6,"For years, your family has descended the depths");
	NC_mvaddstr(12,5,"of the Dunwich ruins. Some out of rumors of great");
	NC_mvaddstr(13,4,"riches. Others in righteous fevor to save the world.");
	NC_mvaddstr(14,6,"But most simply wish to find their loved ones");
	NC_mvaddstr(15,11,"whom never returned...");

	L_MAP_draw_parsed_string(17, 6, "You are the |c3@|c0 sign.");

	L_MAP_draw_parsed_string(20, 22, "Press [c]");

	attron(A_UNDERLINE);
	NC_mvaddstr(28,5," Controls:                                      ");
	attroff(A_UNDERLINE);
	NC_mvaddstr(29,6,"[Arrow Keys / Num Pad]  movement");
	NC_mvaddstr(30,6,"           [s]  target       [d]  look   ");
	NC_mvaddstr(31,6,"[z] autorun [x] cancel/menu   [c] use/confirm ");
	NC_mvaddstr(32,6,"[Page Up] Scroll up    [Page Down] Scroll down");
  }
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

  void L_MAP_draw_parsed_string(int y, int x, std::string str, data& DATA, int type) {	//prints out a string with color and such, returns # of \n's found
	//item color, name, etc handeled elsewhere
	//type 	0 = meantal conditon/always occurs  1 = reading aliements hould apply	2 = hearing aliments apply
	// known/true items call the following function. 

// needs to be re-implemented and then delete this function
	//str = parser(DATA, str, type);
	L_MAP_draw_parsed_string(y, x, str);
	return;
  }

void L_MAP_draw_parsed_string(int y, int x, std::string str) {	//prints out a string with color and such, returns # of \n's found
	int i = 0;
	int x_offset =0;
	NC_color_set(0);
	while (str[i]!=0) {
		if (str[i] == '|') {			//  |  = tag flag
			i++;
			if (str[i] == 'c' || str[i] == 'C') {					// c  color
				i++;		
				if (str[i+1] >= '0' && str[i+1] <= '9') {	// if greater than 9
					i++;
					switch (str[i]) {
						case '0':	NC_color_set(10);	break;
						case '1':	NC_color_set(11);	break;
						case '2':	NC_color_set(12);	break;
						case '3':	NC_color_set(13);	break;
						case '4':	NC_color_set(14);	break;
						case '5':	NC_color_set(15);	break;
						case '6':	NC_color_set(16);	break;

					}		
				}
				else {
					switch (str[i]) {
						case '0':	NC_color_set(0);	break;
						case '1':	NC_color_set(1);	break;
						case '2':	NC_color_set(2);	break;
						case '3':	NC_color_set(3);	break;
						case '4':	NC_color_set(4);	break;
						case '5':	NC_color_set(5);	break;
						case '6':	NC_color_set(6);	break;
						case '7':	NC_color_set(7);	break;
						case '8':	NC_color_set(6);	break;
						case '9':	NC_color_set(7);	break;
						case 'B':	attron(A_BOLD);	break;
						case 'b':	attroff(A_BOLD);	break;
					}			
				}
				i++;
				continue;
			}
		} 
	NC_mvaddch(y,x+x_offset,str[i]);
	x_offset++;
	i++;
	}
	return;
  }

float slope(float y1, float x1, float y2, float x2) {		//finds slope from point(1) to point(2)
	if ((x2 - x1) == 0) return 100000;
	return ( ((y1-y2))/(x2-x1) );
}

bool LOS(const unit& source, const int& y, const int& x, const data& DATA) {		// caluclate a Line Of Sight from source to x,y
	if (DATA.debug[0]) return true;
	float x1 = source.x+0.5;
	float y1 = source.y+0.5;
	float y2 = y + 0.5;										
	float x2 = x + 0.5;							
	float slopes = slope(y1, x1, y2, x2);			
	
	if (x2 > x1 && y2 < y1)	{	//Q1
		if (slopes > 1) x2 -= 0.5;
		if (slopes < 1) y2 += 0.5;
	}	
	if (x2 > x1 && y2 > y1)	{		//Q2
		if (slopes > -1) y2 -= 0.5;
		if (slopes < -1) x2 -= 0.5;
	}
	if (x2 < x1 && y2 > y1)	{	//Q3
		if (slopes > 1) x2 += 0.5;
		if (slopes < 1) y2 -= 0.5;
	}
	if (x2 < x1 && y2 < y1)	{		//Q4
		if (slopes > -1) y2 += 0.5;
		if (slopes < -1) x2 += 0.5;
	}

	float dx = std::abs(x1-x2);
	float dy = std::abs(y1-y2);
	float move_x, move_y, error;
	if (dx<2 && dy<2)return true; // directly adjacent squares
	if ( (pow(dx,2) + pow(dy,2)) > pow(source.vis[1],2) ) return false;	//if outside of view radius
	move_x = x2 >= x1 ? 1 : -1;
	move_y = y2 >= y1 ? 1 : -1;
	dx*=2;
	dy*=2;

	if (dx >= dy) {
		error = dy -(dx/2);
		while (x2 != x1) {
			if (DATA.MAP_DATA.map[int(y1)][int(x1)].get_blocks_vision()) return false;//		
			if (error >0) {
				if (error || (move_x > 0)) {
					y1+= move_y;
					error -=dx;
				} 
			}
			x1+= move_x;
			error += dy;
		}
	}
	else {
		error = dx - (dy/2);
		while (y1 != y2) {
			if (DATA.MAP_DATA.map[int(y1)][int(x1)].get_blocks_vision()) return false;//
			if (error>0) {
				if (error || (move_y > 0)) {
					x1 += move_x;
					error -= dy;
				}
			}
		y1 += move_y;
		error += dx;
		}
	}
	return true;
}




