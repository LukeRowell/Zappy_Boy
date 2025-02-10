#include "Cartridge.h"

Cartridge::Cartridge(std::vector<unsigned char> cartridgeData) : ROM(cartridgeData)
{
	for (unsigned short i = 0x0134; i < 0x0144; i++)
	{
		if (cartridgeData[i] != 0x00)
		{
			title += cartridgeData[i];
		}
	}

	CGBFlag = ROM[0x0143];
	newLicenseeCode = (ROM[0x0144] << 8) | ROM[0x0145];
	SGBFlag = ROM[0x0146];
	cartridgeType = ROM[0x0147];
	ROM_SIZE = ROM[0x0148];
	RAM_SIZE = ROM[0x0149];
	destinationCode = ROM[0x014A];
	oldLicenseeCode = ROM[0x014B];
	maskROMVersionNumber = ROM[0x014C];
	headerChecksum = ROM[0x014D];
	globalChecksum = (ROM[0x014E] << 8) | ROM[0x014F];

	switch (cartridgeType)
	{
		case 0x00:
			RAM_ENABLED = false;
			ROM_BANKING = false;
			break;

		case 0x01:
			RAM_ENABLED = false;
			ROM_BANKING = true;
			ROM_BANK = 0x1;
			break;

		case 0x02:
			RAM_ENABLED = true;
			ROM_BANKING = true;
			ROM_BANK = 0x1;
			break;

		default:
			break;
	}

	/*switch (ROM_SIZE)		//TODO: finish adding all sizes of ROM
	{
		case 0x00:
			ROM = std::vector<unsigned char>(0x8000, 0);
			break;

		case 0x01:
			ROM = std::vector<unsigned char>(0x10000, 0);
			break;

		case 0x02:
			ROM = std::vector<unsigned char>(0x20000, 0);
			break;

		default:
			break;
	}*/

	switch (RAM_SIZE)
	{
		case 0x00:
			RAM = std::vector<unsigned char>(0, 0);
			break;

		case 0x01:
			RAM = std::vector<unsigned char>(0x800, 0);
			break;

		case 0x02:
			RAM = std::vector<unsigned char>(0x2000, 0);
			break;

		case 0x03:
			RAM = std::vector<unsigned char>(0x8000, 0);
			break;

		default:
			break;
	}
}

Cartridge::~Cartridge()
{

}

unsigned char Cartridge::read(const unsigned short address)
{
	switch (cartridgeType)
	{
		case 0x00:		//No MBC
			return readNoMBC(address);
			break;

		case 0x01:		//MBC1
			return readMBC1(address);
			break;

		case 0x02:		//MBC1 + RAM
			return readMBC1(address);
			break;

		default:
			return readNoMBC(address);
			break;
	}
}

void Cartridge::write(const unsigned short address, unsigned char value)
{
	switch (cartridgeType)
	{
		case 0x00:		//No MBC
			writeNoMBC(address, value);
			break;

		case 0x01:		//MBC1
			writeMBC1(address, value);
			break;

		case 0x02:		//MBC1 + RAM
			writeMBC1(address, value);
			break;

		default:
			break;
	}
}

unsigned char Cartridge::readNoMBC(const unsigned short address)
{
	return ROM[address];
}

void Cartridge::writeNoMBC(const unsigned short address, unsigned char value)
{
	return;
}

unsigned char Cartridge::readMBC1(const unsigned short address)
{
	if (address >= 0x0000 && address <= 0x3FFF)
	{
		return ROM[address];
	}

	else if (address >= 0x4000 && address <= 0x7FFF)
	{
		unsigned short bankAddress = address - 0x4000;

		return ROM[(0x4000 * ROM_BANK) + bankAddress];
	}

	else if (address >= 0xA000 && address <= 0xBFFF)
	{
		unsigned int offset = 0x4000 * ROM_BANK;
		unsigned short bankAddress = address - 0xA000;
		unsigned int address = bankAddress + offset;

		return RAM[address];
	}
}

void Cartridge::writeMBC1(const unsigned short address, unsigned char value)
{
	if (address >= 0x0000 && address <= 0x1FFF)		//Writes to this area enable the RAM
	{
		RAM_ENABLED = true;
	}

	if (address >= 0x2000 && address <= 0x3FFF)
	{
		switch (value)
		{
			case 0x0:
				ROM_BANK = 0x1;
				break;

			case 0x20:
				ROM_BANK = 0x21;
				break;

			case 0x40:
				ROM_BANK = 0x41;
				break;

			case 0x60:
				ROM_BANK = 0x61;
				break;

			default:
				ROM_BANK = value & 0x1F;
				break;
		}
	}

	if (address >= 0xA000 && address <= 0xBFFF)
	{
		if (RAM_ENABLED && RAM_SIZE != 0)
		{
			auto offset = 0x2000 * RAM_BANK;
			auto bankAddress = (address - 0xA000) + offset;

			RAM[bankAddress] = value;
		}
	}
}