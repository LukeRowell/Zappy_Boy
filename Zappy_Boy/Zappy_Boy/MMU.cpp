#include "MMU.h"
#include <assert.h>

MMU::MMU(CPU &zbCPU, PPU &zbPPU, Cartridge &zbCartridge) : cpu(zbCPU), ppu(zbPPU), cartridge(zbCartridge)
{
	unsigned char logo[48] = { 0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0c,
		0x00, 0x0d, 0x00, 0x08, 0x11, 0x1f, 0x88, 0x89, 0x00, 0x0e, 0xdc, 0xcc, 0x6e, 0xe6,
		0xdd, 0xdd, 0xd9, 0x99, 0xbb, 0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc, 0xdd,
		0xdc, 0x99, 0x9f, 0xbb, 0xb9, 0x33, 0x3e };

	for (unsigned short i = 0x0104; i < 0x0134; i++)
	{
		memory[i] = logo[i - 0x0104];
	}

	memory[0xFF00] = 0xCF;
	memory[0xFF04] = 0xAB;
	memory[0xFF05] = 0x00;
	memory[0xFF06] = 0x00;
	memory[0xFF07] = 0xF8;
	memory[0xFF0F] = 0xE0;		//BGB value
	memory[0xFF10] = 0x80;
	memory[0xFF11] = 0xBF;
	memory[0xFF12] = 0xF3;
	memory[0xFF14] = 0xBF;
	memory[0xFF16] = 0x3F;
	memory[0xFF17] = 0x00;
	memory[0xFF19] = 0xBF;
	memory[0xFF1A] = 0x7F;
	memory[0xFF1B] = 0xFF;
	memory[0xFF1C] = 0x9F;
	memory[0xFF1E] = 0xBF;
	memory[0xFF20] = 0xFF;
	memory[0xFF21] = 0x00;
	memory[0xFF22] = 0x00;
	memory[0xFF23] = 0xBF;
	memory[0xFF24] = 0x77;
	memory[0xFF25] = 0xF3;
	memory[0xFF26] = 0xF1;
	memory[0xFF40] = 0x91;
	memory[0xFF41] = 0x85;		//BGB value
	memory[0xFF42] = 0x00;
	memory[0xFF43] = 0x00;
	memory[0xFF44] = 0x90;		//BGB value maybe?
	memory[0xFF44] = 0x00;
	memory[0xFF45] = 0x00;
	memory[0xFF47] = 0xFC;
	memory[0xFF48] = 0xFF;
	memory[0xFF49] = 0xFF;
	memory[0xFF4A] = 0x00;
	memory[0xFF4B] = 0x00;
	memory[0xFFFF] = 0x00;

	DIV = &memory[0xFF04];
	TIMA = &memory[0xFF05];
	TMA = &memory[0xFF06];
	TAC = &memory[0xFF07];
	IF = &memory[0xFF0F];
	NR10 = &memory[0xFF10];
	NR11 = &memory[0xFF11];
	NR12 = &memory[0xFF12];
	NR14 = &memory[0xFF14];
	NR21 = &memory[0xFF16];
	NR22 = &memory[0xFF17];
	NR24 = &memory[0xFF19];
	NR30 = &memory[0xFF1A];
	NR31 = &memory[0xFF1B];
	NR32 = &memory[0xFF1C];
	NR33 = &memory[0xFF1E];
	NR41 = &memory[0xFF20];
	NR42 = &memory[0xFF21];
	NR43 = &memory[0xFF22];
	NR44 = &memory[0xFF23];
	NR50 = &memory[0xFF24];
	NR51 = &memory[0xFF25];
	NR52 = &memory[0xFF26];
	LCDC = &memory[0xFF40];
	STAT = &memory[0xFF41];
	SCY = &memory[0xFF42];
	SCX = &memory[0xFF43];
	LY = &memory[0xFF44];
	LYC = &memory[0xFF45];
	DMA = &memory[0xFF46];
	BGP = &memory[0xFF47];
	OBP0 = &memory[0xFF48];
	OBP1 = &memory[0xFF49];
	WY = &memory[0xFF4A];
	WX = &memory[0xFF4B];
	IE = &memory[0xFFFF];

	DMA_Val = 0;
	DMA_Index = 0;
	setup = false;
	setupCycles = 0;
	DIV_WRITE = false;

	lastDirectionalButtonPressed = 0xFF;
	lastSelectButtonPressed = 0xFF;
	readCount = 5;
}

MMU::~MMU()
{

}

unsigned char MMU::readMemory(unsigned short address)		//Return the value from memory located at specified address
{
	//ppu.tick(1);

	assert(address >= 0x0000 && address <= 0xFFFF);

	if (address >= 0x0000 && address <= 0x7FFF)		//Cartridge ROM
	{
		return cartridge.read(address);
	}

	else if (address >= 0x8000 && address <= 0x9FFF)		//VRAM
	{
		//if (ppu.getMode() == 3)
		{
		//	return 0xFF;
		}

		//else
		{
			return memory[address];
		}
	}

	else if (address >= 0xA000 && address <= 0xBFFF)		//Cartridge RAM
	{
		return cartridge.read(address);
		//memory[address];
	}

	else if (address >= 0xC000 && address <= 0xDFFF)		//WRAM
	{
		return memory[address];
	}

	else if (address >= 0xE000 && address <= 0xFDFF)		//ECHO RAM
	{
		return memory[address - 0x2000];
	}

	else if (address >= 0xFE00 && address <= 0xFE9F)		//OAM
	{
		//if (ppu.getMode() == 2 || ppu.getMode() == 3)		//Cannot read OAM during OAM search or V-blank
		{
			//return 0xFF;
		}

		//else
		{
			return memory[address];
		}
	}

	else if (address >= 0xFEA0 && address <= 0xFEFF)		//Unusable memory
	{
		return 0xFF;
	}

	else if (address >= 0xFF00 && address <= 0xFF7F)		//IO
	{
		return readIO(address);
	}

	else if (address >= 0xFF80 && address <= 0xFFFE)
	{
		return memory[address];
	}

	else if (address == 0xFFFF)
	{
		return memory[address];
	}
}

void MMU::writeMemory(unsigned short address, unsigned char value)		//Write the specified value to memory at the specified address
{
	//ppu.tick(1);

	assert(address >= 0x0000 && address <= 0xFFFF);

	if (address >= 0x0000 && address <= 0x7FFF)		//Writing to the cartridge ROM (valid only for games using an MBC)
	{
		//write(address, value);
		cartridge.write(address, value);
		return;
	}

	else if (address >= 0x8000 && address <= 0x9FFF)		//Writing to VRAM
	{
		write(address, value);
		return;
	}

	else if (address >= 0xA000 && address <= 0xBFFF)		//Writing to cartridge RAM
	{
		//write(address, value);
		cartridge.write(address, value);
		return;
	}

	else if (address >= 0xC000 && address <= 0xDFFF)		//Writing to work RAM
	{
		write(address, value);
		return;
	}

	else if (address >= 0xE000 && address <= 0xFDFF)		//Writing to ECHO RAM
	{
		write(address - 0x2000, value);		//TODO: Examine how/if this needs to be handled
		return;
	}

	else if (address >= 0xFE00 && address <= 0xFE9F)		//Writing to OAM
	{
		//if (ppu.getMode() != 2)
		{
			write(address, value);
		}
		return;
	}

	else if (address >= 0xFEA0 && address <= 0xFEFF)		//Unusable memory, writes not allowed
	{
		return;
	}

	else if (address >= 0xFF00 && address <= 0xFF7F)		//Writing to an IO register
	{
		writeIO(address, value);
		return;
	}

	else if (address >= 0xFF80 && address <= 0xFFFE)		//Writing to HRAM
	{
		write(address, value);
		return;
	}

	else if (address == 0xFFFF)		//Writing to IE register
	{
		write(0xFFFF, value);
		return;
	}
}

unsigned char MMU::readIO(unsigned short address)
{
	unsigned char retval;

	switch (address)
	{
		case 0xFF04:
			retval = cpu.DIV >> 8;
			return cpu.DIV;
			break;

		case 0xFF05:
			return memory[address];
			break;

		case 0xFF00:
			if (memory[0xFF00] == 0xEF)
				memory[0xFF00] = memory[0xFF00] & lastDirectionalButtonPressed;
			else if (memory[0xFF00] == 0xDF)
				memory[0xFF00] = memory[0xFF00] & lastSelectButtonPressed;

			return memory[address];
			break;

		case 0xFF0F:
			return memory[address];
			break;

		case 0xFF44:
			return ppu.line;
			break;

		default:
			return memory[address];
			break;
	}
}

void MMU::writeIO(unsigned short registerValue, unsigned char value)
{
	unsigned char writeval = value | 0xCF;

	switch (registerValue)
	{
		case 0xFF00:		//Joypad input
			if (value == 0x30)
			{
				lastDirectionalButtonPressed = 0xFF;
				lastSelectButtonPressed = 0xFF;
			}

			write(0xFF00, writeval);
			//write(0xFF00, value);
			break;

		case 0xFF01:		//Serial data transfer
			write(0xFF01, value);
			//std::cout << std::hex << value;
			break;

		case 0xFF02:		//Serial data transfer
			write(0xFF02, value);
			break;

		case 0xFF03:
			write(0xFF03, value);
			break;

		case 0xFF04:		//DIV, writing to this registers resets it to 0x00
			cpu.DIV = 0x0000;
			write(0xFF04, 0x00);
			//cpu.setDivider(0);
			DIV_WRITE = true;
			cpu.timerClocksElapsed = 0;		//Writing to DIV resets TIMA
			break;

		case 0xFF05:		//TIMA
			timerEnabledThisInstruction = true;
			write(0xFF05, value);
			break;

		case 0xFF06:		//TMA
			write(0xFF06, value);
			break;

		case 0xFF07:		//TAC
			write(0xFF07, value | 0xF8);
			timerEnabledThisInstruction = true;
			break;

		case 0xFF0F:		//IF			TODO: upper 3 bits should remain set
			write(0xFF0F, value);
			write(0xFF0F, value | 0xE0);
			break;

		case 0xFF11:
			write(0xFF11, value);
			break;

		case 0xFF26:
			write(0xFF26, value);
			break;

		case 0xFF40:		//LCDC
			write(0xFF40, value);
			break;

		case 0xFF41:		//STAT
			write(0xFF41, value);
			break;

		case 0xFF42:		//SCY
			write(0xFF42, value);
			break;

		case 0xFF43:		//SCX
			write(0xFF43, value);
			break;

		case 0xFF44:		//LY, writing to this register resets it to 0x00
			write(0xFF44, 0x00);
			ppu.line = 0x00;
			//write(0xFF44, value);
			break;

		case 0xFF45:		//LYC
			write(0xFF45, value);
			break;

		case 0xFF46:		//Write to DMA register, triggers DMA transfer
			DMA_Val = value;
			DMA_Index = 0;
			setup = true;
			setupCycles = 1;
			cpu.setDMAStatus(true);
			write(0xFF46, value);
			break;

		case 0xFF47:		//BGP (palette)
			write(0xFF47, value);
			break;

		case 0xFF48:		//OBP0 (sprite palette 0)
			write(0xFF48, value);
			break;

		case 0xFF49:		//OBP1 (sprite palette 1)
			write(0xFF49, value);
			break;

		case 0xFF4A:		//WY
			write(0xFF4A, value);
			break;

		case 0xFF4B:		//WX
			write(0xFF4B, value);
			break;

		case 0xFF4D:		//KEY1 (prepare speed switch for CGB mode)
			write(0xFF4D, value);
			break;

		case 0xFF50:		//???
			write(0xFF50, value);
			break;

		default:
			write(registerValue, value);
			break;
	}
}

void MMU::tickDMA()		//DMA transfer to OAM triggered by writing to DMA register
{
	unsigned short start_addr = DMA_Val * 0x100;

	//if (!setup)
	{
		if (DMA_Index < 0x9F)
		{
			for (int i = 0; i < cpu.getCyclesElapsed() - setupCycles; i++)
			{
				writeMemory(0xFE00 + DMA_Index, readMemory(start_addr + DMA_Index));
				DMA_Index++;
			}
			//writeMemory(0xFE00 + DMA_Index, readMemory(start_addr + DMA_Index));
			//DMA_Index++;
			setupCycles = 0;
		}

		else
		{
			cpu.setDMAStatus(false);
		}
	}

	//else
	{
		//setup = false;
	}
}

void MMU::updateDIV(unsigned short divider)
{
	memory[0xFF04] += 1;
}

void MMU::updateLY(unsigned char newLY)
{
	memory[0xFF44] = newLY;
}

void MMU::setDIV_WRITE(bool newDIV_WRITE)
{
	DIV_WRITE = newDIV_WRITE;
}

bool MMU::getDIV_WRITE()
{
	return DIV_WRITE;
}

unsigned char MMU::read(unsigned short address)		//Return the value from memory located at specified address
{
	return memory[address];
}

void MMU::write(unsigned short address, unsigned char value)		//Write the specified value to memory at the specified address
{
	memory[address] = value;
}