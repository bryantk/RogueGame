
#pragma once

#include <vector>
#include <string>
#include <sstream>


//***************************
//  Opens 'file' (from main directory) and returns vector of tokens from start::'location' to 'location'::end
//***************************
  std::vector<std::string> tokens(std::string file, std::string location) {
	std::vector<std::string> temp;
	std::string word = "";
	std::ifstream readFile;
	readFile.open(file.c_str());
	if (readFile.is_open()) {
		while (!readFile.eof()) {	// while there is still data
			readFile >> word;
			if ( word == ("start::"+location) ) {
				readFile >> word;
				//temp.push_back("Found:");
				while (word != (location+"::end") ) {
					if (word != "") {
						if (word[0] == '\"') {
							std::string sentence = word;
							while (word[word.size()-1] != '\"') {
								readFile >> word;
								sentence += " " + word;
							}
							sentence.erase(0,1);
							sentence.erase(sentence.end()-1);
							temp.push_back(sentence);
							word = "";
						}
						if (word[0] == '#' && word[1] == '#' && word.size() > 2) {	// comment word
							readFile >> word;
							continue;
						}
						if (word[word.size()-1]=='.' && word.size() != 1) {
							word.erase(word.size()-1);
							temp.push_back(word);
							temp.push_back(".");
						}
						else if (word[word.size()-1]==',' && word.size() != 1) {
							word.erase(word.size()-1);
							temp.push_back(word);
							temp.push_back(",");
						}
						else
							if (word != "") temp.push_back(word);
					}
					readFile >> word;
					if (readFile.eof()) return temp;
				}
			}
			word = "";
		}
	}
	readFile.close();
	return temp;
  }
