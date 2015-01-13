#pragma once
#ifndef MAP_DATA_H
#define MAP_DATA_H

#include <vector>
#include <string>
#include <sstream>

#include <cmath>

#include "tile.h"
//#include "../constants/constants.h"

class map_data {

public:

	//current map data
	int map_id, map_y, map_x;
	tile map[256][256];


  map_data() {
	map_y = 0;
	map_x = 0;

	for (int i = 0; i<256; i++) {	for (int j = 0; j<256; j++) {
			map[i][j] = tile('.',4);
			if (j == 255 || i == 255 || i == 0 || j == 0) {
				map[i][j] = tile('!',12);
				map[i][j].set_passability(false); 
				map[i][j].set_indestructability(true); 
			}
	}	}


  }

  ~map_data() {
	//Clean up data usage / save?
  }

};
#endif
