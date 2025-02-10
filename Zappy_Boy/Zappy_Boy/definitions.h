#pragma once

#define SPEED_MODIFIER 1

const int SCREEN_WIDTH = 160;
const int SCREEN_HEIGHT = 144;

const unsigned int CLOCKS_PER_HBLANK = 204; /* Mode 0 */
//const unsigned int CLOCKS_PER_HBLANK = 456;
const unsigned int CLOCKS_PER_VBLANK = 4560; /* Mode 1 */
const unsigned int CLOCKS_PER_SCANLINE_OAM = 80; /* Mode 2 */
const unsigned int CLOCKS_PER_SCANLINE_VRAM = 172; /* Mode 3 */

//const unsigned int CLOCKS_PER_HBLANK = 51;
//const unsigned int CLOCKS_PER_VBLANK = 1140;
//const unsigned int CLOCKS_PER_SCANLINE_OAM = 20;
//const unsigned int CLOCKS_PER_SCANLINE_VRAM = 43;

const unsigned int CLOCKS_PER_SCANLINE =
(CLOCKS_PER_SCANLINE_OAM + CLOCKS_PER_SCANLINE_VRAM + CLOCKS_PER_HBLANK);

const unsigned int SCANLINES_PER_FRAME = 144;
const unsigned int CLOCKS_PER_FRAME = (CLOCKS_PER_SCANLINE * SCANLINES_PER_FRAME) + CLOCKS_PER_VBLANK;

const unsigned int BACKGROUND_SIZE = 256;
const unsigned int TILES_PER_LINE = 32;
const unsigned int TILE_HEIGHT_PX = 8;
const unsigned int TILE_WIDTH_PX = 8;
const unsigned int TILE_BYTES = 16;
const unsigned int SPRITE_BYTES = 4;

enum class GBColor
{
	Color0,
	Color1,
	Color2,
	Color3
};