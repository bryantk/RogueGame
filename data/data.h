#pragma once
#ifndef DATA_H
#define DATA_H

#include <iostream>
#include <fstream>
#include <ctime>

#include <sstream>

#include "map_data/map_data.h"
#include "objects/obj_data.h"

#include "objects/items/item.h"
#include "objects/units/unit.h"

enum Game_State {GAME,INVENTORY,MAGIC,OPTIONS,INTRO,DEATH,SHOP,SAVE_QUIT,DEBUG};

class data {
public:
		//hero family info
	struct family {
		unsigned int bloodline_age;
		unsigned int age;
		unsigned int attempt;
		std::string name;
	} family_info;
	struct game_dat {
		int floor;
	} game_data;
		//menu selcetors
	struct selectors {
		int item;				//used to determine item location in inventory/equipment and if equipment
		bool is_equipment;

		int selector_0;
		int selector_1;

		bool bool_0;
		bool bool_1;

		bool T_F_bool_0;
		std::string display_message;
	} selector;
		//map constants
	short int map_max_width;
	short int map_max_height;
	short int map_width;
	short int map_height;
	short int map_x;
	short int map_y;
	short int window_width;
	short int window_height;
		//message system constants
	short int MAX_MESSAGES;
	short int MESSAGE_DISPLAY_HEIGHT;
	short int MESSAGE_DISPLAY_WIDTH;
		//control defaults
	char CONFIRM_KEY;
	char CANCEL_KEY;
	char ALT_1_KEY;
	char ALT_2_KEY;
	char ALT_KEY;

	short int UP_SCROLL;
	short int DOWN_SCROLL;

	char NW_KEY;		char N_KEY;			char NE_KEY;
	char W_KEY;		char CENTER_KEY;		char E_KEY;
	char SW_KEY;		char S_KEY;			char SE_KEY;
							short int N_ARROW;
	short int W_ARROW;		short int S_ARROW;		short int E_ARROW;
	

		//DATA
	int parser_random_seed;	//set seed of message randomizer
	std::vector<std::string> messages;
	int messages_position;

	Game_State state;
	Game_State saved_menu_state;
	bool running;
	int input;
	bool force_update;
	bool action_performed;
	unsigned int turn_counter;

	bool looking;
	bool targeting;
	bool selecting_direction;
	bool going_to_use;
	bool alt_selection;

	bool automate_movement;

	map_data MAP_DATA;
	obj_data OBJ_DATA;

		// DELETE ME when done
		bool debug[10];

  data() {
//BEGIN DATA
parser_random_seed = 0;
for (int i = 0; i < 10; i++) debug[i] = false;
// family info
family_info.name = "";
family_info.age = 0;
family_info.bloodline_age = 0;
family_info.attempt = 1;
// GAME DATA
game_data.floor = 1;
// menu selectors
selector.item = 0;
selector.is_equipment = false;
selector.selector_0 = 0;
selector.selector_1 = 1;
selector.bool_0 = false;
selector.bool_1 = false;
selector.T_F_bool_0 = false;
//constants
map_max_width = 	256;
map_max_height = 	256;
map_width = 		50;
map_height = 		25;
map_x = 			5;
map_y = 			0;
window_width = 	60;
window_height = 	35;

		//message system constants
MAX_MESSAGES = 			30;
MESSAGE_DISPLAY_HEIGHT = 	5;
MESSAGE_DISPLAY_WIDTH = 	58;

		//control defaults
CONFIRM_KEY = 		'c';
CANCEL_KEY = 		'x';
ALT_1_KEY = 		'd';
ALT_2_KEY = 		's';
ALT_KEY =			'z';

UP_SCROLL = 		339;
DOWN_SCROLL = 		338;

NW_KEY = 			'7';	N_KEY = 			'8';	NE_KEY = 			'9';
W_KEY = 			'4';	CENTER_KEY = 		'5';	E_KEY = 			'6';
SW_KEY = 			'1';	S_KEY = 			'2';	SE_KEY = 			'3';

							N_ARROW = 			259;
W_ARROW = 			260;	S_ARROW = 			258;	E_ARROW = 			261;
//END of constant definitions
	
	messages_position = 0;
	
	running = true;
	saved_menu_state = INVENTORY;
	state = INTRO;
	input = 0;
	action_performed = false;
	force_update = true;
	turn_counter = 0;

	looking = false;
	targeting = false;
	selecting_direction = false;
	going_to_use = false;
	alt_selection = false;

	automate_movement = false;
  }
  ~data() {
	//Clean up data usage / save?
  }

  void load_keys() {

  }
  void save_keys() {

  }

  void clear_selection() {
	selector.item = 0;
  }

  void flags_off() {
	looking = false;
	targeting = false;
	selecting_direction = false;
	going_to_use = false;
	alt_selection = false;
  }

  bool is_direction() {
	if (input == NW_KEY) return true;
	if (input == N_KEY) return true;
	if (input == NE_KEY) return true;
	if (input == W_KEY) return true;
	if (input == CENTER_KEY) return true;
	if (input == E_KEY) return true;
	if (input == SW_KEY) return true;
	if (input == S_KEY) return true;
	if (input == SE_KEY) return true;

	if (input == N_ARROW) return true;
	if (input == S_ARROW) return true;
	if (input == W_ARROW) return true;
	if (input == E_ARROW) return true;
	return false;
  }

  bool is_up() {if (input == N_KEY) return true; if (input == N_ARROW) return true; return false;}
  bool is_down() {if (input == S_KEY) return true; if (input == S_ARROW) return true; return false;}
  bool is_right() {if (input == E_KEY) return true; if (input == E_ARROW) return true; return false;}
  bool is_left() {if (input == W_KEY) return true; if (input == W_ARROW) return true; return false;}

  bool is_confirm() {if(input== CONFIRM_KEY) return true; return false;}
  bool is_cancel() {if(input== CANCEL_KEY) return true; return false;}
  bool is_alt1() {if(input== ALT_1_KEY) return true; return false;}
  bool is_alt2() {if(input== ALT_2_KEY) return true; return false;}
  bool is_alternate() {if(input== ALT_KEY) return true; return false;}

  bool is_scroll_up() {if(input== UP_SCROLL) return true; return false;}
  bool is_scroll_down() {if(input== DOWN_SCROLL) return true; return false;}

  std::string to_upper(std::string in) { int i = 0;	while (in[i] != 0) {	in[i] = toupper(in[i]);	i++;	}	return in; }
  std::string to_str (int in) {std::ostringstream stream;	stream << in;	return stream.str();	} 

  void break_at(std::string& in, std::string& second, const int& length){
	second = "";
	int i = length;
	while (in.size()>length ) {						//while needs newline
		while (in[i]!=' ') {
			i--;
		}
		second =in.substr(i+1,in.size()-1);
		in.erase(in.begin()+i, in.end());
	}
  }

void add_message(std::string in) {
//**********************

	//***^	CODE HERE:	in = hero effects(in);

//**********************
	while (in.size()>MESSAGE_DISPLAY_WIDTH ) {						//while needs newline
		int i = MESSAGE_DISPLAY_WIDTH;//
		if (messages.size()>=MAX_MESSAGES) messages.erase(messages.begin());
		while (in[i]!=' ') {//
			i--;//
		}//
		messages.push_back(in.substr(0,i));//
		in.erase(in.begin(),in.begin()+i+1);//
	}
	messages.push_back(in);
	if (messages.size() > MAX_MESSAGES) messages.erase(messages.begin());	//if full, erase oldest
	messages_position = messages.size() - MESSAGE_DISPLAY_HEIGHT;			//set scroll to newest message
	if (messages_position<0) messages_position = 0;
  }

  void scoll_messages(bool scroll_up) {
	if (scroll_up && messages_position > 0) messages_position--;
	if (!scroll_up && messages.size() > MESSAGE_DISPLAY_HEIGHT) {
		if (messages_position < (MAX_MESSAGES - MESSAGE_DISPLAY_HEIGHT) && 
		    messages_position < (messages.size() - MESSAGE_DISPLAY_HEIGHT)  ) messages_position++;
	}  
 }

std::string parser(std::string in, bool written) {
	return parser_main( in, (written)?(0):(2));
}
std::string parser(std::string in) {
	return parser_main( in, 1);
}


std::string parser_main(std::string in, int type) {

//define written 0
//define mental    1
//define heard   2 
	std::string str = in;
	//***
	int i = 0;
	//***^ temp solution
		srand (parser_random_seed);
	bool color_blind= OBJ_DATA.hero.find_effect("color_blind");
	bool dyslexic	= OBJ_DATA.hero.find_effect("dyslexic");
	bool illiterate	= OBJ_DATA.hero.find_effect("illiterate");
	bool leet	= 	  OBJ_DATA.hero.find_effect("1337");
	bool hard_hearing = OBJ_DATA.hero.find_effect("hard_hearing");
	bool rosey = 		OBJ_DATA.hero.find_effect("rose_world");
	// ideas!
	// bad at math / exagerates:	changes all numbers * (1+rand()%strength/100)


	while (str[i]!=0) {	//first pass, for single letter ailments
		if (str[i]==' ') i++;
		else {
			if (str[i] == '|' && (str[i+1] == 'c' || str[i+1] == 'C')) {			//  skip if color tag
				i+=3;
			} 
			if (illiterate && type == 0 ) {
				if (str[i] < '0' || str[i] > '9') {
					if (rand()%101 <= OBJ_DATA.hero.find_effect_strength("illiterate"))				// how bad is your reading?
					str[i] = rand()%(122-65)+65;	// check if written text or speech
				}
			}
			if ( leet ) {
				if (rand()%101 <= OBJ_DATA.hero.find_effect_strength("1337")) {
					switch (str[i]) {
						case 'o':	str[i]='0';	break;
						case 'A':	str[i]='4';	break;
						case 'E':	
						case 'e':	str[i]='3';	break;
						case 'l':	str[i]='1';	break;
						case 'B':	str[i]='8';	break;
						case 'T':	
						case 't':	str[i]='7';	break;
					}
				}
			}
			i++;
		}
	}
	i = 0;
	std::string word = "";
	while (str[i]!=0) {	//2nd pass, for word ailments
		if (str[i] >= ' ' && str[i] <= '/') {
			if ( rosey ) {
				if ( rand()%101 <= OBJ_DATA.hero.find_effect_strength("rose_world") ) {
					// add word EX
					if (word == "the") { str.insert(i," handsome"); word = ""; } 
					//Replace word EX
					if (word == "right") { str.insert(i,"ONE"); word = "RIGHT";}			
				}
			}
			if (leet) {
				if (word == "7h3" || word == "7he" || word == "th3" || word == "the") word = "teh";
				
			}
			if ( dyslexic && type == 0 ) {
				std::string swap = "";
				if (word.size() > 3) {
					swap+=word[0];
					word.erase(word.begin());
					char end = word[word.size()-1];
					word.erase(word.end()-1);

					for (int i = word.size(); i > 0; i--) {
						int r = rand()%word.size();
						swap+=word[r];
						word.erase(word.begin()+r);
					}
					word = swap;
					word += end;
				}
			}
			if ( hard_hearing && type == 2 ) {
				if ( rand()%101 <= OBJ_DATA.hero.find_effect_strength("hard_hearing") ) {
					std::string temp = "";
					for (int j = 0; j < word.size(); j++) temp+='_';
					word = temp;
				}
			}
			if (word != "")
				str.replace((i-word.size()), word.size(), word );
			word = "";
		} 
		else {
			word+=str[i];
		}
		i++;
	}
	//***^ temp solution
		srand (time(NULL));
	return str;
  }


  void log_file(std::string str) {
	std::time_t t = std::time(0);
	t -= 1372140000;	//set to seconds since 6/25/2013
	t /= 60;	//mins since 6/25/2013
	int month = ( 7 + t / (60*24*31) )%12;
	int days = 25 + t / (60*24);
	int hours = (t % (60*24))/60;
	int mins = t%60;

	switch (month) {
		case 1:		days = days%32; 
					break;
		case 2:		days = days%29; 
					break;
		case 3:		days = days%32; 
					break;
		case 4:		days = days%31; 
					break;
		case 5:		days = days%32; 
					break;
		case 6:		days = days%31; 
					break;
		case 7:		days = days%32; 
					break;
		case 8:		days = days%32; 
					break;
		case 9:		days = days%31; 
					break;
		case 10:	days = days%32; 
					break;
		case 11:	days = days%31; 
					break;
		case 12:	days = days%32; 
					break;
	}
	if (days == 0 ) days++;

	if (mins < 10 )
		str = " "+to_str(month) + "/" + to_str(days) + " " + to_str(hours) + ":0"+ to_str(mins) + "   " + str;
	else
		str = " "+to_str(month) + "/" + to_str(days) + " " + to_str(hours) + ":"+ to_str(mins) + "   " + str;

	std::ofstream outfile;
	outfile.open("log.txt", std::ios::out | std::ios::binary | std::ios::app);
	outfile << str;
	outfile.close();
  }

};
#endif
