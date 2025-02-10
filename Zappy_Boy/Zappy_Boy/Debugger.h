#pragma once
#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <iostream>
#include <iomanip>
#include <string>
#include <stdio.h>
#include <stdlib.h>
class CPU;

class GameBoy;

class Debugger
{
	public:

		Debugger();
		~Debugger();

		void setPreviousValues(GameBoy& zappyBoy);

		void printPC(unsigned short PC);

		void printVar8(unsigned char var);

		void printVar16(unsigned short var);

		void printChangedVars(GameBoy& zappyBoy);

		void printMessage(CPU& ZBCPU);

	private:

		unsigned char prev_A;					//Registers A - L
		unsigned char prev_F;
		unsigned char prev_B;
		unsigned char prev_C;
		unsigned char prev_D;
		unsigned char prev_E;
		unsigned char prev_H;
		unsigned char prev_L;
		unsigned short prev_AF;
		unsigned short prev_BC;					//Registers B and C combined
		unsigned short prev_DE;					//Registers D and E combined
		unsigned short prev_HL;					//Registers H and L combined
		unsigned short prev_SP;					//Stack pointer
		unsigned short prev_PC;					//Program counter	
		unsigned char prev_memoryAF;
		unsigned char prev_memoryBC;
		unsigned char prev_memoryDE;
		unsigned char prev_memoryHL;
		unsigned char prev_memorySP;
		unsigned char prev_memoryNN;
		unsigned char prev_memoryFFXX;
		unsigned char prev_memoryFFCC;
		unsigned char prev_zFlag;				//Zero flag
		unsigned char prev_nFlag;				//Subtract flag
		unsigned char prev_hFlag;				//Half-carry flag
		unsigned char prev_cFlag;
};

#endif