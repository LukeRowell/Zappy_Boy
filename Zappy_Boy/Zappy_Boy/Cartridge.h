#pragma once
#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <iostream>
#include <vector>

class Cartridge
{
	public:

		Cartridge(std::vector<unsigned char> cartridgeData);
		~Cartridge();

		unsigned char read(const unsigned short address);
		void write(const unsigned short address, unsigned char value);

	private:

		unsigned char readNoMBC(const unsigned short address);
		void writeNoMBC(const unsigned short address, unsigned char value);

		unsigned char readMBC1(const unsigned short address);
		void writeMBC1(const unsigned short address, unsigned char value);

		std::vector<unsigned char> ROM;
		std::vector<unsigned char> RAM;

		unsigned short ROM_BANK;
		unsigned short RAM_BANK;

		bool RAM_ENABLED;
		bool ROM_BANKING;

		std::string title;
		unsigned char CGBFlag;
		unsigned short newLicenseeCode;
		unsigned char SGBFlag;
		unsigned char cartridgeType;
		unsigned char ROM_SIZE;
		unsigned char RAM_SIZE;
		unsigned char destinationCode;
		unsigned char oldLicenseeCode;
		unsigned char maskROMVersionNumber;
		unsigned char headerChecksum;
		unsigned short globalChecksum;
};

#endif