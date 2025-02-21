#include "GameBoy.h"

//GameBoy::GameBoy(std::function<void(std::vector<sf::Color>&)> draw) : cpu(mmu), ppu(cpu, mmu), mmu(cpu, ppu)
GameBoy::GameBoy(std::vector<unsigned char> cartridgeData) : cartridge(cartridgeData), cpu(mmu), ppu(cpu, mmu), mmu(cpu, ppu, cartridge)
{

}

GameBoy::~GameBoy()
{

}

bool GameBoy::loadGame(std::string fileName)
{
	std::ifstream ROM_File;
	int memoryOffset = 0x0000;
	char currentVal;
	bool status;

	ROM_File.open(fileName, std::ios::binary);

	if (!ROM_File.is_open())
	{
		//std::cout << "Could not open ROM: " << fileName << std::endl;
		status = false;
	}

	else
	{
		while (memoryOffset < 0x0100)		//The first 16K of ROM is always loaded into memory starting at 0x0000
		{
			ROM_File.get(currentVal);
			//memory[memoryOffset] = (unsigned char)currentVal;
			mmu.writeMemory(memoryOffset, (unsigned char)currentVal);
			memoryOffset++;
		}

		switch(mmu.readMemory(0x0147))
		//switch (mmu.memory[0x0147])		//Check which Memory Bank Controller (MBC) we need to use
		{
			case 0x00:		//No MBC (all ROM data is mapped directly into memory)
				while (ROM_File.get(currentVal))
				{
					//memory[memoryOffset] = (unsigned char)currentVal;
					mmu.writeMemory(memoryOffset, (unsigned char)currentVal);
					memoryOffset++;
				}
				break;

			case 0x01:
				break;

			default:
				break;
		}

		status = true;
	}

	ROM_File.close();

	return status;
}

void GameBoy::pressButton(int buttonIndex)
{
	//mmu.writeMemory(0xFF0F, mmu.readMemory(0xFF0F) | 0x10);
	//mmu.write(0xFF0F, 0x10);
	//mmu.write(0xFFFF, 0x10);

	unsigned char cur = mmu.read(0xFF00) & 0xF0;

	switch (buttonIndex)
	{
		case 0:		//Up
			//std::cout << "Up from pressButton" << std::endl;
			//mmu.writeMemory(0xFF00, 0xEB);
			mmu.lastDirectionalButtonPressed = 0xFB;
			mmu.readCount = 50;
			break;

		case 1:		//Down
			//mmu.writeMemory(0xFF00, 0xE7);
			mmu.lastDirectionalButtonPressed = 0xF7;
			mmu.readCount = 50;
			break;

		case 2:		//Left
			//mmu.writeMemory(0xFF00, 0xED);
			mmu.lastDirectionalButtonPressed = 0xFD;
			mmu.readCount = 50;
			break;

		case 3:		//Right
			//mmu.writeMemory(0xFF00, 0xEE);
			//mmu.write(0xFF00, 0xEE);
			//cpu.loadRegister8(cpu.A, 0xEE);
			//cpu.loadRegister8(cpu.IE, 0x08);
			//cpu.loadRegister8(cpu.IF, 0x08);
			//mmu.write(0xFFFF, 0x10);
			//cur = cur | 0x0E;
			//mmu.write(0xFF00, cur);
			mmu.lastDirectionalButtonPressed = 0xFE;
			mmu.readCount = 50;
			break;

		case 4:		//A
			//mmu.writeMemory(0xFF00, 0xDE);
			mmu.lastSelectButtonPressed = 0xFE;
			mmu.readCount = 50;

			break;

		case 5:		//B
			//mmu.writeMemory(0xFF00, 0xDD);
			mmu.lastSelectButtonPressed = 0xFD;
			mmu.readCount = 50;
			break;

		case 6:		//Start
			//mmu.writeMemory(0xFF00, 0xC7);
			mmu.lastSelectButtonPressed = 0xF7;
			mmu.readCount = 50;
			break;

		case 7:		//Select
			//mmu.writeMemory(0xFF00, 1096760xDB);
			mmu.lastSelectButtonPressed = 0xFB;
			mmu.readCount = 50;
			break;

		case 8:		//No button pressed
			//mmu.writeMemory(0xFF00, 0xFF);
			break;

		default:
			break;
	}
}

//void GameBoy::powerOn(std::function<void(std::vector<sf::Color>&)> draw)
void GameBoy::powerOn(std::function <bool()> checkWindow, std::function<void(const Buffer &buffer)> draw)
{
	ppu.setupDrawFunction(draw);

	while (checkWindow())
	{
		int cyclesElapsed = cpu.tick();

		ppu.tick(4);

		cpu.updateTimer(cyclesElapsed);
	}
}

void GameBoy::enableDebugger()
{
	cpu.debuggerEnabled = true;
}