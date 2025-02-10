#include "Tile.h"
#include "Color.h"
#include "bitwise.h"

Tile::Tile(unsigned short& tile_address, MMU& mmu, unsigned int size_multiplier)
{
	for (unsigned int x = 0; x < TILE_WIDTH_PX; x++)
	{
		for (unsigned int y = 0; y < TILE_HEIGHT_PX * size_multiplier; y++)
		{
			buffer[pixel_index(x, y)] = GBColor::Color0;
		}
	}

	for (unsigned int tile_line = 0; tile_line < TILE_HEIGHT_PX * size_multiplier; tile_line++)
	{
		unsigned int index_into_tile = 2 * tile_line;
		unsigned short line_start = tile_address + index_into_tile;

		unsigned char pixels_1 = mmu.readMemory(line_start);
		unsigned char pixels_2 = mmu.readMemory(line_start + 1);

		std::vector<unsigned char> pixel_line = get_pixel_line(pixels_1, pixels_2);

		for (unsigned int x = 0; x < TILE_WIDTH_PX; x++)
		{
			buffer[pixel_index(x, tile_line)] = get_color(pixel_line[x]);
		}
	}
}

GBColor Tile::get_pixel(unsigned int x, unsigned int y) const
{
	return buffer[pixel_index(x, y)];
}

std::vector<unsigned char> Tile::get_pixel_line(unsigned char byte1, unsigned char byte2) const
{
	std::vector<unsigned char> pixel_line;

	for (unsigned char i = 0; i < 8; i++)
	{
		unsigned char color_value = static_cast<unsigned char>((bitwise::bit_value(byte2, 7 - i) << 1) | bitwise::bit_value(byte1, 7 - i));
		pixel_line.push_back(color_value);
	}

	return pixel_line;
}

inline unsigned int Tile::pixel_index(unsigned int x, unsigned int y)
{
	return (y * TILE_HEIGHT_PX) + x;
}