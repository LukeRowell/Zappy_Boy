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
			writeMBC1(address, value);
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
		if (!MODE_REG)
			return ROM[address];
		else
			return ROM[0x4000 * 0 + address];
	}

	else if (address >= 0x4000 && address <= 0x7FFF)
	{
		unsigned char HIGH_BANK_NUMBER;

		switch (ROM_SIZE)
		{
			case 0x00:
				HIGH_BANK_NUMBER = BANK1_REG & 0x01;
				break;
			case 0x01:
				HIGH_BANK_NUMBER = BANK1_REG & 0x03;
				break;
			case 0x02:
				HIGH_BANK_NUMBER = BANK1_REG & 0x07;
				break;
			case 0x03:
				HIGH_BANK_NUMBER = BANK1_REG & 0x0F;
				break;
			case 0x04:
				HIGH_BANK_NUMBER = BANK1_REG & 0x1F;
				break;
			case 0x05:
				HIGH_BANK_NUMBER = BANK1_REG & 0x1F;
				break;
			case 0x06:
				HIGH_BANK_NUMBER = BANK1_REG & 0x1F;
				break;
			default:
				HIGH_BANK_NUMBER = 0x01;
				break;
		};

		return ROM[0x4000 * HIGH_BANK_NUMBER + (address - 0x4000)];
	}

	else if (address >= 0xA000 && address <= 0xBFFF)
	{
		if (!RAMG_REG)
			return 0xFF;

		unsigned short RAMAddress;

		switch (RAM_SIZE)
		{
			case 0x01:
				RAMAddress = (address - 0xA000) % RAM.size();
				break;
			case 0x02:
				RAMAddress = (address - 0xA000) % RAM.size();
				break;
			case 0x03:
				if (!MODE_REG)
					RAMAddress = address - 0xA000;
				else
					RAMAddress = 0x2000 * BANK2_REG + (address - 0xA000);
				break;
			default:
				RAMAddress = (address - 0xA000) % RAM.size();
		};

		return RAM[RAMAddress];
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
		if (value == 0x00)
			BANK1_REG = 0x01;
		else
		{
			switch (ROM_SIZE)
			{
				case 0x00:
					BANK1_REG = value & 0x01;
					break;
				case 0x01:
					BANK1_REG = value & 0x03;
					break;
				case 0x02:
					BANK1_REG = value & 0x07;
					break;
				case 0x03:
					BANK1_REG = value & 0x0F;
					break;
				case 0x04:
					BANK1_REG = value & 0x1F;
					break;
				case 0x05:
					BANK1_REG = value & 0x1F;
					break;
				case 0x06:
					BANK1_REG = value & 0x1F;
					break;
				default:
					BANK1_REG = 0x01;
					break;
			};
		}

		return;
	}

	if (address >= 0x4000 && address <= 0x5FFF)
	{
		BANK2_REG = value & 0x03;
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