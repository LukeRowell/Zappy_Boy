#pragma once
#ifndef TILE_H
#define TILE_H

#include "MMU.h"
#include "definitions.h"

class MMU;

class Tile
{
	public:

		Tile(unsigned short& address, MMU& mmu, unsigned int size_multiplier = 1);

		GBColor get_pixel(unsigned int x, unsigned int y) const;

	private:

		static unsigned int pixel_index(unsigned int x, unsigned int y);
		std::vector<unsigned char> get_pixel_line(unsigned char byte1, unsigned char byte2) const;

		GBColor buffer[TILE_HEIGHT_PX * 2 * TILE_WIDTH_PX];
};

#endif