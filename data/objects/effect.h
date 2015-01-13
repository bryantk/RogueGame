#pragma once
#ifndef EFFECT_H
#define EFFECT_H

#include <string>

//	FLAGS
//
//	128		overrides 	lower priority effects
//	64		high_priority_status							
//	32		equipment		based effect		
//	16		status_effect
//	8		presists		must be cured specificaly
//	4		genetic	
//	2		additive_str ength
//	1		additive_dur ation

class effect {
	std::string tag;
	unsigned char flags;
public:
	unsigned char priority;
	int strength;
	unsigned char range;
	char duration;

  effect(unsigned char pri, std::string s, int str, unsigned char r, unsigned char d) {
	priority = pri;
	tag = s;
	strength=str;
	range = r;
	duration = d;
	flags = 0;
  }
   effect(unsigned char pri, std::string s, int str, unsigned char r, unsigned char d, unsigned char f) {
	priority = pri;
	tag = s;
	strength=str;
	range = r;
	duration = d;
	flags = f;
  }

  ~effect() {
	//Clean up data usage / save?
  }
  void set_strength(int i) {
	strength = i;
  }
  void add_strength(int i) {
	strength+= i;
  }

  std::string get_tag() { return tag; }

   void overrides(bool t) {				//	F		E		D		B		5		0
		flags &= 0x5F;					//	1111		1110		1101		1011		0111		0000
		if (t) flags |= 128; }
  void high_priority_status(bool t) {
		flags &= 0xBF;
		if (t) flags |= 64; }
  void equipment(bool t) {
		flags &= 0xDF;
		if (t) flags |= 32; }
  void status_effect(bool t) {
		flags &= 0xEF;
		if (t) flags |= 16; }
  void presists(bool t) {
		flags &= 0xF5;
		if (t) flags |= 8; }
  void genetic(bool t) {
		flags &= 0xFB;
		if (t) flags |= 4; }
  void additive_str(bool t) {
		flags &= 0xFD;
		if (t) flags |= 2; }
  void additive_dur(bool t) {
		flags &= 0xFE;
		if (t) flags |= 1; }

  bool overrides() const {return flags & 128;}
  bool high_priority_status() const {return flags & 64;}
  bool equipment() const {return flags & 32;}
  bool status_effect() const {return flags & 16;}
  bool presists() const {return flags & 8;}
  bool genetic() const {return flags & 4;}
  bool additive_str() const {return flags & 2;}
  bool additive_dur() const {return flags & 1;}


};
#endif
