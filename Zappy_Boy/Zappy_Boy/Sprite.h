#pragma once
#ifndef SPRITE_H
#define SPRITE_H

#include "bitwise.h"

class Sprite
{
	public:

		unsigned char yPos;
		unsigned char xPos;
		unsigned short tileIndex;
		unsigned char attributeFlags;

		bool priority = bitwise::check_bit(attributeFlags, 7);
		bool yFlip = bitwise::check_bit(attributeFlags, 6);
		bool xFlip = bitwise::check_bit(attributeFlags, 5);
		bool dmgPalette = bitwise::check_bit(attributeFlags, 4);
		
	private:
};

#endif SPRITE_H