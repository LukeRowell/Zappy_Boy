#pragma once
#ifndef MMU_H
#define MMU_H

#include "CPU.h"
#include "PPU.h"
#include "Cartridge.h"

class CPU;
class PPU;
class Cartridge;

class MMU
{
	public:

		MMU(CPU &zbCPU, PPU &zbPPU, Cartridge &zbCartridge);
		~MMU();

		unsigned char readMemory(unsigned short address);
		void writeMemory(unsigned short address, unsigned char value);

		unsigned char readIO(unsigned short address);
		void writeIO(unsigned short address, unsigned char value);

		void tickDMA();

		void updateDIV(unsigned short divider);

		void updateLY(unsigned char newLY);

		void setDIV_WRITE(bool newDIV_WRITE);

		bool getDIV_WRITE();

		unsigned char lastPressed;
		int readCount;

		unsigned short DIV_COUNTER;
		unsigned char* DIV;
		unsigned char* TIMA;
		unsigned char* TMA;
		unsigned char* TAC;
		unsigned char* IF;
		unsigned char* NR10;
		unsigned char* NR11;
		unsigned char* NR12;
		unsigned char* NR14;
		unsigned char* NR21;
		unsigned char* NR22;
		unsigned char* NR24;
		unsigned char* NR30;
		unsigned char* NR31;
		unsigned char* NR32;
		unsigned char* NR33;
		unsigned char* NR41;
		unsigned char* NR42;
		unsigned char* NR43;
		unsigned char* NR44;
		unsigned char* NR50;
		unsigned char* NR51;
		unsigned char* NR52;
		unsigned char* LCDC;
		unsigned char* STAT;
		unsigned char* SCY;
		unsigned char* SCX;
		unsigned char* LY;
		unsigned char* LYC;
		unsigned char* DMA;
		unsigned char* BGP;
		unsigned char* OBP0;
		unsigned char* OBP1;
		unsigned char* WY;
		unsigned char* WX;
		unsigned char* IE;

		unsigned char read(unsigned short address);
		void write(unsigned short address, unsigned char value);

		const static int MEM_SIZE = 65536;
		unsigned char memory[MEM_SIZE];

		bool setup;
		int setupCycles;
		unsigned char DMA_Val;
		unsigned char DMA_Index;
		bool DIV_WRITE;

		CPU &cpu;
		PPU &ppu;
		Cartridge &cartridge;
};

#endif