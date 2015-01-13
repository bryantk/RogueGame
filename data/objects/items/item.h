#pragma once				
#include <string>	
#include <sstream>
#include <vector>	
#include <cmath>

#include "../effect.h"
#include "../../../open.h"
class item {							//	FLAGS			1					2  
		unsigned int id;
		std::string name;
		std::string description;
		char type;						//(S)croll,	(P)otion, (t)hrowing item, Armor, Weapon, Ring, Neck, other... etc
		unsigned int value;
		unsigned char weight;			// divide by 10 for displaed value
		int y;
		int x;
		int rarity;

		int knowledge_of;

		std::vector<effect> effects;
										//Flags		0		0			0			0		|	0			0			0			0	
		unsigned short int flags;		//			head	neck		chest		l.hand	|	r.hand		ring		Cursed		Known

public:									
  item() {	
	id = 0;							
	name = "None";
	description = "There is nothing here.";
	value = 0;
	weight = 0;
	type = 'f';
	y = -1;
	x = -1;
	knowledge_of = 0;
	flags = 0;
  }

// this constructor is not needed... is only supporting a "firebolt" spellbook framework...
  item(unsigned int id_tag, std::string n, std::string d, char t, unsigned int v, unsigned char w, int y_loc, int x_loc) {	
	id = id_tag;							
	name = n;
	description = d;
	type = t;
	value = v;
	weight = w;
	y = y_loc;
	x = x_loc;
	flags = 0;
  }


  item(std::string tag, bool is_known=false) {
	y = 0;
	x = 0;
	knowledge_of = 0;
	std::vector<std::string> dump = tokens("data/bin/data2.dat", ("item"+tag) );
	id = atoi(tag.c_str());
	flags = 0;
	name = dump[0];
	description = dump[1];
	type = dump[2].at(0);
	value = atoi(dump[3].c_str() );
	weight = atoi(dump[4].c_str() );
	if ( rand()%101 <= atoi(dump[5].c_str()) ) is_cursed(true);
	rarity = atoi(dump[6].c_str() );
	int i = 7;

	while (dump[i] != "#effects") {
		if (dump[i] == "head") is_head(true);
		if (dump[i] == "neck") is_neck(true);
		if (dump[i] == "chest") is_chest(true);
		if (dump[i] == "arm") is_arm(true);
		if (dump[i] == "both") is_both(true);
		if (dump[i] == "ring") is_ring(true);
		if (dump[i] == "cursed") is_cursed(true);
		if (dump[i] == "known") flags |= 1;
		i++;
	}
	i++;
	while (dump[i] != "#end") {
		unsigned char flags = atoi(dump[i+5].c_str() );
		flags = 0;
		if (dump[i+5].size() == 9) {
			flags = 0;
			if (dump[i+5].at(0) == '1') flags |= 128;
			if (dump[i+5].at(1) == '1') flags |= 64;
			if (dump[i+5].at(2) == '1') flags |= 32;
			if (dump[i+5].at(3) == '1') flags |= 16;

			if (dump[i+5].at(5) == '1') flags |= 8;
			if (dump[i+5].at(6) == '1') flags |= 4;
			if (dump[i+5].at(7) == '1') flags |= 2;
			if (dump[i+5].at(8) == '1') flags |= 1;
		}
		add_effect( effect(atoi(dump[i].c_str() ), dump[i+1], atoi(dump[i+2].c_str() ), atoi(dump[i+3].c_str() ), atoi(dump[i+4].c_str() ), flags ) );
		i+= 6;	
	}
	if ( is_cursed() ) {		//***^	waht negatives are applied to cursed equipment?
		remove_effect("enchanted");
		remove_effect("boost");
		int random = rand()%5*-1;
 		add_effect( effect( 12, "enchanted", random, 0, -1, 0 ) );
		add_effect( effect( 12, "boost", 75, 0, -1, 0 ) );
	}

  }

  ~item() {  }

  std::string to_str (int in) {std::ostringstream stream;	stream << in;	return stream.str();	} 

  void set_location(const int& new_y, const int& new_x) {
	y = new_y;
	x = new_x;
  }

  bool check_location(const int& dy, const int& dx){
	if (dy == y && dx == x) return true;
	return false;
  }
/*
head	128
neck	64
chest	32
left	16
right	8
ring	4
cursed	2
known	1
*/

  void is_head(bool t) {				//	F		E		D		B		5		0
		flags &= 0x5F;				//	1111		1110		1101		1011		0111		0000
		if (t) flags |= 128; }
  void is_neck(bool t) {
		flags &= 0xBF;
		if (t) flags |= 64; }
  void is_chest(bool t) {
		flags &= 0xDF;
		if (t) flags |= 32; }
  void is_arm(bool t) {
		flags &= 0xEF;
		if (t) flags |= 16; }
  void is_both(bool t) {
		flags &= 0xF5;
		if (t) flags |= 8; }
  void is_ring(bool t) {
		flags &= 0xFB;
		if (t) flags |= 4; }
  void is_cursed(bool t) {
		flags &= 0xFD;
		if (t) flags |= 2; }
  void is_known(bool t) {
		flags &= 0xFE;
		if (t) flags |= 1; }	



//***^
//
//	TO DO: tick_effects, add_effect,
//
//***^

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

  bool remove_effect(std::string tag) {
	for (int i = 0; i < effects.size(); i++) {
		if (effects[i].get_tag() == tag) {
			effects.erase(effects.begin()+i);
			return true; 
		}
	}
	return false;
  }

  void decurse() {
	is_known(true);
	is_cursed(false);
	remove_effect("enchanted");
	remove_effect("boost");
	
  }

  bool shared_name() {
	if (type == 'p' || type == 's' ) return true;
	return false;
  }

  bool find_effect(std::string tag) {
	for (int i = 0; i < effects.size(); i++) {
		if (effects[i].get_tag() == tag) return true; 
	}
	return false;
  }

  int find_effect_index(std::string tag) {
	for (int i = 0; i < effects.size(); i++) {
		if (effects[i].get_tag() == tag) return i; 
	}
	return -1;
  }

  int find_effect_strength(std::string tag) {
	for (int i = 0; i < effects.size(); i++) {
		if (effects[i].get_tag() == tag) return effects[i].strength; 
	}
	return 0;
  }

  int find_effect_strength_w_bonus(std::string tag) {		//gross function i made to add bonuses to "compare" function
	for (int i = 0; i < effects.size(); i++) {
		if (effects[i].get_tag() == tag)  {
			float multi = 1;
			if ( find_effect("boost") )
				multi = float(find_effect_strength("boost")/float(100) );
				int return_val = effects[i].strength+find_effect_strength("enchanted");
				return_val = float(return_val) * multi;
			return (return_val); 
		}
	}
	return 0;
  }

  int effects_size() {
	return effects.size();
  }

  effect& get_effect(int i) {
	return effects[i];
  }

  void add_knowledge_of(int i) { knowledge_of+=i; }

//----GET-------------------------------
  int get_knowledge_of() { return knowledge_of; }
  bool is_equipment() const {return flags & 0xFC;}

  bool is_head() const {return flags & 128;}
  bool is_neck() const {return flags & 64;}
  bool is_chest() const {return flags & 32;}
  bool is_arm() const {return flags & 16;}
  bool is_both() const {return flags & 8;}
  bool is_ring() const {return flags & 4;}
  bool is_cursed() const {return flags & 2;}
  bool is_known() const {return flags & 1;}	

  int get_id() {return id;}
  std::string get_name() {return name;}
  std::string get_description() {
	return description;
  }
  void set_name(std::string new_name) {	name = new_name;}
  unsigned int get_value() {return value;}
  unsigned char get_weight() {return weight;}
  std::string get_type_name() {
	if (type == 'p') return "Potion";
	if (type == 's') return "Scroll";
	return "NULL";
  }
  char get_display_char() {
	//if (type == 'a') return 'a';
		return '>';
  	}
  char get_type() {
	return type;
  }
  int get_color() {
	return 0;
  }
  void get_location(int& item_y, int& item_x) {
	item_y = y;
	item_x = x;
	return;
  }
  std::vector<effect> get_equ_effects() {
	std::vector<effect> list;
	int bonus = find_effect_strength("enchanted");
	float multi = 1;
	if ( find_effect("boost") )
		multi = float(find_effect_strength("boost")/float(100) );
	for (int i = 0; i < effects.size(); i++) {
		if (effects[i].equipment() ) {
			effect temp = effects[i];
			temp.strength = float(temp.strength) * multi;	// multi bonus first	than addition
			temp.strength += bonus;
			list.push_back(temp); 
		}
	}
	return list;
  }

  std::string effect_desc() {
	std::string str = "";

	int bonus = find_effect_strength("enchanted");
	float multi = 1;
	if ( find_effect("boost") )
		multi = float(find_effect_strength("boost")/float(100) );
	for (int i = 0; i < effects.size(); i++) {
		if (effects[i].equipment() ) {
			str += effects[i].get_tag().substr(4);;
			int number = float(effects[i].strength) * multi + bonus;
			if (number > 0 ) str+="|c4+" + to_str(number)+"|c0"; else if (number < 0) str += "|c2"+to_str(number)+"|c0";		// |c## is causing problems
			str += " ";
		}
	}
	return str;
  }

  void grab_minuses(item ITEM, int (&array)[11]) {
	array[0] -= ITEM.find_effect_strength_w_bonus("equ_HP");
	array[1] -= ITEM.find_effect_strength_w_bonus("equ_STR");
	array[2] -= ITEM.find_effect_strength_w_bonus("equ_DEF");
	array[3] -= ITEM.find_effect_strength_w_bonus("equ_WIS");	
	array[4] -= ITEM.find_effect_strength_w_bonus("equ_INT");
	array[5] -= ITEM.find_effect_strength_w_bonus("equ_Vis");
	array[6] -= ITEM.find_effect_strength_w_bonus("equ_Eva");

	array[7] -= ITEM.find_effect_strength_w_bonus("equ_Fire_res");
	array[8] -= ITEM.find_effect_strength_w_bonus("equ_Water_res");
	array[9] -= ITEM.find_effect_strength_w_bonus("equ_Earth_res");
	array[10] -= ITEM.find_effect_strength_w_bonus("equ_Air_res");
	return;
  }

// compares calling item to item in function	
// ex. item_str+3(item_NULL)  returns  3
// calling item will replace in function item
  void compare(item ITEM, int (&array)[11]) {		
	array[0] = find_effect_strength_w_bonus("equ_HP");
	array[0] -= ITEM.find_effect_strength_w_bonus("equ_HP");
	array[1] = find_effect_strength_w_bonus("equ_STR");
	array[1] -= ITEM.find_effect_strength_w_bonus("equ_STR");
	array[2] = find_effect_strength_w_bonus("equ_DEF");
	array[2] -= ITEM.find_effect_strength_w_bonus("equ_DEF");
	array[3] = find_effect_strength_w_bonus("equ_WIS");
	array[3] -= ITEM.find_effect_strength_w_bonus("equ_WIS");	
	array[4] = find_effect_strength_w_bonus("equ_INT");
	array[4] -= ITEM.find_effect_strength_w_bonus("equ_INT");
	array[5] = find_effect_strength_w_bonus("equ_Vis");
	array[5] -= ITEM.find_effect_strength_w_bonus("equ_Vis");
	array[6] = find_effect_strength_w_bonus("equ_Eva");
	array[6] -= ITEM.find_effect_strength_w_bonus("equ_Eva");

	array[7] = find_effect_strength_w_bonus("equ_Fire_res");
	array[7] -= ITEM.find_effect_strength_w_bonus("equ_Fire_res");
	array[8] = find_effect_strength_w_bonus("equ_Water_res");
	array[8] -= ITEM.find_effect_strength_w_bonus("equ_Water_res");
	array[9] = find_effect_strength_w_bonus("equ_Earth_res");
	array[9] -= ITEM.find_effect_strength_w_bonus("equ_Earth_res");
	array[10] = find_effect_strength_w_bonus("equ_Air_res");
	array[10] -= ITEM.find_effect_strength_w_bonus("equ_Air_res");
	
	return;
  }


};
