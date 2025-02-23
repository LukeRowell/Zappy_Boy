#include "PPU.h"
#include "GameBoy.h"
#include "bitwise.h"

PPU::PPU(CPU &zbCPU, MMU &zbMMU) : cpu(zbCPU), mmu(zbMMU), buffer(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	PPU_Mode = 0;
	line = 0x00;
}

PPU::~PPU()
{

}


//void PPU::setupDrawFunction(std::function<void(std::vector<sf::Color>&)> drawFunc)
void PPU::setupDrawFunction(std::function<void(const Buffer &buffer)> drawFunc)
{
	draw = drawFunc;
}

Palette PPU::loadPalette()
{
	Palette currentPalette;
	unsigned char color3bits = *(mmu.BGP) & 0xC0;
	unsigned char color2bits = *(mmu.BGP) & 0x30;
	unsigned char color1bits = *(mmu.BGP) & 0x0C;
	unsigned char color0bits = *(mmu.BGP) & 0x03;

	switch (color3bits)
	{
		case 0x00:
			currentPalette.color3 = sf::Color::White;
			break;

		case 0x40:
			currentPalette.color3 = sf::Color(170, 170, 170);		//light gray
			break;

		case 0x80:
			currentPalette.color3 = sf::Color(85, 85, 85);		//dark gray
			break;

		case 0xC0:
			currentPalette.color3 = sf::Color::Black;
			break;

		default:
			break;
	}

	switch (color2bits)
	{
		case 0x00:
			currentPalette.color2 = sf::Color::White;
			break;

		case 0x10:
			currentPalette.color2 = sf::Color(170, 170, 170);		//light gray
			break;

		case 0x20:
			currentPalette.color2 = sf::Color(85, 85, 85);		//dark gray
			break;

		case 0x30:
			currentPalette.color2 = sf::Color::Black;
			break;

		default:
			break;
	}

	switch (color1bits)
	{
		case 0x00:
			currentPalette.color1 = sf::Color::White;
			break;

		case 0x04:
			currentPalette.color1 = sf::Color(170, 170, 170);		//light gray
			break;

		case 0x08:
			currentPalette.color1 = sf::Color(85, 85, 85);		//dark gray
			break;

		case 0x0C:
			currentPalette.color1 = sf::Color::Black;
			break;

		default:
			break;
	}

	switch (color0bits)
	{
		case 0x00:
			currentPalette.color0 = sf::Color::White;		//White
			break;

		case 0x01:
			currentPalette.color0 = sf::Color(170, 170, 170);		//light gray
			break;

		case 0x02:
			currentPalette.color0 = sf::Color(85, 85, 85);		//dark gray
			break;

		case 0x03:
			currentPalette.color0 = sf::Color::Black;		//Black
			break;

		default:
			break;
	}

	return currentPalette;
}

Palette PPU::loadSpritePalette(bool use_palette_1)
{
	Palette spritePalette;

	if (use_palette_1)
	{
		unsigned char color0 = bitwise::compose_bits(bitwise::bit_value(*(mmu.OBP1), 1), bitwise::bit_value(*(mmu.OBP1), 0));
		unsigned char color1 = bitwise::compose_bits(bitwise::bit_value(*(mmu.OBP1), 3), bitwise::bit_value(*(mmu.OBP1), 2));
		unsigned char color2 = bitwise::compose_bits(bitwise::bit_value(*(mmu.OBP1), 5), bitwise::bit_value(*(mmu.OBP1), 4));
		unsigned char color3 = bitwise::compose_bits(bitwise::bit_value(*(mmu.OBP1), 7), bitwise::bit_value(*(mmu.OBP1), 6));

		spritePalette.color0 = getActualColor(color0);
		spritePalette.color1 = getActualColor(color1);
		spritePalette.color2 = getActualColor(color2);
		spritePalette.color3 = getActualColor(color3);
	}

	else
	{
		unsigned char color0 = bitwise::compose_bits(bitwise::bit_value(*(mmu.OBP0), 1), bitwise::bit_value(*(mmu.OBP0), 0));
		unsigned char color1 = bitwise::compose_bits(bitwise::bit_value(*(mmu.OBP0), 3), bitwise::bit_value(*(mmu.OBP0), 2));
		unsigned char color2 = bitwise::compose_bits(bitwise::bit_value(*(mmu.OBP0), 5), bitwise::bit_value(*(mmu.OBP0), 4));
		unsigned char color3 = bitwise::compose_bits(bitwise::bit_value(*(mmu.OBP0), 7), bitwise::bit_value(*(mmu.OBP0), 6));

		spritePalette.color0 = getActualColor(color0);
		spritePalette.color1 = getActualColor(color1);
		spritePalette.color2 = getActualColor(color2);
		spritePalette.color3 = getActualColor(color3);
	}

	return spritePalette;
}

int PPU::getMode()
{
	return PPU_Mode;
}

sf::Color PPU::getActualColor(unsigned char colorValue)
{
	switch (colorValue)
	{
		case 0:
			//return sf::Color::Green;
			return sf::Color::White;
			break;

		case 1:
			return sf::Color(170, 170, 170);
			//return sf::Color::Red;
			break;

		case 2:
			return sf::Color(85, 85, 85);
			//return sf::Color::Blue;
			break;

		case 3:
			//return sf::Color::Yellow;
			return sf::Color::Black;
			break;

		default:
			break;
	}
}

sf::Color PPU::getColor(unsigned char colorNum)
{
	switch (colorNum)
	{
		case 0x00:
			return palette.color0;
			break;

		case 0x01:
			return palette.color1;
			break;

		case 0x02:
			return palette.color2;
			break;

		case 0x03:
			return palette.color3;
			break;

		default:
			//return sf::Color::Red;
			break;
	}
}

sf::Color PPU::get_color_from_palette(GBColor color, const Palette& palette)
{
	switch (color)
	{
		case GBColor::Color0:
			return palette.color0;
			break;

		case GBColor::Color1:
			return palette.color1;
			break;

		case GBColor::Color2:
			return palette.color2;
			break;

		case GBColor::Color3:
			return palette.color3;
			break;

		default:
			break;
	}
}

inline unsigned char PPU::getBit(const unsigned char byte, const unsigned char bitIndex)
{
	return (byte >> bitIndex) & 1;
}

unsigned char PPU::get_pixel(unsigned char byte1, unsigned char byte2, unsigned char bitIndex) const
{
	using bitwise::bit_value;

	return static_cast<unsigned char>((bit_value(byte2, 7 - bitIndex) << 1) | bit_value(byte1, 7 - bitIndex));
}

void PPU::drawBackground()
{
	LCDEnabled = *(mmu.LCDC) & 0x80;

	if (!LCDEnabled)		//If the LCD is off we cannot draw to the screen
	{
		return;
	}

	windowTileSelect = *(mmu.LCDC) & 0x40;
	windowDisplayEnabled = *(mmu.LCDC) & 0x20;
	largeSpriteSize = *(mmu.LCDC) & 0x04;
	spritesEnabled = *(mmu.LCDC) & 0x02;
	backgroundDisplayEnabled = *(mmu.LCDC) & 0x01;

	if (backgroundDisplayEnabled)
	{
		backgroundAndWindowTileSelect = *(mmu.LCDC) & 0x10;
		backgroundTileSelect = *(mmu.LCDC) & 0x08;
		backgroundTileSelect ? map_start_addr = 0x9C00 : map_start_addr = 0x9800;
		backgroundAndWindowTileSelect ? tile_start_addr = 0x8000 : tile_start_addr = 0x8800;
		palette = loadPalette();
		scx = *(mmu.SCX);
		scy = *(mmu.SCY);
		current_line = line;

		for (unsigned int x = 0; x < SCREEN_WIDTH; x++)
		{
			x_in_bg_map = (scx + x) % BACKGROUND_SIZE;
			y_in_bg_map = (scy + current_line) % BACKGROUND_SIZE;

			tile_x = x_in_bg_map / TILE_WIDTH_PX;
			tile_y = y_in_bg_map / TILE_HEIGHT_PX;

			x_in_tile = x_in_bg_map % TILE_WIDTH_PX;
			y_in_tile = y_in_bg_map % TILE_HEIGHT_PX;

			tile_index = (tile_y * TILES_PER_LINE) + tile_x;

			tile_id_address = map_start_addr + tile_index;

			tile_id = mmu.read(tile_id_address);

			//backgroundTileSelect ? tile_offset = tile_id * TILE_BYTES : tile_offset = ((signed char)tile_id + 128) * TILE_BYTES;
			//backgroundTileSelect ? tile_offset = ((signed char)tile_id + 128) * TILE_BYTES : tile_offset = tile_id * TILE_BYTES;

			backgroundAndWindowTileSelect ? tile_offset = tile_id * TILE_BYTES : tile_offset = ((signed char)tile_id + 128) * TILE_BYTES;
			//backgroundAndWindowTileSelect ? tile_offset = ((signed char)tile_id + 128) * TILE_BYTES : tile_offset = tile_id * TILE_BYTES;

			index_into_tile = y_in_tile * 2;

			tile_start = tile_start_addr + tile_offset;
			tile_line_start = tile_start_addr + tile_offset + index_into_tile;

			firstPixels = mmu.read(tile_line_start);
			secondPixels = mmu.read(tile_line_start + 1);

			colorForPixel = getColor(get_pixel(firstPixels, secondPixels, x_in_tile));
			//colorForPixel = sf::Color::Red;

			buffer.set_pixel(x, current_line, colorForPixel);
			//pixels[(current_line * SCREEN_WIDTH) + x] = colorForPixel;
		}
	}
}

void PPU::drawWindow()
{
	LCDEnabled = *(mmu.LCDC) & 0x80;
	windowTileSelect = *(mmu.LCDC) & 0x40;
	windowDisplayEnabled = *(mmu.LCDC) & 0x20;
	backgroundAndWindowTileSelect = *(mmu.LCDC) & 0x10;
	backgroundTileSelect = *(mmu.LCDC) & 0x08;

	if (!LCDEnabled)		//If the LCD is off we cannot draw to the screen
	{
		return;
	}

	if (windowDisplayEnabled)
	{
		windowTileSelect ? map_start_addr = 0x9C00 : map_start_addr = 0x9800;
		backgroundAndWindowTileSelect ? tile_start_addr = 0x8000 : tile_start_addr = 0x8800;
		palette = loadPalette();
		wx = *(mmu.WX);
		wy = *(mmu.WY);
		current_line = line;

		unsigned int y_in_screen = current_line - wy;

		if (y_in_screen > SCREEN_HEIGHT)
		{
			return;
		}

		for (unsigned int x = 0; x < SCREEN_WIDTH; x++)
		{
			unsigned int x_in_screen = wx + x - 7;

			tile_x = x_in_screen / TILE_WIDTH_PX;
			tile_y = y_in_screen / TILE_HEIGHT_PX;

			x_in_tile = x_in_screen % TILE_WIDTH_PX;
			y_in_tile = y_in_screen % TILE_HEIGHT_PX;

			tile_index = tile_y * TILES_PER_LINE + tile_x;

			tile_id_address = map_start_addr + tile_index;

			tile_id = mmu.read(tile_id_address);

			backgroundAndWindowTileSelect ? tile_offset = tile_id * TILE_BYTES : tile_offset = ((signed char)tile_id + 128) * TILE_BYTES;

			index_into_tile = y_in_tile * 2;

			tile_start = tile_start_addr + tile_offset;
			tile_line_start = tile_start_addr + tile_offset + index_into_tile;

			firstPixels = mmu.read(tile_line_start);
			secondPixels = mmu.read(tile_line_start + 1);

			colorForPixel = getColor(get_pixel(firstPixels, secondPixels, x_in_tile));

			buffer.set_pixel(x, current_line, colorForPixel);
		}
	}
}

void PPU::drawSprites()
{
	Palette spritePalette;

	for (int i = 0; i < 40; i++)		//40 total sprites
	{
		offset_in_oam = i * SPRITE_BYTES;
		oam_start = 0xFE00 + offset_in_oam;

		unsigned char sprite_y = mmu.read(oam_start);
		unsigned char sprite_x = mmu.read(oam_start + 1);

		if (sprite_y == 0 || sprite_y >= 160)
		{
			continue;
		}

		if (sprite_x == 0 || sprite_x >= 168)
		{
			continue;
		}

		//unsigned int sprite_size_multiplier = sprite_size() ? 2 : 1;

		unsigned short tile_set_location = 0x8000;

		unsigned char pattern_i = mmu.read(oam_start + 2);
		unsigned char sprite_attrs = mmu.read(oam_start + 3);

		bool use_palette_1 = bitwise::check_bit(sprite_attrs, 4);
		bool flip_x = bitwise::check_bit(sprite_attrs, 5);
		bool flip_y = bitwise::check_bit(sprite_attrs, 6);
		bool obj_behind_bg = bitwise::check_bit(sprite_attrs, 7);

		spritePalette = loadSpritePalette(use_palette_1);

		unsigned short tile_offset = pattern_i * TILE_BYTES;

		unsigned short pattern_address = tile_set_location + tile_offset;

		Tile tile(pattern_address, mmu, 1);

		int start_y = sprite_y - 16;
		int start_x = sprite_x - 8;

		for (unsigned int y = 0; y < TILE_HEIGHT_PX; y++)
		{
			for (unsigned int x = 0; x < TILE_WIDTH_PX; x++)
			{
				unsigned int maybe_flipped_y = !flip_y ? y : (TILE_HEIGHT_PX * 1) - y - 1;
				unsigned int maybe_flipped_x = !flip_x ? x : TILE_WIDTH_PX - x - 1;

				GBColor gb_color = tile.get_pixel(maybe_flipped_x, maybe_flipped_y);

				if (gb_color == GBColor::Color0)
				{
					continue;
				}

				sf::Color screen_color = get_color_from_palette(gb_color, spritePalette);

				int pixel_x = start_x + x;
				int pixel_y = start_y + y;

				if (pixel_x < 0 || pixel_x > SCREEN_WIDTH)
				{
					continue;
				}

				if (pixel_y < 0 || pixel_y > SCREEN_HEIGHT)
				{
					continue;
				}

				buffer.set_pixel(pixel_x, pixel_y, screen_color);
			}
		}
	}
}

std::vector<unsigned char> PPU::getTile(int tileIndex)
{
	std::vector<unsigned char> tileData;
	bool isBaseAddressingMode = bitwise::check_bit(*(mmu.LCDC), 4);
	unsigned short startAddr = isBaseAddressingMode ? 0x8000 + (tileIndex * 16) : 0x9000 + (0 * 16);

	for (int i = 0; i < 16; i++)
	{
		tileData.push_back(mmu.read(startAddr + i));
	}

	return tileData;
}

std::vector<unsigned char> PPU::getBackgroundMap(int tileIndex)
{
	std::vector<unsigned char> mapData;
	bool isBaseAddressingMode = bitwise::check_bit(*(mmu.LCDC), 4);
	unsigned short startAddr = isBaseAddressingMode ? 0x9800 + (tileIndex * 16) : 0x9000 + (0 * 16);

	for (int i = 0; i < 32; i++)
	{
		mapData.push_back(mmu.read(startAddr + i));
	}

	return mapData;
}

bool PPU::fetchBackgroundPixels()
{
	unsigned short startAddr = bitwise::check_bit(*(mmu.LCDC), 4) ? 0x8000 : 0x9000;
	//int tileStartIndex = floor(SCX / 8);
	//int indexIntoStartTile = SCX % 8;
	int bgRowIndex = 0;
	std::vector<unsigned char> tiles;





	//sf::Color pixel = backgroundFIFO.front();
	//backgroundFIFO.pop();

	//buffer.set_pixel(fetcherXPos, line, pixel);

	//fetcherXPos++;
	//fetcherXPos %= 160;

	return true;

	//20 wide, 18 high
	/*
	for (int i = 0; i < 360; i++)
	{
		unsigned short tilesetAddr = 0x9800 + i + bgRowIndex + ((SCX / 8) % 32);

		for (int i = 0; i < 16; i++)
		{
			tiles.push_back(mmu.read(0x8000 + i + mmu.read(tilesetAddr)));
		}

		if (i % 19 == 0 && i != 0)
			bgRowIndex += 12;
	}

	
	int rowIndex = 0;

	int xIndex = 0;
	int yIndex = 0;

	for (int i = 0; i < 23040; i++)
	{

	}
	*/


	/*
	for (int i = 0; i < tiles.size(); i += 2)
	{
		for (int j = 7; j >= 0; j--)
		{
			if (!bitwise::check_bit(tiles[i], j) && !bitwise::check_bit(tiles[i + 1], j))		//00
				buffer.set_pixel((7 - j) + (8 * (i % 20)), rowIndex, sf::Color(232, 232, 232));
			else if (!bitwise::check_bit(tiles[i], j) && bitwise::check_bit(tiles[i + 1], j))	//01
				buffer.set_pixel((7 - j) + (8 * (i % 20)), rowIndex, sf::Color(88, 88, 88));
			else if (bitwise::check_bit(tiles[i], j) && !bitwise::check_bit(tiles[i + 1], j))	//10
				buffer.set_pixel((7 - j) + (8 * (i % 20)), rowIndex, sf::Color(160, 160, 160));
			else																					//11
				buffer.set_pixel((7 - j) + (8 * (i % 20)), rowIndex, sf::Color(16, 16, 16));
		}

		if (i % 19 == 0 && i != 0)
			rowIndex += 8;
	}
	*/

	/*
	for (int m = 0; m < 18; m++)
	{
		for (int t = 0; t < 20; t++)
		{
			int rowIndex = m * 8;
			int mapIndex = (m * 32) + t;

			unsigned char tilesetIndex = mmu.read(0x9800 + mapIndex);
			std::vector<unsigned char> tileData = getTile(tilesetIndex);

			for (int i = 0; i < tileData.size(); i += 2)
			{
				for (int j = 7; j >= 0; j--)
				{
					if (!bitwise::check_bit(tileData[i], j) && !bitwise::check_bit(tileData[i + 1], j))		//00
						buffer.set_pixel((7 - j) + (t * 8), rowIndex, sf::Color(232, 232, 232));
					else if (!bitwise::check_bit(tileData[i], j) && bitwise::check_bit(tileData[i + 1], j))	//01
						buffer.set_pixel((7 - j) + (t * 8), rowIndex, sf::Color(88, 88, 88));
					else if (bitwise::check_bit(tileData[i], j) && !bitwise::check_bit(tileData[i + 1], j))	//10
						buffer.set_pixel((7 - j) + (t * 8), rowIndex, sf::Color(160, 160, 160));
					else																					//11
						buffer.set_pixel((7 - j) + (t * 8), rowIndex, sf::Color(16, 16, 16));
				}

				rowIndex++;
			}
		}
	}
	*/
}

void PPU::searchOAM()
{

}

void PPU::tick(int cyclesRemaining)
{
	/*
	bool displayEnabled = bitwise::check_bit(*(mmu.LCDC), 7);

	if (!displayEnabled)
		return;

	while (cyclesElapsed > 0)
	{
		cycleCount++;

		switch (PPU_Mode)
		{
			case 0:			//OAM SCAN (80 dots)
				searchOAM();
				break;
			case 1:			//DRAW (172 - 289 dots)

				break;
			case 2:			//H-BLANK (87 - 294 dots)

				break;
			case 3:			//V-BLANK (4560 dots total, 10 lines with 456 per line)

				break;
		}

		cyclesElapsed--;
	}
	*/

	unsigned char SCX = *(mmu.SCX);
	unsigned char SCY = *(mmu.SCY);
	unsigned char LY = *(mmu.LY);

	while (cyclesRemaining > 0)
	{
		cycleCount++;

		switch (PPU_Mode)
		{
			case 0:		//Real mode 2, OAM search
				if (cycleCount >= CLOCKS_PER_SCANLINE_OAM)
				{
					cycleCount = cycleCount % CLOCKS_PER_SCANLINE_OAM;
					//cycleCount = 0;
					mmu.write(0xFF41, mmu.read(0xFF41) | 0x01);
					mmu.write(0xFF41, mmu.read(0xFF41) | 0x02);
					PPU_Mode = 1;
				}

				cyclesRemaining--;
				break;

			case 1:		//Real mode 3, data transfer to LCD driver
				if (cyclesRemaining >= 2)
				{
					switch (fetcherState)
					{
						case 0:
							
							
							//offset = ((SCX / 8) & 0x1F + (32 * (((LY + SCY) & 0xFF) / 8))) & 0x3FF;
							offset = (((SCX / 8) + fetcherXPos) & 0x1F) + ((LY + SCY) & 0xFF);
							//std::cout << "fetcher x: " << fetcherXPos << std::endl;
							//std::cout << "tile addr: " << std::hex << std::setfill('0') << std::setw(4) << 0x9800 + offset + fetcherXPos << std::endl;
							//tileFetchAddr = mmu.read(0x9800 + offset + fetcherXPos) << 4;
							tileFetchAddr = mmu.read(0x9800 + offset) << 4;
							fetcherState++;
							break;
						case 1:
							//tileDataLow = mmu.read(0x8000 + tileFetchAddr + (2 * ((LY + SCY) % 8)));
							if (tileFetchAddr == 0x09B0)
								tileFetchAddr = tileFetchAddr;

							fetchoffset = tileFetchAddr + (2 * ((LY + SCY) % 8));

							//tileDataLow = mmu.read(0x8000 + fetchoffset);
							tileDataLow = mmu.read(0x8000 + tileFetchAddr);
							//std::cout << "tile data low: " << std::hex << std::setfill('0') << std::setw(4) << tileDataLow << std::endl;
							fetcherState++;
							break;
						case 2:
							//tileDataHigh = mmu.read(0x8000 + tileFetchAddr + 1 + (2 * ((LY + SCY) % 8)));
							//tileDataHigh = mmu.read(0x8000 + tileFetchAddr + 1 + (2 * ((LY + SCY) % 8)));
							tileDataHigh = mmu.read(0x8000 + tileFetchAddr);
							//std::cout << "tile data high: " << std::hex << std::setfill('0') << std::setw(4) << tileDataHigh << std::endl;
							fetcherState++;
							break;
						case 3:
							if (backgroundFIFO.empty())
							{
								for (int i = 7; i >= 0; i--)
								{
									if (!bitwise::check_bit(tileDataLow, i) && !bitwise::check_bit(tileDataHigh, i))
										backgroundFIFO.push(sf::Color(232, 232, 232));
									else if (!bitwise::check_bit(tileDataLow, i) && bitwise::check_bit(tileDataHigh, i))
										backgroundFIFO.push(sf::Color(88, 88, 88));
									else if (bitwise::check_bit(tileDataLow, i) && !bitwise::check_bit(tileDataHigh, i))
										backgroundFIFO.push(sf::Color(160, 160, 160));
									else
										backgroundFIFO.push(sf::Color(16, 16, 16));
								}

								fetcherState = 0;
							}

							break;
					};

					cyclesRemaining -= 2;
				}

				if (!backgroundFIFO.empty())
				{
					sf::Color pixel = backgroundFIFO.front();
					backgroundFIFO.pop();

					buffer.set_pixel(fetcherXPos, line, pixel);

					fetcherXPos++;
					//popcount++;
				}

				if (fetcherXPos == 160)
				{
					std::queue<sf::Color>().swap(backgroundFIFO);
					fetcherXPos = 0;
					fetcherState = 0;

					cycleCount = cycleCount % CLOCKS_PER_SCANLINE_VRAM;
					//cycleCount = 0;
					PPU_Mode = 2;

					bool hblank_interrupt = bitwise::check_bit(*(mmu.STAT), 3);

					if (hblank_interrupt)
					{
						mmu.write(0xFF0F, mmu.read(0xFF0F) | 0x02);
					}

					bool lyc_interrupt = bitwise::check_bit(*(mmu.STAT), 6);
					bool lyc = (*(mmu.LYC) == line);

					if (lyc_interrupt && lyc)
					{
						mmu.write(0xFF0F, mmu.read(0xFF0F) | 0x02);
					}

					lyc ? mmu.write(0xFF41, mmu.read(0xFF41) | 0x04) : mmu.write(0xFF41, mmu.read(0xFF41) & 0xFB);
					mmu.write(0xFF41, mmu.read(0xFF41) & 0xFE);
					mmu.write(0xFF41, mmu.read(0xFF41) & 0xFD);
				}

				cyclesRemaining--;

				if (cycleCount >= CLOCKS_PER_SCANLINE_VRAM)
				{

				}
				break;

			case 2:		//Real mode 0, H-Blank
				if (cycleCount >= CLOCKS_PER_HBLANK)
				{
					//drawBackground();		//0:21 with just this enabled
					//drawWindow();


					/*
					for (int m = 0; m < 17; m++)
					{
						for (int t = 0; t < 16; t++)
						{
							int rowIndex = m * 8;
							int tileIndex = (m * 16) + t;

							std::vector<unsigned char> tileData = getTile(tileIndex);

							for (int i = 0; i < tileData.size(); i += 2)
							{
								for (int j = 7; j >= 0; j--)
								{
									if (!bitwise::check_bit(tileData[i], j) && !bitwise::check_bit(tileData[i + 1], j))		//00
										buffer.set_pixel((7 - j) + (t * 8), rowIndex, sf::Color(232, 232, 232));
									else if (!bitwise::check_bit(tileData[i], j) && bitwise::check_bit(tileData[i + 1], j))	//01
										buffer.set_pixel((7 - j) + (t * 8), rowIndex, sf::Color(88, 88, 88));
									else if (bitwise::check_bit(tileData[i], j) && !bitwise::check_bit(tileData[i + 1], j))	//10
										buffer.set_pixel((7 - j) + (t * 8), rowIndex, sf::Color(160, 160, 160));
									else																					//11
										buffer.set_pixel((7 - j) + (t * 8), rowIndex, sf::Color(16, 16, 16));
								}

								rowIndex++;
							}
						}
					}
					*/
					line++;
					mmu.updateLY(line);

					cycleCount = cycleCount % CLOCKS_PER_HBLANK;
					//cycleCount = 0;

					if (line == 144)
						//if(*(mmu.LY))
					{
						PPU_Mode = 3;
						mmu.write(0xFF41, mmu.read(0xFF41) | 0x01);
						mmu.write(0xFF41, mmu.read(0xFF41) & 0xFD);

						mmu.write(0xFF0F, mmu.read(0xFF0F) | 0x01);
					}

					else
					{
						PPU_Mode = 0;
						mmu.write(0xFF41, mmu.read(0xFF41) & 0xFE);
						mmu.write(0xFF41, mmu.read(0xFF41) | 0x02);
					}
				}

				cyclesRemaining--;
				break;

			case 3:		//Real mode 1, V-Blank
				if (cycleCount >= CLOCKS_PER_SCANLINE)
					//if(cycleCount >= CLOCKS_PER_VBLANK)
				{
					line++;
					cycleCount = cycleCount % CLOCKS_PER_SCANLINE;
					//cycleCount = 0;

					if (line == 154)
					{
						if (cpu.getRefreshClocksElapsed() >= 70224)
						{
							drawSprites();
							draw(buffer);		//1:21 with just this enabled
							cpu.setRefreshClocksElapsed(cpu.getRefreshClocksElapsed() - 70224);
						}

						buffer.reset();
						line = 0;
						mmu.write(0xFF44, 0x00);
						PPU_Mode = 0;
						mmu.write(0xFF41, mmu.read(0xFF41) & 0xFE);
						mmu.write(0xFF41, mmu.read(0xFF41) | 0x02);
					}
				}
				cyclesRemaining--;
				break;

			default:		//Invalid PPU mode
				break;
		}

	}
}

sf::Color PPU::callGetPixel(unsigned int x, unsigned int y) const
{
	return getPixel(x, y);
}

inline sf::Color PPU::getPixel(unsigned int x, unsigned int y) const
{
	//return pixels[(SCREEN_WIDTH * y) + x];
	return sf::Color::Black;
}