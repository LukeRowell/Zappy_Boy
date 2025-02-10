#pragma once
#ifndef CPU_H
#define CPU_H

#include <iostream>
#include "MMU.h"
#include "Debugger.h"

class MMU;

class CPU
{
	public:

		CPU(MMU &zbMMU);
		~CPU();

		bool checkBit(int bitIndex, unsigned char byte);

		void loadRegister8(unsigned char &reg, unsigned char value);
		void loadRegister16(unsigned short &reg, unsigned short value);

		void stackPush(unsigned short reg);
		void stackPop(unsigned char &upperReg, unsigned char &lowerReg);

		void ADD(unsigned char value);
		void ADC(unsigned char value);
		void ADDHL(unsigned short reg);

		void SUB(unsigned char value);
		void SBC(unsigned char value);

		void AND(unsigned char value);
		void OR(unsigned char value);
		void XOR(unsigned char value);

		void CP(unsigned char value);

		void INC(unsigned char &reg);
		void INCHL();

		void DEC(unsigned char &reg);
		void DECHL();

		void SWAP(unsigned char &reg);
		void SWAPHL();

		void RLC(unsigned char &reg);
		void RL(unsigned char &reg);

		void RRC(unsigned char &reg);
		void RR(unsigned char &reg);

		void SLA(unsigned char &reg);

		void SRA(unsigned char &reg);

		void SRL(unsigned char &reg);

		void interruptHandler();

		void setIF(unsigned char newIF);

		void setIE(unsigned char newIE);

		void setTimer(unsigned char newTimer);

		unsigned char getTimer();

		void setDivider(unsigned short newDivider);

		unsigned short getDivider();

		void updateTimer(int cyclesElapsed);

		void addCycles(int cyclesToAdd);

		void setDMAStatus(bool newStatus);

		int getClocksElapsed();

		int getCyclesElapsed();

		void setRefreshClocksElapsed(int newRefreshClocksElapsed);

		int getRefreshClocksElapsed();

		int tick();

		int divtest;

		unsigned char opcode;				//Current opcode
		unsigned char A;					//Registers A - L
		unsigned char F;
		unsigned char B;
		unsigned char C;
		unsigned char D;
		unsigned char E;
		unsigned char H;
		unsigned char L;
		unsigned short AF;
		unsigned short BC;					//Registers B and C combined
		unsigned short DE;					//Registers D and E combined
		unsigned short HL;					//Registers H and L combined
		unsigned short SP;					//Stack pointer
		unsigned short PC;					//Program counter	
		unsigned char zFlag;				//Zero flag
		unsigned char nFlag;				//Subtract flag
		unsigned char hFlag;				//Half-carry flag
		unsigned char cFlag;				//Carry flag
		unsigned char IF;
		unsigned char IE;

		unsigned char CB_opcode;
		unsigned char immediate_8;
		unsigned char upperPC;
		unsigned char lowerPC;
		unsigned char upperSP;
		unsigned char lowerSP;
		unsigned short immediate_16;
		signed char signed_immediate_8;
		unsigned short prevPC;

		unsigned char tileOffset;
		int cycleCount;						//Cycles elapsed
		int speedModifier;
		int cyclesElapsed;
		int clocksElapsed;
		int timerClocksElapsed;
		int divClocksElapsed;
		int refreshClocksElapsed;
		int timerFreq;
		unsigned char timer;
		unsigned char DIV;
		bool halted;						//Halted status
		bool IME;
		bool IME_READY;
		bool interruptWaiting;
		bool useCondOpcodeCycles;
		bool poppedAF;
		bool dontJump;
		bool haltBug;
		bool DMA_Active;

		MMU &mmu;
		Debugger debugger;

		bool debuggerEnabled = false;
};

#endif