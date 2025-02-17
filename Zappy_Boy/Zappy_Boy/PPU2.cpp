#include "PPU2.h"

PPU2::PPU2(CPU& zbCPU, MMU& zbMMU) : cpu(zbCPU), mmu(zbMMU), buffer(SCREEN_WIDTH, SCREEN_HEIGHT)
{

}

PPU2::~PPU2()
{

}

Tile PPU2::getTile()
{
	bool isBaseAddressingMode = bitwise::check_bit(*(mmu.LCDC), 4);

	if (isBaseAddressingMode)
		mmu.readMemory(0x8000 + (tileNumber * 16));
	else
		mmu.readMemory(0x9000 + (signedTileNumber * 16));
}

void PPU2::tick()
{



}