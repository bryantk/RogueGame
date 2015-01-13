
#include "data/data.h"
#include "data/map_data/map_data.h"

#include <fstream>
#include <string>
#include <sstream>

using namespace std;

void TEMP_load_map(map_data& DATA) {
	string word = "";
	ifstream readFile;
	readFile.open("maps/map1.map");
	if (readFile.is_open()) {
	int y = 0;
		while (!readFile.eof()) {	// while there is still data
			readFile >> word;
			int x = 0;
			while (word[x]!=0) {
				switch (word[x]-48) {
					case 0:
						DATA.map[y][x] = tile('.',0);
					break;
					case 1:
						DATA.map[y][x] = tile(' ',9);
						DATA.map[y][x].set_blocks_vision(true);
						DATA.map[y][x].set_passability(false);
					break;
					case 2:
						DATA.map[y][x] = tile('_',3);
						DATA.map[y][x].set_is_door(true);
					break;
					case 3:
						DATA.map[y][x] = tile('/',3);
						DATA.map[y][x].set_blocks_vision(true);
						DATA.map[y][x].set_is_door(true);
						DATA.map[y][x].set_passability(false);
					break;
					case 7:
						DATA.map[y][x] = tile('*',3);
					break;
					default:	
						DATA.map[y][x] = tile('.',4);
				}
				x++;
			}		
			y++;
		}
		word = "";
	}

	readFile.close();
  }
