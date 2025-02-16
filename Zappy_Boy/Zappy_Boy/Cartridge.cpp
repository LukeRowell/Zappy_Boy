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

	RAMG_REG = 0b0;
	MODE_REG = 0b0;
	BANK1_REG = 0x01;
	BANK2_REG = 0x0;

	switch (cartridgeType)
	{
		case 0x00:
			RAMG_REG = false;
			ROM_BANKING = false;
			break;

		case 0x01:
			RAMG_REG = false;
			ROM_BANKING = true;
			break;

		case 0x02:
			RAMG_REG = false;
			ROM_BANKING = true;
			break;

		case 0x03:
			RAMG_REG = false;
			ROM_BANKING = true;
			ROM_BANK = 0x1;

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
			return readNoMBC(address);
			break;

		case 0x02:		//MBC1 + RAM
			return readMBC1(address);
			break;

		case 0x03:		//MBC1 + RAM + BATTERY
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
			writeNoMBC(address, value);
			break;

		case 0x02:		//MBC1 + RAM
			writeMBC1(address, value);
			break;

		case 0x03:		//MBC1 + RAM + BATTERY
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
		if (!RAMG_REG)
			return 0xFF;

		//unsigned int offset = 0x4000 * ROM_BANK;
		//unsigned short bankAddress = address - 0xA000;
		//unsigned int address = bankAddress + offset;

		return RAM[(address - 0xA000) % RAM.size()];
	}
}

void Cartridge::writeMBC1(const unsigned short address, unsigned char value)
{
	if (address >= 0x0000 && address <= 0x1FFF)		//Writing to this register enables/disables RAM
	{
		if ((value & 0x0F) == 0x0A)
			RAMG_REG = true;
		else
			RAMG_REG = false;

		return;
	}

	if (address >= 0x2000 && address <= 0x3FFF)		//ROM bank select register
	{
		BANK1_REG = value;

		if (BANK1_REG == 0x00)
			BANK1_REG |= 0x01;

		return;
	}

	if (address >= 0x4000 && address <= 0x5FFF)
	{

	}

	if (address >= 0x6000 && address <= 0x7FFF)		//Banking mode select register
	{
		MODE_REG = bitwise::check_bit(value, 0);

		return;
	}

	if (address >= 0xA000 && address <= 0xBFFF)		//RAM bank select register
	{
		if (RAMG_REG)
		{
			if (RAM_SIZE == 0x03)
			{
				if (!MODE_REG)
					RAM[address - 0xA000] = value;
				else
					RAM[0x2000 * BANK2_REG + (address - 0xA000)] = value;
			}

			else
				RAM[(address - 0xA000) % RAM.size()] = value;
		}

		return;
	}
}