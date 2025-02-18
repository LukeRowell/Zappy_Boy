#pragma once
#ifndef PPU_H
#define PPU_H

#include <functional>
#include <SFML/Graphics.hpp>
#include "CPU.h"
#include "MMU.h"
#include "buffer.h"
#include "Tile.h"

class GameBoy;
class CPU;
class MMU;
enum class GBColor;

struct Palette
{
	sf::Color color0;
	sf::Color color1;
	sf::Color color2;
	sf::Color color3;
};

class PPU
{
	public:

		PPU(CPU &zbCPU, MMU &zbMMU);
		~PPU();

		//void setupDrawFunction(std::function<void(std::vector<sf::Color>&)> drawFunc);
		void setupDrawFunction(std::function<void(const Buffer &buffer)> drawFunc);

		Palette loadPalette();

		Palette loadSpritePalette(bool use_palette_1);

		int getMode();

		sf::Color getActualColor(unsigned char colorValue);

		sf::Color getColor(unsigned char colorNum);

		sf::Color get_color_from_palette(GBColor gb_color, const Palette& palette);

		inline unsigned char getBit(const unsigned char byte, const unsigned char bitIndex);

		unsigned char get_pixel(unsigned char byte1, unsigned char byte2, unsigned char bitIndex) const;

		void drawBackground();

		void drawWindow();

		void drawSprites();

		void tick(int elapsedCycles);
		unsigned char line = 0x00;

		sf::Color callGetPixel(unsigned int x, unsigned int y) const;
		inline sf::Color getPixel(unsigned int x, unsigned int y) const;

		int getCycleCount() { return cycleCount; }

		std::vector<unsigned char> getTile(int tileIndex);

	private:

		int PPU_Mode;
		unsigned int cycleCount = 0;

		unsigned int x_in_bg_map, y_in_bg_map;
		unsigned int tile_x, tile_y;
		unsigned int x_in_tile, y_in_tile;
		unsigned int tile_index;
		unsigned short tile_id_address;
		unsigned char tile_id;
		unsigned int tile_offset;
		unsigned int index_into_tile;
		unsigned short tile_start, tile_line_start;
		unsigned char firstPixels, secondPixels;
		sf::Color colorForPixel;

		unsigned short offset_in_oam;
		unsigned short oam_start;

		bool LCDEnabled;
		bool windowTileSelect;
		bool windowDisplayEnabled;
		bool backgroundAndWindowTileSelect;
		bool backgroundTileSelect;
		bool largeSpriteSize;
		bool spritesEnabled;
		bool backgroundDisplayEnabled;
		unsigned int current_line;
		unsigned int scx;
		unsigned int scy;
		unsigned int wx;
		unsigned int wy;

		unsigned short map_start_addr;
		unsigned short tile_start_addr;
		unsigned short tile_addr;
		unsigned short tile_set_start_addr;
		unsigned short tile_line_start_addr;

		Palette palette;
		
		//std::vector<sf::Color> pixels;
		//sf::Color pixels[23040];

		Buffer buffer;

		//std::function<void(std::vector<sf::Color>&)> draw;
		std::function<void(const Buffer &buffer)> draw;

		CPU& cpu;
		MMU& mmu;
};

#endif