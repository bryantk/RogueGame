#pragma once				
								
class tile {							//	FLAGS			1					2  			3
		unsigned char tile_character;		//	0	128		indestructable		water 		key_locked
		unsigned short int scent_value;		//	0	64		passable			lava 		has_item
		unsigned short int flags1;			//	0	32		blocks_vision		poison   
		unsigned short int flags2;			//	0	16		healthy_scent		has_seen  
		unsigned short int flags3;			//	1	8		silent				 color 
											//	1	4		is_trap				 color 
											//	1	2		has_unit			 color 
											//	1	1		is_door				 color 
	
public:									
  tile() {								
	tile_character = 'Z';
	scent_value = 0;
	flags1 = 0x40;		//defualt to normal
	flags2 = 0;
	flags3 = 0;
  }
  tile(unsigned char t, unsigned char c) {								
	tile_character = t;
	scent_value = 0;
	flags1 = 0x40;		//defualt to normal
	flags2 = 0;
	flags2 |= (c & 0x0F);
	flags3 = 0;
  }
  tile(unsigned char t, unsigned char c, unsigned int f1, unsigned int f2) {								
	tile_character = t;
	scent_value = 0;
	flags1 = f1;		
	flags2 = f2;
	flags2 |= (c & 0x0F);
	flags3 = 0;
  }
  ~tile() {  }
//----SET-------------------------------
  void set_char(unsigned char t) {tile_character = t;}
  void set_scent_value(unsigned short int t) {scent_value = t;}

  void set_color(unsigned char c) {
	flags2 &= 0xF0;
	flags2 |= (c & 0x0F); }

  void set_indestructability(bool t) {
		flags1 &= 0x7F;
		if (t) flags1 |= 128; }
  void set_passability(bool t) {
		flags1 &= 0xBF;
		if (t) flags1 |= 64; }
  void set_blocks_vision(bool t) {
		flags1 &= 0xDF;
		if (t) flags1 |= 32; }
  void set_healthy_scent(bool t) {
		flags1 &= 0xEF;
		if (t) flags1 |= 16; }
  void set_silent(bool t) {
		flags1 &= 0xF7;
		if (t) flags1 |= 8; }
  void set_is_trap(bool t) {
		flags1 &= 0xFB;
		if (t) flags1 |= 4; }
  void set_has_unit(bool t) {
		flags1 &= 0xFD;
		if (t) flags1 |= 2; }
  void set_is_door(bool t) {
		flags1 &= 0xFE;
		if (t) flags1 |= 1; }

  void set_water(bool t) {
		flags2 &= 0x7F;
		if (t) flags2 |= 128; }
  void set_lava(bool t) {
		flags2 &= 0xBF;
		if (t) flags2 |= 64; }
  void set_poison(bool t) {
		flags2 &= 0xDF;
		if (t) flags2 |= 32; }
  void set_has_seen(bool t) {
		flags2 &= 0xEF;
		if (t) flags2 |= 16; }

  void set_key_locked(bool t) {
		flags3 &= 0x7F;
		if (t) flags3 |= 128; }
    void set_has_item(bool t) {
		flags3 &= 0xBF;
		if (t) flags3 |= 64; }
//----GET-------------------------------
  unsigned char get_char() {return tile_character;}
  unsigned short int get_scent_value() {return scent_value;}
  unsigned char get_color() {
	unsigned char temp = 0;
	temp |= (flags2 & 0x0F);
	return temp; }

  bool get_indestructability() const {return flags1 & 128;}
  bool get_passability() const {return flags1 & 64;}
  bool get_blocks_vision() const {return flags1 & 32;}
  bool get_healthy_scent() const {return flags1 & 16;}
  bool get_silent() const {return flags1 & 8;}
  bool get_is_trap() const {return flags1 & 4;}
  bool get_has_unit() const {return flags1 & 2;}
  bool get_is_door() const {return flags1 & 1;}

  bool get_water() const {return flags2 & 128;}
  bool get_lava() const {return flags2 & 64;}
  bool get_poison() const {return flags2 & 32;}
  bool get_has_seen()const  {return flags2 & 16;}

  bool get_key_locked() const {return flags3 & 128;}
  bool get_has_item() const {return flags3 & 64;}
};
