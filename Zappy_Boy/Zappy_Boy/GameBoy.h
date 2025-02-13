#pragma once
#ifndef GAMEBOY_H
#define GAMEBOY_H

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <ShlObj.h>
#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "CPU.h"
#include "PPU.h"
#include "MMU.h"
#include "Cartridge.h"
#include "definitions.h"
#include "Debugger.h"

class GameBoy
{
	public:

		//GameBoy(std::function<void(std::vector<sf::Color>&)> draw);
		GameBoy(std::vector<unsigned char> cartridgeData);
		~GameBoy();

		bool loadGame(std::string fileName);

		void pressButton(int buttonIndex);

		//void powerOn(std::function<void(std::vector<sf::Color>&)> draw);
		void powerOn(std::function<bool()> checkWindow, std::function<void(const Buffer &buffer)> draw);

		void enableDebugger();

		CPU cpu;
		PPU ppu;
		MMU mmu;
		Cartridge cartridge;

		static std::vector<std::string> debugStrings;
};

#endif