#pragma once
#ifndef PPU2_H
#define PPU2_H

#include "bitwise.h"
#include "CPU.h"
#include "MMU.h"
#include "buffer.h"

class PPU2
{
	public:

		PPU2(CPU& zbCPU, MMU& zbMMU);
		~PPU2();

		Tile getTile();

		void tick();

	private:

		unsigned char tileNumber;
		signed char signedTileNumber;

		CPU& cpu;
		MMU& mmu;
		Buffer buffer;

		std::function<void(const Buffer& buffer)> drawFunc;
};

#endif