#include "CPU.h"
#include "opcode_cycle_counts.h"
#include "DAA_Values.h"
#include "bitwise.h"
#include <thread>

CPU::CPU(MMU &zbMMU) : mmu(zbMMU)
{
	A = 0x01;
	F = 0xB0;
	B = 0x00;
	C = 0x13;
	D = 0x00;
	E = 0xD8;
	H = 0x01;
	L = 0x4D;
	AF = A << 8 | F;
	BC = B << 8 | C;
	DE = D << 8 | E;
	HL = H << 8 | L;
	SP = 0xFFFE;
	PC = 0x0100;
	//PC = 0x0000;
	zFlag = 0x01;
	//nFlag = 0x01;
	hFlag = 0x01;
	cFlag = 0x01;
	tileOffset = 0x00;
	//PPU_Mode = 0;
	cycleCount = 0;
	cyclesElapsed = 0;
	clocksElapsed = 0;
	timerClocksElapsed = 0;
	divClocksElapsed = 0;
	refreshClocksElapsed = 0;
	prevPC = 0;
	halted = false;
	IME = false;
	IME_READY = false;
	useCondOpcodeCycles = false;
	speedModifier = 1;
	poppedAF = false;
	dontJump = false;
	haltBug = false;
	DMA_Active = false;

	timer = 0;
	DIV = 0xABCC;

	divtest = 0x00b2d5e6;
}

CPU::~CPU()
{

}

bool CPU::checkBit(int bitIndex, unsigned char byte)
{
	return (byte & (1 << (unsigned char)bitIndex)) != 0;
}

void CPU::loadRegister8(unsigned char &reg, unsigned char value)
{
	reg = value;
}

void CPU::loadRegister16(unsigned short &reg, unsigned short value)
{
	reg = value;
}

void CPU::stackPush(unsigned short reg)
{
	unsigned char upperReg = reg >> 8;
	unsigned char lowerReg = reg;

	SP--;
	mmu.writeMemory(SP, upperReg);
	SP--;
	mmu.writeMemory(SP, lowerReg);
}

void CPU::stackPop(unsigned char &upperReg, unsigned char &lowerReg)
{
	poppedAF ? lowerReg = mmu.readMemory(SP) & 0xF0 : lowerReg = mmu.readMemory(SP);
	SP++;
	upperReg = mmu.readMemory(SP);
	SP++;
}

void CPU::ADD(unsigned char value)
{
	unsigned int result;
	unsigned char tempReg = A;

	result = A + value;
	A = (unsigned char)result;

	nFlag = 0x00;
	A == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
	((tempReg & 0xF) + (value & 0xF) > 0xF) ? hFlag = 0x01 : hFlag = 0x00;
	(result & 0x100) != 0 ? cFlag = 0x01 : cFlag = 0x00;
}

void CPU::ADC(unsigned char value)
{
	unsigned int result;
	unsigned char tempReg = A;

	result = A + value + cFlag;
	A = (unsigned char)result;

	nFlag = 0x00;
	A == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
	((tempReg & 0xF) + (value & 0xF) + cFlag) > 0xF ? hFlag = 0x01 : hFlag = 0x00;
	result > 0xFF ? cFlag = 0x01 : cFlag = 0x00;
}

void CPU::ADDHL(unsigned short value)
{
	unsigned int totalResult;

	totalResult = HL + value;
	nFlag = 0x00;
	((HL & 0xFFF) + (value & 0xFFF) > 0xFFF) ? hFlag = 0x01 : hFlag = 0x00;
	((totalResult & 0x10000) != 0) ? cFlag = 0x01 : cFlag = 0x00;
	HL = (unsigned short)totalResult;
}

void CPU::SUB(unsigned char value)
{
	unsigned short result;

	result = A - value;
	nFlag = 0x01;
	result == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
	((A & 0xF) - (value & 0xF)) < 0 ? hFlag = 0x01 : hFlag = 0x00;
	A < value ? cFlag = 0x01 : cFlag = 0x00;
	A -= value;
}

void CPU::SBC(unsigned char value)
{
	int result;
	unsigned char tempReg = A;

	result = A - cFlag - value;
	A = (unsigned char)result;

	nFlag = 0x01;
	A == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
	((tempReg & 0xF) - (value & 0xF) - cFlag < 0) ? hFlag = 0x01 : hFlag = 0x00;
	result < 0 ? cFlag = 0x01 : cFlag = 0x00;
}

void CPU::AND(unsigned char value)
{
	unsigned short result;

	result = A & value;
	nFlag = 0x00;
	result == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
	hFlag = 0x01;
	cFlag = 0x00;
	A &= value;
}

void CPU::OR(unsigned char value)
{
	unsigned short result;

	result = A | value;
	nFlag = 0x00;
	result == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
	hFlag = 0x00;
	cFlag = 0x00;
	A |= value;
}

void CPU::XOR(unsigned char value)
{
	unsigned short result;

	result = A ^ value;
	nFlag = 0x00;
	result == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
	hFlag = 0x00;
	cFlag = 0x00;
	A ^= value;
}

void CPU::CP(unsigned char value)
{
	unsigned short result;

	result = A - value;
	nFlag = 0x01;
	result == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
	((A & 0xF) - (value & 0xF)) < 0 ? hFlag = 0x01 : hFlag = 0x00;
	A < value ? cFlag = 0x01 : cFlag = 0x00;
}

void CPU::INC(unsigned char &reg)
{
	unsigned char resultChar;
	unsigned short result;

	result = reg + 0x01;
	resultChar = reg + 0x01;
	nFlag = 0x00;
	resultChar == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
	(resultChar & 0x0F) == 0x00 ? hFlag = 0x01 : hFlag = 0x00;
	reg++;
}

void CPU::INCHL()
{
	unsigned char resultChar;

	resultChar = mmu.readMemory(HL) + 0x01;
	nFlag = 0x00;
	resultChar == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
	(resultChar & 0x0F) == 0x00 ? hFlag = 0x01 : hFlag = 0x00;
	mmu.writeMemory(HL, mmu.readMemory(HL) + 0x01);
}

void CPU::DEC(unsigned char &reg)
{
	unsigned char resultChar;

	resultChar = reg - 0x01;
	nFlag = 0x01;
	resultChar == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
	(resultChar & 0x0F) == 0x0F ? hFlag = 0x01 : hFlag = 0x00;
	reg--;
}

void CPU::DECHL()
{
	unsigned char result;
	result = (unsigned char)(mmu.readMemory(HL) - 0x01);
	
	nFlag = 0x01;
	result == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
	(result & 0x0F) == 0x0F ? hFlag = 0x01 : hFlag = 0x00;
	mmu.writeMemory(HL, result);
}

void CPU::SWAP(unsigned char &reg)
{
	unsigned char upperNibble, lowerNibble;

	nFlag = 0x00;
	hFlag = 0x00;
	cFlag = 0x00;
	upperNibble = (reg & 0xF0) >> 4;
	lowerNibble = (reg & 0x0F) << 4;
	reg = lowerNibble | upperNibble;
	reg == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
}

void CPU::SWAPHL()
{
	unsigned char upperNibble, lowerNibble;

	nFlag = 0x00;
	hFlag = 0x00;
	cFlag = 0x00;
	upperNibble = (mmu.readMemory(HL) & 0xF0) >> 4;
	lowerNibble = (mmu.readMemory(HL) & 0x0F) << 4;
	mmu.writeMemory(HL, (lowerNibble | upperNibble));
	mmu.readMemory(HL) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
}

void CPU::RLC(unsigned char &reg)
{
	unsigned char tempChar, resultChar;

	nFlag = 0x00;
	hFlag = 0x00;

	((reg & 0x80) != 0x00) ? tempChar = 0x01 : tempChar = 0x00;
	resultChar = (unsigned char)((reg << 1) | tempChar);
	resultChar == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
	tempChar == 0x01 ? cFlag = 0x01 : cFlag = 0x00;

	reg = resultChar;
}

void CPU::RL(unsigned char &reg)
{
	unsigned char tempChar = cFlag;
	unsigned char resultChar, result;

	nFlag = 0x00;
	hFlag = 0x00;

	((reg & 0x80) != 0x00) ? cFlag = 0x01 : cFlag = 0x00;
	resultChar = (unsigned char)(reg << 1);
	resultChar |= tempChar;
	resultChar == 0x00 ? zFlag = 0x01 : zFlag = 0x00;

	reg = resultChar;
}

void CPU::RRC(unsigned char &reg)
{
	unsigned char resultChar;

	nFlag = 0x00;
	hFlag = 0x00;
	cFlag = reg & 0x01;
	resultChar = reg >> 1;
	resultChar |= cFlag << 7;
	resultChar == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
	reg = resultChar;
}

void CPU::RR(unsigned char &reg)
{
	unsigned char tempChar, resultChar;

	nFlag = 0x00;
	hFlag = 0x00;
	tempChar = cFlag;
	cFlag = reg & 0x01;
	resultChar = reg >> 1;
	resultChar |= tempChar << 7;
	resultChar == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
	reg = resultChar;
}

void CPU::SLA(unsigned char &reg)
{
	nFlag = 0x00;
	hFlag = 0x00;
	cFlag = reg & 0x80;
	cFlag >>= 7;
	reg <<= 1;
	reg &= 0xFE;
	reg == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
}

void CPU::SRA(unsigned char &reg)
{
	unsigned char tempChar, finalBit, resultChar;

	nFlag = 0x00;
	hFlag = 0x00;

	((reg & 0x01) != 0x00) ? tempChar = 0x01 : tempChar = 0x00;
	((reg & 0x80) != 0x00) ? finalBit = 0x01 : finalBit = 0x00;
	resultChar = (unsigned char)(reg >> 1);
	resultChar = bitwise::set_bit_to(resultChar, 7, finalBit);

	resultChar == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
	tempChar == 0x01 ? cFlag = 0x01 : cFlag = 0x00;
	reg = resultChar;
}

void CPU::SRL(unsigned char &reg)
{
	nFlag = 0x00;
	hFlag = 0x00;
	cFlag = reg & 0x01;
	reg >>= 1;
	reg &= 0x7F;
	reg == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
}

void CPU::interruptHandler()
{
	//cyclesElapsed += 5;

	if (IME_READY)
	{
		IME = true;
		IME_READY = false;
	}
	
	/*
	if ((*(mmu.IF) & 0x01) != 0)			//V-blank
	{
		*(mmu.IE) = 0x01;
	}

	else if ((*(mmu.IF) & 0x02) != 0)		//LCDC status interrupt
	{
		*(mmu.IE) = 0x02;
	}

	else if ((*(mmu.IF) & 0x04) != 0)		//Timer overflow
	{
		*(mmu.IE) = 0x04;
	}

	else if ((*(mmu.IF) & 0x08) != 0)		//Serial transfer completion
	{
		*(mmu.IE) = 0x08;
	}

	else if ((*(mmu.IF) & 0x10) != 0)		//P10-P13 input signal goes low
	{
		*(mmu.IE) = 0x10;
	}
	*/

	/*switch (IF)
	{
		case 0xE1:
			IE = 0x01;
			break;

		case 0xE2:
			IE = 0x02;
			break;

		case 0xE4:
			IE = 0x04;
			break;

		case 0xE8:
			IE = 0x08;
			break;

		case 0xF0:
			IE = 0x10;
			break;

		default:
			IE = 0x00;
			break;
	}*/

	if (IME || halted)
	{
		//if (IE == 0x01 && IF == 0xE1)
		if (bitwise::check_bit(*(mmu.IE), 0) && bitwise::check_bit(*(mmu.IF), 0))
		{
			/*if (SP < 0xFF80 || SP > 0xFFFE)
			{
				PC = SP;
				SP = 0xFFFE;
				mmu.writeMemory(0xFFFF, mmu.readMemory(0xFFFF) >> 1);
				return;
			}*/

			if (!dontJump)
			{

				mmu.writeMemory(0xFF0F, 0xE0);		//Upper 

				IME = false;
				mmu.writeMemory(SP - 0x0001, upperPC);
				mmu.writeMemory(SP - 0x0002, lowerPC + (PC - prevPC));
				SP -= 2;
				PC = 0x0040;
			}

			else
			{
				dontJump = false;
			}

			halted ? cyclesElapsed = 6 : cyclesElapsed = 5;
			halted = false;

			mmu.writeMemory(0xFF0F, 0xE0);
		}

		else if (bitwise::check_bit(*(mmu.IE), 1) && bitwise::check_bit(*(mmu.IF), 1))
		//else if (IE == 0x02 && IF == 0xE2)
		{
			/*if (SP < 0xFF80 || SP > 0xFFFE)
			{
				PC = SP;
				SP = 0xFFFE;
				mmu.writeMemory(0xFFFF, mmu.readMemory(0xFFFF) >> 1);
				return;
			}*/

			if (!dontJump)
			{
				mmu.writeMemory(0xFF0F, 0xE0);		//Upper 

				IME = false;
				mmu.writeMemory(SP - 0x0001, upperPC);
				mmu.writeMemory(SP - 0x0002, lowerPC + (PC - prevPC));
				SP -= 2;
				PC = 0x0048;
			}

			else
			{
				dontJump = false;
			}

			halted ? cyclesElapsed = 6 : cyclesElapsed = 5;
			halted = false;

			mmu.writeMemory(0xFF0F, 0xE0);
		}

		else if (bitwise::check_bit(*(mmu.IE), 2) && bitwise::check_bit(*(mmu.IF), 2))
		//else if (IE == 0x04 && IF == 0xE4)
		{
			/*if (SP < 0xFF80 || SP > 0xFFFE)
			{
				PC = SP;
				SP = 0xFFFE;
				mmu.writeMemory(0xFFFF, mmu.readMemory(0xFFFF) >> 1);
				return;
			}*/

			if (!dontJump)
			{
				mmu.writeMemory(0xFF0F, 0xE0);		//Upper 

				IME = false;
				mmu.writeMemory(SP - 0x0001, upperPC);
				mmu.writeMemory(SP - 0x0002, lowerPC + (PC - prevPC));
				SP -= 2;
				PC = 0x0050;
			}

			else
			{
				dontJump = false;
			}

			halted ? cyclesElapsed = 6 : cyclesElapsed = 5;
			halted = false;

			mmu.writeMemory(0xFF0F, 0xE0);
		}

		else if (bitwise::check_bit(*(mmu.IE), 3) && bitwise::check_bit(*(mmu.IF), 3))
		//else if (IE == 0x08 && IF == 0xE8)
		{
			/*if (SP < 0xFF80 || SP > 0xFFFE)
			{
				PC = SP;
				SP = 0xFFFE;
				mmu.writeMemory(0xFFFF, mmu.readMemory(0xFFFF) >> 1);
				return;
			}*/

			if (!dontJump)
			{
				mmu.writeMemory(0xFF0F, 0xE0);		//Upper 

				IME = false;
				mmu.writeMemory(SP - 0x0001, upperPC);
				mmu.writeMemory(SP - 0x0002, lowerPC + (PC - prevPC));
				SP -= 2;
				PC = 0x0058;
			}

			else
			{
				dontJump = false;
			}

			halted ? cyclesElapsed = 6 : cyclesElapsed = 5;
			halted = false;

			mmu.writeMemory(0xFF0F, 0xE0);
		}

		else if (bitwise::check_bit(*(mmu.IE), 4) && bitwise::check_bit(*(mmu.IF), 4))
		{
			/*if (SP < 0xFF80 || SP > 0xFFFE)
			{
				PC = SP;
				SP = 0xFFFE;
				mmu.writeMemory(0xFFFF, mmu.readMemory(0xFFFF) >> 1);
				return;
			}*/

			if (!dontJump)
			{
				//mmu.writeMemory(0xFF0F, 0xF0);		//Upper 

				IME = false;
				mmu.writeMemory(SP - 0x0001, upperPC);
				mmu.writeMemory(SP - 0x0002, lowerPC + (PC - prevPC));
				SP -= 2;
				PC = 0x0060;
			}

			else
			{
				dontJump = false;
			}

			halted ? cyclesElapsed = 6 : cyclesElapsed = 5;
			halted = false;

			mmu.writeMemory(0xFF0F, 0xE0);
		}
	}

	/*
	if (IME)
	{
		mmu.writeMemory(0xFF0F, 0xE0);		//Upper 

		switch (IE)
		{
			case 0x01:							//V-blank
				IME = false;
				mmu.writeMemory(SP - 0x0001, upperPC);
				mmu.writeMemory(SP - 0x0002, lowerPC + 2);
				SP -= 2;
				PC = 0x0040;
				break;

			case 0x02:							//LCDC status interrupt
				IME = false;
				mmu.writeMemory(SP - 0x0001, upperPC);
				mmu.writeMemory(SP - 0x0002, lowerPC + 2);
				SP -= 2;
				PC = 0x0048;
				break;

			case 0x04:							//Timer overflow
				IME = false;
				mmu.writeMemory(SP - 0x0001, upperPC);
				mmu.writeMemory(SP - 0x0002, lowerPC + 2);
				SP -= 2;
				PC = 0x0050;
				break;

			case 0x08:							//Serial transfer completion
				IME = false;
				mmu.writeMemory(SP - 0x0001, upperPC);
				mmu.writeMemory(SP - 0x0002, lowerPC + 2);
				SP -= 2;
				PC = 0x0058;
				break;

			case 0x10:							//P10-P13 input signal goes low
				IME = false;
				mmu.writeMemory(SP - 0x0001, upperPC);
				mmu.writeMemory(SP - 0x0002, lowerPC + 2);
				SP -= 2;
				PC = 0x0060;
				break;

			default:
				break;
		}

		*(mmu.IE) = 0x00;
		IE = 0x00;
	}
	*/
}

void CPU::setIF(unsigned char newIF)
{
	IF = newIF;
	//IF |= 0xE0;		//upper 3 bits remain set
}

void CPU::setIE(unsigned char newIE)
{
	IE = newIE;
}

void CPU::setTimer(unsigned char newTimer)
{
	timer = newTimer;
}

unsigned char CPU::getTimer()
{
	return timer;
}

void CPU::setDivider(unsigned short newDIV)
{
	DIV = newDIV;
}

unsigned short CPU::getDivider()
{
	return DIV;
}

void CPU::updateTimer(int cyclesElapsed)
{
	unsigned char currentTIMA = *(mmu.TIMA);
	unsigned char currentTMA = *(mmu.TMA);
	unsigned char currentTAC = *(mmu.TAC);

	unsigned char prevTIMA = *(mmu.TIMA);

	bool incrementTIMA = false;

	if (mmu.TIMAOverflow)
	{
		mmu.writeMemory(0xFF05, mmu.readMemory(0xFF06));
		//mmu.writeMemory(0xFF0F, mmu.readMemory(0xFF0F) | 0x04);
		mmu.TIMAOverflow = false;
	}

	if (bitwise::check_bit(*(mmu.TAC), 2))
	{
		timerClocksElapsed += cyclesElapsed;

		if (!bitwise::check_bit(*(mmu.TAC), 1) && !bitwise::check_bit(*(mmu.TAC), 0) && timerClocksElapsed >= 256)
		{
			timerClocksElapsed -= 256;
			incrementTIMA = true;
		}

		else if (!bitwise::check_bit(*(mmu.TAC), 1) && bitwise::check_bit(*(mmu.TAC), 0) && timerClocksElapsed >= 4)
		{
			timerClocksElapsed -= 4;
			incrementTIMA = true;
		}

		else if (bitwise::check_bit(*(mmu.TAC), 1) && !bitwise::check_bit(*(mmu.TAC), 0) && timerClocksElapsed >= 16)
		{
			timerClocksElapsed -= 16;
			incrementTIMA = true;
		}

		else if(bitwise::check_bit(*(mmu.TAC), 1) && bitwise::check_bit(*(mmu.TAC), 0) && timerClocksElapsed >= 64)
		{
			timerClocksElapsed -= 64;
			incrementTIMA = true;
		}
	}

	if (incrementTIMA)
	{
		prevTIMA = mmu.readMemory(0xFF05);
		mmu.writeMemory(0xFF05, mmu.readMemory(0xFF05) + 1);

		if (prevTIMA > mmu.readMemory(0xFF05))
			mmu.TIMAOverflow = true;
	}
}

void CPU::addCycles(int cyclesToAdd)
{
	cyclesElapsed += cyclesToAdd;
}

void CPU::setDMAStatus(bool newStatus)
{
	DMA_Active = newStatus;
}

int CPU::getClocksElapsed()
{
	return clocksElapsed;
}

int CPU::getCyclesElapsed()
{
	return cyclesElapsed;
}

void CPU::setRefreshClocksElapsed(int newRefreshClocksElapsed)
{
	refreshClocksElapsed = newRefreshClocksElapsed;
}

int CPU::getRefreshClocksElapsed()
{
	return refreshClocksElapsed;
}

int CPU::tick()
{
	unsigned char resultChar, tempChar;
	unsigned char upperNibble, lowerNibble;
	unsigned short subResult, totalResult, result;

	//cyclesElapsed = 0;

	if (halted)
	{
		if (IME)
		{
			//Regular halt
		}

		else
		{
			if ((*(mmu.IE) == 0x00 || *(mmu.IE) == 0xE0) && (*(mmu.IF) == 0x00 || *(mmu.IF) == 0xE0))
			{
				dontJump = true;
			}

			else if(*(mmu.IE) != 0x00 && *(mmu.IE) != 0xE0 && *(mmu.IF) != 0x00 && *(mmu.IF) != 0xE0)
			{
				halted = false;
				haltBug = true;
				prevPC = PC;
				cyclesElapsed++;
			}
		}
	}

	interruptHandler();

	/*
	ss << std::hex << std::setfill('0') << std::setw(4) << PC;
	debugStrings.push_back("PC:" + ss.str() + " (cy: " + std::to_string(refreshClocksElapsed) + ")\n");
	ss.str("");
	ss.clear();
	*/

	opcode = mmu.readMemory(PC);
	CB_opcode = mmu.readMemory(PC + 1);
	immediate_8 = mmu.readMemory(PC + 1);
	immediate_16 = (mmu.readMemory(PC + 2) << 8) | mmu.readMemory(PC + 1);
	signed_immediate_8 = (signed char)immediate_8;
	upperSP = SP >> 8;
	lowerSP = SP;
	upperPC = PC >> 8;
	lowerPC = PC;

	AF = A << 8 | F;
	BC = B << 8 | C;
	DE = D << 8 | E;
	HL = H << 8 | L;

	if (debuggerEnabled)
	{
		//debugger.printMessage(*this);
	}

	//For checking button polling in Tetris
	//unsigned short testPC = 0x29A6;

	unsigned short testPC = 0x0183;
	unsigned short testPC2 = 0x0000;

	if (PC == testPC || PC == testPC2)
	{
		PC = PC == testPC ? testPC : testPC2;
	}

	if (halted)
	{
		return 1;
	}

	if (DMA_Active)
	{
		mmu.tickDMA();
	}

	mmu.setDIV_WRITE(false);

	cyclesElapsed = 0;
	prevPC = PC;

	switch (opcode)
	{
		case 0x00:		//NOP
			PC++;
			break;

		case 0x06:
			loadRegister8(B, immediate_8);
			PC += 2;
			break;

		case 0x0E:
			loadRegister8(C, immediate_8);
			PC += 2;
			break;

		case 0x16:
			loadRegister8(D, immediate_8);
			PC += 2;
			break;

		case 0x1E:
			loadRegister8(E, immediate_8);
			PC += 2;
			break;

		case 0x26:
			loadRegister8(H, immediate_8);
			PC += 2;
			break;

		case 0x2E:
			loadRegister8(L, immediate_8);
			PC += 2;
			break;

		case 0x7F:		//LD A, A
			loadRegister8(A, A);
			PC++;
			break;

		case 0x78:		//LD A, B
			loadRegister8(A, B);
			PC++;
			break;

		case 0x79:		//LD A, C
			loadRegister8(A, C);
			PC++;
			break;

		case 0x7A:		//LD A, D
			loadRegister8(A, D);
			PC++;
			break;

		case 0x7B:		//LD A, E
			loadRegister8(A, E);
			PC++;
			break;

		case 0x7C:		//LD A, H
			loadRegister8(A, H);
			PC++;
			break;

		case 0x7D:		//LD A, L
			loadRegister8(A, L);
			PC++;
			break;

		case 0x40:		//LD B, B
			loadRegister8(B, B);
			PC++;
			break;

		case 0x41:		//LD B, C
			loadRegister8(B, C);
			PC++;
			break;

		case 0x42:		//LD B, D
			loadRegister8(B, D);
			PC++;
			break;

		case 0x43:		//LD B, E
			loadRegister8(B, E);
			PC++;
			break;

		case 0x44:		//LD B, H
			loadRegister8(B, H);
			PC++;
			break;

		case 0x45:		//LD B, L
			loadRegister8(B, L);
			PC++;
			break;

		case 0x46:		//LD B, (HL)
			loadRegister8(B, mmu.readMemory(HL));
			PC++;
			break;

		case 0x48:		//LD C, B
			loadRegister8(C, B);
			PC++;
			break;

		case 0x49:		//LD C, C
			loadRegister8(C, C);
			PC++;
			break;

		case 0x4A:		//LD C, D
			loadRegister8(C, D);
			PC++;
			break;

		case 0x4B:		//LD C, E
			loadRegister8(C, E);
			PC++;
			break;

		case 0x4C:		//LD C, H
			loadRegister8(C, H);
			PC++;
			break;

		case 0x4D:		//LD C, L
			loadRegister8(C, L);
			PC++;
			break;

		case 0x4E:		//LD C, (HL)
			loadRegister8(C, mmu.readMemory(HL));
			PC++;
			break;

		case 0x50:		//LD D, B
			loadRegister8(D, B);
			PC++;
			break;

		case 0x51:		//LD D, C
			loadRegister8(D, C);
			PC++;
			break;

		case 0x52:		//LD D, D
			loadRegister8(D, D);
			PC++;
			break;

		case 0x53:		//LD D, E
			loadRegister8(D, E);
			PC++;
			break;

		case 0x54:		//LD D, H
			loadRegister8(D, H);
			PC++;
			break;

		case 0x55:		//LD D, L
			loadRegister8(D, L);
			PC++;
			break;

		case 0x56:		//LD D, (HL)
			loadRegister8(D, mmu.readMemory(HL));
			PC++;
			break;

		case 0x58:		//LD E, B
			loadRegister8(E, B);
			PC++;
			break;

		case 0x59:		//LD E, C
			loadRegister8(E, C);
			PC++;
			break;

		case 0x5A:		//LD E, D
			loadRegister8(E, D);
			PC++;
			break;

		case 0x5B:		//LD E, E
			loadRegister8(E, E);
			PC++;
			break;

		case 0x5C:		//LD E, H
			loadRegister8(E, H);
			PC++;
			break;

		case 0x5D:		//LD E, L
			loadRegister8(E, L);
			PC++;
			break;

		case 0x5E:		//LD E, (HL)
			loadRegister8(E, mmu.readMemory(HL));
			PC++;
			break;

		case 0x60:		//LD H, B
			loadRegister8(H, B);
			PC++;
			break;

		case 0x61:		//LD H, C
			loadRegister8(H, C);
			PC++;
			break;

		case 0x62:		//LD H, D
			loadRegister8(H, D);
			PC++;
			break;

		case 0x63:		//LD H, E
			loadRegister8(H, E);
			PC++;
			break;

		case 0x64:		//LD H, H
			loadRegister8(H, H);
			PC++;
			break;

		case 0x65:		//LD H, L
			loadRegister8(H, L);
			PC++;
			break;

		case 0x66:		//LD H, (HL)
			loadRegister8(H, mmu.readMemory(HL));
			PC++;
			break;

		case 0x68:		//LD L, B
			loadRegister8(L, B);
			PC++;
			break;

		case 0x69:		//LD L, C
			loadRegister8(L, C);
			PC++;
			break;

		case 0x6A:		//LD L, D
			loadRegister8(L, D);
			PC++;
			break;

		case 0x6B:		//LD L, E
			loadRegister8(L, E);
			PC++;
			break;

		case 0x6C:		//LD L, H
			loadRegister8(L, H);
			PC++;
			break;

		case 0x6D:		//LD L, L
			loadRegister8(L, L);
			PC++;
			break;

		case 0x6E:		//LD L, (HL)
			loadRegister8(L, mmu.readMemory(HL));
			PC++;
			break;

		case 0x70:		//LD (HL), B
			mmu.writeMemory(HL, B);
			PC++;
			break;

		case 0x71:		//LD (HL), C
			mmu.writeMemory(HL, C);
			PC++;
			break;

		case 0x72:		//LD (HL), D
			mmu.writeMemory(HL, D);
			PC++;
			break;

		case 0x73:		//LD (HL), E
			mmu.writeMemory(HL, E);
			PC++;
			break;

		case 0x74:		//LD (HL), H
			mmu.writeMemory(HL, H);
			PC++;
			break;

		case 0x75:		//LD (HL), L
			mmu.writeMemory(HL, L);
			PC++;
			break;

		case 0x36:		//LD (HL), n
			mmu.writeMemory(HL, immediate_8);
			PC += 2;
			break;

		case 0x0A:		//LD A, (BC)
			loadRegister8(A, mmu.readMemory(BC));
			PC++;
			break;

		case 0x1A:		//LD A, (DE)
			loadRegister8(A, mmu.readMemory(DE));
			PC++;
			break;

		case 0x7E:		//LD A, (HL)
			loadRegister8(A, mmu.readMemory(HL));
			PC++;
			break;

		case 0xFA:		//LD A, (nn)
			loadRegister8(A, mmu.readMemory(immediate_16));
			PC += 3;
			break;

		case 0x3E:
			loadRegister8(A, immediate_8);
			PC += 2;
			break;

		case 0x47:
			loadRegister8(B, A);
			PC++;
			break;

		case 0x4F:
			loadRegister8(C, A);
			PC++;
			break;

		case 0x57:
			loadRegister8(D, A);
			PC++;
			break;

		case 0x5F:
			loadRegister8(E, A);
			PC++;
			break;

		case 0x67:
			loadRegister8(H, A);
			PC++;
			break;

		case 0x6F:
			loadRegister8(L, A);
			PC++;
			break;

		case 0x02:
			mmu.writeMemory(BC, A);
			PC++;
			break;

		case 0x12:
			mmu.writeMemory(DE, A);
			PC++;
			break;

		case 0x77:
			mmu.writeMemory(HL, A);
			PC++;
			break;

		case 0xEA:
			mmu.writeMemory(immediate_16, A);
			PC += 3;
			break;

		case 0xF2:
			loadRegister8(A, mmu.readMemory(0xFF00 + C));
			PC++;
			break;

		case 0xE2:
			mmu.writeMemory(0xFF00 + C, A);
			PC++;
			break;

		case 0x32:
			mmu.writeMemory(HL, A);
			HL--;
			PC++;
			break;

		case 0x3A:
			//mmu.writeMemory(HL, A);
			A = mmu.readMemory(HL);
			HL--;
			PC++;
			break;

		case 0x2A:
			loadRegister8(A, mmu.readMemory(HL));
			HL++;
			PC++;
			break;

		case 0x22:
			mmu.writeMemory(HL, A);
			HL++;
			PC++;
			break;

		case 0xE0:
			mmu.writeMemory(0xFF00 + immediate_8, A);
			PC += 2;
			break;

		case 0xF0:		//NOTE: changed from 2 cycles to 3
			loadRegister8(A, mmu.readMemory(0xFF00 + immediate_8));
			PC += 2;
			break;

		case 0x01:
			loadRegister16(BC, immediate_16);
			PC += 3;
			break;

		case 0x11:
			loadRegister16(DE, immediate_16);
			PC += 3;
			break;

		case 0x21:
			loadRegister16(HL, immediate_16);
			PC += 3;
			break;

		case 0x31:
			loadRegister16(SP, immediate_16);
			PC += 3;
			break;

		case 0xF9:
			loadRegister16(SP, HL);
			PC++;
			break;

		case 0xF8:
			totalResult = SP + signed_immediate_8;
			zFlag = 0x00;
			nFlag = 0x00;
			(((SP ^ signed_immediate_8 ^ (totalResult & 0xFFFF)) & 0x10) == 0x10) ? hFlag = 0x01 : hFlag = 0x00;
			(((SP ^ signed_immediate_8 ^ (totalResult & 0xFFFF)) & 0x100) == 0x100) ? cFlag = 0x01 : cFlag = 0x00;
			HL = totalResult;
			PC += 2;
			break;

		case 0x08:
			mmu.writeMemory(immediate_16, lowerSP);
			mmu.writeMemory(immediate_16 + 0x0001, upperSP);
			PC += 3;
			break;

		case 0xF5:
			stackPush(AF);
			PC++;
			break;

		case 0xC5:
			stackPush(BC);
			PC++;
			break;

		case 0xD5:
			stackPush(DE);
			PC++;
			break;

		case 0xE5:
			stackPush(HL);
			PC++;
			break;

		case 0xF1:
			poppedAF = true;
			stackPop(A, F);
			PC++;
			break;

		case 0xC1:
			stackPop(B, C);
			PC++;
			break;

		case 0xD1:
			stackPop(D, E);
			PC++;
			break;

		case 0xE1:
			stackPop(H, L);
			PC++;
			break;

		case 0x87:
			ADD(A);
			PC++;
			break;

		case 0x80:
			ADD(B);
			PC++;
			break;

		case 0x81:
			ADD(C);
			PC++;
			break;

		case 0x82:
			ADD(D);
			PC++;
			break;

		case 0x83:
			ADD(E);
			PC++;
			break;

		case 0x84:
			ADD(H);
			PC++;
			break;

		case 0x85:
			ADD(L);
			PC++;
			break;

		case 0x86:
			ADD(mmu.readMemory(HL));
			//ADDHL(A);
			PC++;
			break;

		case 0xC6:
			ADD(immediate_8);
			PC += 2;
			break;

		case 0x8F:
			ADC(A);
			PC++;
			break;

		case 0x88:
			ADC(B);
			PC++;
			break;

		case 0x89:
			ADC(C);
			PC++;
			break;

		case 0x8A:
			ADC(D);
			PC++;
			break;

		case 0x8B:
			ADC(E);
			PC++;
			break;

		case 0x8C:
			ADC(H);
			PC++;
			break;

		case 0x8D:
			ADC(L);
			PC++;
			break;

		case 0x8E:
			ADC(mmu.readMemory(HL));
			PC++;
			break;

		case 0xCE:
			ADC(immediate_8);
			PC += 2;
			break;

		case 0x97:
			SUB(A);
			PC++;
			break;

		case 0x90:
			SUB(B);
			PC++;
			break;

		case 0x91:
			SUB(C);
			PC++;
			break;

		case 0x92:
			SUB(D);
			PC++;
			break;

		case 0x93:
			SUB(E);
			PC++;
			break;

		case 0x94:
			SUB(H);
			PC++;
			break;

		case 0x95:
			SUB(L);
			PC++;
			break;

		case 0x96:
			SUB(mmu.readMemory(HL));
			PC++;
			break;

		case 0xD6:
			SUB(immediate_8);
			PC += 2;
			break;

		case 0x9F:
			SBC(A);
			PC++;
			break;

		case 0x98:
			SBC(B);
			PC++;
			break;

		case 0x99:
			SBC(C);
			PC++;
			break;

		case 0x9A:
			SBC(D);
			PC++;
			break;

		case 0x9B:
			SBC(E);
			PC++;
			break;

		case 0x9C:
			SBC(H);
			PC++;
			break;

		case 0x9D:
			SBC(L);
			PC++;
			break;

		case 0x9E:
			SBC(mmu.readMemory(HL));
			PC++;
			break;

		case 0xDE:
			SBC(immediate_8);
			PC += 2;
			break;

		case 0xA7:
			AND(A);
			PC++;
			break;

		case 0xA0:
			AND(B);
			PC++;
			break;

		case 0xA1:
			AND(C);
			PC++;
			break;

		case 0xA2:
			AND(D);
			PC++;
			break;

		case 0xA3:
			AND(E);
			PC++;
			break;

		case 0xA4:
			AND(H);
			PC++;
			break;

		case 0xA5:
			AND(L);
			PC++;
			break;

		case 0xA6:
			AND(mmu.readMemory(HL));
			PC++;
			break;

		case 0xE6:
			AND(immediate_8);
			PC += 2;
			break;

		case 0xB7:
			OR(A);
			PC++;
			break;

		case 0xB0:
			OR(B);
			PC++;
			break;

		case 0xB1:
			OR(C);
			PC++;
			break;

		case 0xB2:
			OR(D);
			PC++;
			break;

		case 0xB3:
			OR(E);
			PC++;
			break;

		case 0xB4:
			OR(H);
			PC++;
			break;

		case 0xB5:
			OR(L);
			PC++;
			break;

		case 0xB6:
			OR(mmu.readMemory(HL));
			PC++;
			break;

		case 0xF6:
			OR(immediate_8);
			PC += 2;
			break;

		case 0xAF:
			XOR(A);
			PC++;
			break;

		case 0xA8:
			XOR(B);
			PC++;
			break;

		case 0xA9:
			XOR(C);
			PC++;
			break;

		case 0xAA:
			XOR(D);
			PC++;
			break;

		case 0xAB:
			XOR(E);
			PC++;
			break;

		case 0xAC:
			XOR(H);
			PC++;
			break;

		case 0xAD:
			XOR(L);
			PC++;
			break;

		case 0xAE:
			XOR(mmu.readMemory(HL));
			PC++;
			break;

		case 0xEE:
			XOR(immediate_8);
			PC += 2;
			break;

		case 0xBF:
			CP(A);
			PC++;
			break;

		case 0xB8:
			CP(B);
			PC++;
			break;

		case 0xB9:
			CP(C);
			PC++;
			break;

		case 0xBA:
			CP(D);
			PC++;
			break;

		case 0xBB:
			CP(E);
			PC++;
			break;

		case 0xBC:
			CP(H);
			PC++;
			break;

		case 0xBD:
			CP(L);
			PC++;
			break;

		case 0xBE:
			CP(mmu.readMemory(HL));
			PC++;
			break;

		case 0xFE:
			CP(immediate_8);
			PC += 2;
			break;

		case 0x3C:
			INC(A);
			PC++;
			break;

		case 0x04:
			INC(B);
			PC++;
			break;

		case 0x0C:
			INC(C);
			PC++;
			break;

		case 0x14:
			INC(D);
			PC++;
			break;

		case 0x1C:
			INC(E);
			PC++;
			break;

		case 0x24:
			INC(H);
			PC++;
			break;

		case 0x2C:
			INC(L);
			PC++;
			break;

		case 0x34:
			INCHL();
			PC++;
			break;

		case 0x3D:
			DEC(A);
			PC++;
			break;

		case 0x05:
			DEC(B);
			PC++;
			break;

		case 0x0D:
			DEC(C);
			PC++;
			break;

		case 0x15:
			DEC(D);
			PC++;
			break;

		case 0x1D:
			DEC(E);
			PC++;
			break;

		case 0x25:
			DEC(H);
			PC++;
			break;

		case 0x2D:
			DEC(L);
			PC++;
			break;

		case 0x35:
			DECHL();
			PC++;
			break;

		case 0x09:
			ADDHL(BC);
			PC++;
			break;

		case 0x19:
			ADDHL(DE);
			PC++;
			break;

		case 0x29:
			ADDHL(HL);
			PC++;
			break;

		case 0x39:
			ADDHL(SP);
			PC++;
			break;

		case 0xE8:
			totalResult = SP + signed_immediate_8;
			zFlag = 0x00;
			nFlag = 0x00;
			((SP ^ signed_immediate_8 ^ (totalResult & 0xFFFF)) & 0x10) == 0x10 ? hFlag = 0x01 : hFlag = 0x00;
			((SP ^ signed_immediate_8 ^ (totalResult & 0xFFFF)) & 0x100) == 0x100 ? cFlag = 0x01 : cFlag = 0x00;
			SP += signed_immediate_8;
			PC += 2;
			break;

		case 0x03:
			BC++;
			PC++;
			break;

		case 0x13:
			DE++;
			PC++;
			break;

		case 0x23:
			HL++;
			PC++;
			break;

		case 0x33:
			SP++;
			PC++;
			break;

		case 0x0B:
			BC--;
			PC++;
			break;

		case 0x1B:
			DE--;
			PC++;
			break;

		case 0x2B:
			HL--;
			PC++;
			break;

		case 0x3B:
			SP--;
			PC++;
			break;

		case 0xCB:
			switch (CB_opcode)
			{
				case 0x37:
					SWAP(A);
					PC++;
					break;

				case 0x30:
					SWAP(B);
					PC++;
					break;

				case 0x31:
					SWAP(C);
					PC++;
					break;

				case 0x32:
					SWAP(D);
					PC++;
					break;

				case 0x33:
					SWAP(E);
					PC++;
					break;

				case 0x34:
					SWAP(H);
					PC++;
					break;

				case 0x35:
					SWAP(L);
					PC++;
					break;

				case 0x36:
					SWAPHL();
					PC++;
					break;

				case 0x07:		//RLC A
					RLC(A);
					PC++;
					break;

				case 0x00:		//RLC B
					RLC(B);
					PC++;
					break;

				case 0x01:		//RLC C
					RLC(C);
					PC++;
					break;

				case 0x02:		//RLC D
					RLC(D);
					PC++;
					break;

				case 0x03:		//RLC E
					RLC(E);
					PC++;
					break;

				case 0x04:		//RLC H
					RLC(H);
					PC++;
					break;

				case 0x05:		//RLC L
					RLC(L);
					PC++;
					break;

				case 0x06:		//RLC (HL)
					nFlag = 0x00;
					hFlag = 0x00;
					cFlag = mmu.readMemory(HL) & 0x80;
					cFlag >>= 7;
					resultChar = mmu.readMemory(HL) << 1;
					resultChar |= cFlag;
					resultChar == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					mmu.writeMemory(HL, resultChar);
					PC++;
					break;

				case 0x17:		//RL A
					RL(A);
					PC++;
					break;

				case 0x10:		//RL B
					RL(B);
					PC++;
					break;

				case 0x11:		//RL C
					RL(C);
					PC++;
					break;

				case 0x12:		//RL D
					RL(D);
					PC++;
					break;

				case 0x13:		//RL E
					RL(E);
					PC++;
					break;

				case 0x14:		//RL H
					RL(H);
					PC++;
					break;

				case 0x15:		//RL L
					RL(L);
					PC++;
					break;

				case 0x16:		//RL (HL)
					nFlag = 0x00;
					hFlag = 0x00;
					tempChar = cFlag;
					((mmu.readMemory(HL) & 0x80) != 0) ? resultChar = 0x01 : resultChar = 0x00;
					resultChar == 0x01 ? cFlag = 0x01 : cFlag = 0x00;
					result = (unsigned char)(mmu.readMemory(HL) << 1);
					result |= tempChar;
					result == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					mmu.writeMemory(HL, result);
					PC++;
					break;

				case 0x0F:		//RRC A
					RRC(A);
					PC++;
					break;

				case 0x08:		//RRC B
					RRC(B);
					PC++;
					break;

				case 0x09:		//RRC C
					RRC(C);
					PC++;
					break;

				case 0x0A:		//RRC D
					RRC(D);
					PC++;
					break;

				case 0x0B:		//RRC E
					RRC(E);
					PC++;
					break;

				case 0x0C:		//RRC H
					RRC(H);
					PC++;
					break;

				case 0x0D:		//RRC L
					RRC(L);
					PC++;
					break;

				case 0x0E:		//RRC (HL)
					nFlag = 0x00;
					hFlag = 0x00;
					cFlag = mmu.readMemory(HL) & 0x01;
					resultChar = mmu.readMemory(HL) >> 1;
					resultChar |= cFlag << 7;
					resultChar == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					mmu.writeMemory(HL, resultChar);
					PC++;
					break;

				case 0x1F:		//RR A
					RR(A);
					PC++;
					break;

				case 0x18:		//RR B
					RR(B);
					PC++;
					break;

				case 0x19:		//RR C
					RR(C);
					PC++;
					break;

				case 0x1A:		//RR D
					RR(D);
					PC++;
					break;

				case 0x1B:		//RR E
					RR(E);
					PC++;
					break;

				case 0x1C:		//RR H
					RR(H);
					PC++;
					break;

				case 0x1D:		//RR L
					RR(L);
					PC++;
					break;

				case 0x1E:		//RR (HL)
					nFlag = 0x00;
					hFlag = 0x00;
					tempChar = cFlag;
					cFlag = mmu.readMemory(HL) & 0x01;
					resultChar = mmu.readMemory(HL) >> 1;
					resultChar |= tempChar << 7;
					resultChar == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					mmu.writeMemory(HL, resultChar);
					PC++;
					break;

				case 0x27:		//SLA A
					SLA(A);
					PC++;
					break;

				case 0x20:		//SLA B
					SLA(B);
					PC++;
					break;

				case 0x21:		//SLA C
					SLA(C);
					PC++;
					break;

				case 0x22:		//SLA D
					SLA(D);
					PC++;
					break;

				case 0x23:		//SLA E
					SLA(E);
					PC++;
					break;

				case 0x24:		//SLA H
					SLA(H);
					PC++;
					break;

				case 0x25:		//SLA L
					SLA(L);
					PC++;
					break;

				case 0x26:		//SLA (HL)
					nFlag = 0x00;
					hFlag = 0x00;
					cFlag = mmu.readMemory(HL) & 0x80;
					cFlag >>= 7;
					mmu.writeMemory(HL, mmu.readMemory(HL) << 1);
					mmu.writeMemory(HL, mmu.readMemory(HL) & 0xFE);
					mmu.readMemory(HL) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x2F:		//SRA A
					SRA(A);
					PC++;
					break;

				case 0x28:		//SRA B
					SRA(B);
					PC++;
					break;

				case 0x29:		//SRA C
					SRA(C);
					PC++;
					break;

				case 0x2A:		//SRA D
					SRA(D);
					PC++;
					break;

				case 0x2B:		//SRA E
					SRA(E);
					PC++;
					break;

				case 0x2C:		//SRA H
					SRA(H);
					PC++;
					break;

				case 0x2D:		//SRA L
					SRA(L);
					PC++;
					break;

				case 0x2E:		//SRA (HL)
					nFlag = 0x00;
					hFlag = 0x00;

					((mmu.readMemory(HL) & 0x01) != 0x00) ? tempChar = 0x01 : tempChar = 0x00;
					((mmu.readMemory(HL) & 0x80) != 0x00) ? resultChar = 0x01 : resultChar = 0x00;

					result = (unsigned char)(mmu.readMemory(HL) >> 1);
					result = bitwise::set_bit_to(result, 7, resultChar);				//TODO: examine this
					//resultChar == 0x01 ? result &= 0xFF : result &= 0x7F;

					result == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					cFlag = tempChar;

					mmu.writeMemory(HL, result);
					PC++;
					break;

				case 0x3F:		//SRL A
					SRL(A);
					PC++;
					break;

				case 0x38:		//SRL B
					SRL(B);
					PC++;
					break;

				case 0x39:		//SRL C
					SRL(C);
					PC++;
					break;

				case 0x3A:		//SRL D
					SRL(D);
					PC++;
					break;

				case 0x3B:		//SRL E
					SRL(E);
					PC++;
					break;

				case 0x3C:		//SRL H
					SRL(H);
					PC++;
					break;

				case 0x3D:		//SRL L
					SRL(L);
					PC++;
					break;

				case 0x3E:		//SRL (HL)
					nFlag = 0x00;
					hFlag = 0x00;
					((mmu.readMemory(HL) & 0x01) != 0x00) ? tempChar = 0x01 : tempChar = 0x00;
					resultChar = (mmu.readMemory(HL) >> 1);
					cFlag = tempChar;
					resultChar == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					mmu.writeMemory(HL, resultChar);
					PC++;
					break;

				case 0x47:		//BIT 0, A
					nFlag = 0x00;
					hFlag = 0x01;
					(A & 0x01) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x4F:		//BIT 1, A
					nFlag = 0x00;
					hFlag = 0x01;
					(A & 0x02) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x57:		//BIT 2, A
					nFlag = 0x00;
					hFlag = 0x01;
					(A & 0x04) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x5F:		//BIT 3, A
					nFlag = 0x00;
					hFlag = 0x01;
					(A & 0x08) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x67:		//BIT 4, A
					nFlag = 0x00;
					hFlag = 0x01;
					(A & 0x10) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x6F:		//BIT 5, A
					nFlag = 0x00;
					hFlag = 0x01;
					(A & 0x20) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x77:		//BIT 6, A
					nFlag = 0x00;
					hFlag = 0x01;
					(A & 0x40) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x7F:		//BIT 7, A
					nFlag = 0x00;
					hFlag = 0x01;
					(A & 0x80) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x40:		//BIT 0, B
					nFlag = 0x00;
					hFlag = 0x01;
					(B & 0x01) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x48:		//BIT 1, B
					nFlag = 0x00;
					hFlag = 0x01;
					(B & 0x02) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x50:		//BIT 2, B
					nFlag = 0x00;
					hFlag = 0x01;
					(B & 0x04) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x58:		//BIT 3, B
					nFlag = 0x00;
					hFlag = 0x01;
					(B & 0x08) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x60:		//BIT 4, B
					nFlag = 0x00;
					hFlag = 0x01;
					(B & 0x10) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x68:		//BIT 5, B
					nFlag = 0x00;
					hFlag = 0x01;
					(B & 0x20) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x70:		//BIT 6, B
					nFlag = 0x00;
					hFlag = 0x01;
					(B & 0x40) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x78:		//BIT 7, B
					nFlag = 0x00;
					hFlag = 0x01;
					(B & 0x80) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x41:		//BIT 0, C
					nFlag = 0x00;
					hFlag = 0x01;
					(C & 0x01) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x49:		//BIT 1, C
					nFlag = 0x00;
					hFlag = 0x01;
					(C & 0x02) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x51:		//BIT 2, C
					nFlag = 0x00;
					hFlag = 0x01;
					(C & 0x04) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x59:		//BIT 3, C
					nFlag = 0x00;
					hFlag = 0x01;
					(C & 0x08) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x61:		//BIT 4, C
					nFlag = 0x00;
					hFlag = 0x01;
					(C & 0x10) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x69:		//BIT 5, C
					nFlag = 0x00;
					hFlag = 0x01;
					(C & 0x20) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x71:		//BIT 6, C
					nFlag = 0x00;
					hFlag = 0x01;
					(C & 0x40) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x79:		//BIT 7, C
					nFlag = 0x00;
					hFlag = 0x01;
					(C & 0x80) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x42:		//BIT 0, D
					nFlag = 0x00;
					hFlag = 0x01;
					(D & 0x01) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x4A:		//BIT 1, D
					nFlag = 0x00;
					hFlag = 0x01;
					(D & 0x02) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x52:		//BIT 2, D
					nFlag = 0x00;
					hFlag = 0x01;
					(D & 0x04) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x5A:		//BIT 3, D
					nFlag = 0x00;
					hFlag = 0x01;
					(D & 0x08) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x62:		//BIT 4, D
					nFlag = 0x00;
					hFlag = 0x01;
					(D & 0x10) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x6A:		//BIT 5, D
					nFlag = 0x00;
					hFlag = 0x01;
					(D & 0x20) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x72:		//BIT 6, D
					nFlag = 0x00;
					hFlag = 0x01;
					(D & 0x40) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x7A:		//BIT 7, D
					nFlag = 0x00;
					hFlag = 0x01;
					(D & 0x80) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x43:		//BIT 0, E
					nFlag = 0x00;
					hFlag = 0x01;
					(E & 0x01) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x4B:		//BIT 1, E
					nFlag = 0x00;
					hFlag = 0x01;
					(E & 0x02) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x53:		//BIT 2, E
					nFlag = 0x00;
					hFlag = 0x01;
					(E & 0x04) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x5B:		//BIT 3, E
					nFlag = 0x00;
					hFlag = 0x01;
					(E & 0x08) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x63:		//BIT 4, E
					nFlag = 0x00;
					hFlag = 0x01;
					(E & 0x10) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x6B:		//BIT 5, E
					nFlag = 0x00;
					hFlag = 0x01;
					(E & 0x20) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x73:		//BIT 6, E
					nFlag = 0x00;
					hFlag = 0x01;
					(E & 0x40) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x7B:		//BIT 7, E
					nFlag = 0x00;
					hFlag = 0x01;
					(E & 0x80) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x44:		//BIT 0, H
					nFlag = 0x00;
					hFlag = 0x01;
					(H & 0x01) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x4C:		//BIT 1, H
					nFlag = 0x00;
					hFlag = 0x01;
					(H & 0x02) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x54:		//BIT 2, H
					nFlag = 0x00;
					hFlag = 0x01;
					(H & 0x04) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x5C:		//BIT 3, H
					nFlag = 0x00;
					hFlag = 0x01;
					(H & 0x08) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x64:		//BIT 4, H
					nFlag = 0x00;
					hFlag = 0x01;
					(H & 0x10) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x6C:		//BIT 5, H
					nFlag = 0x00;
					hFlag = 0x01;
					(H & 0x20) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x74:		//BIT 6, H
					nFlag = 0x00;
					hFlag = 0x01;
					(H & 0x40) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x7C:		//BIT 7, H
					nFlag = 0x00;
					hFlag = 0x01;
					(H & 0x80) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x45:		//BIT 0, L
					nFlag = 0x00;
					hFlag = 0x01;
					(L & 0x01) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x4D:		//BIT 1, L
					nFlag = 0x00;
					hFlag = 0x01;
					(L & 0x02) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x55:		//BIT 2, L
					nFlag = 0x00;
					hFlag = 0x01;
					(L & 0x04) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x5D:		//BIT 3, L
					nFlag = 0x00;
					hFlag = 0x01;
					(L & 0x08) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x65:		//BIT 4, L
					nFlag = 0x00;
					hFlag = 0x01;
					(L & 0x10) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x6D:		//BIT 5, L
					nFlag = 0x00;
					hFlag = 0x01;
					(L & 0x20) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x75:		//BIT 6, L
					nFlag = 0x00;
					hFlag = 0x01;
					(L & 0x40) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x7D:		//BIT 7, L
					nFlag = 0x00;
					hFlag = 0x01;
					(L & 0x80) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x46:		//BIT 0, (HL)
					nFlag = 0x00;
					hFlag = 0x01;
					(mmu.readMemory(HL) & 0x01) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x4E:		//BIT 1, (HL)
					nFlag = 0x00;
					hFlag = 0x01;
					(mmu.readMemory(HL) & 0x02) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x56:		//BIT 2, (HL)
					nFlag = 0x00;
					hFlag = 0x01;
					(mmu.readMemory(HL) & 0x04) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x5E:		//BIT 3, (HL)
					nFlag = 0x00;
					hFlag = 0x01;
					(mmu.readMemory(HL) & 0x08) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x66:		//BIT 4, (HL)
					nFlag = 0x00;
					hFlag = 0x01;
					(mmu.readMemory(HL) & 0x10) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x6E:		//BIT 5, (HL)
					nFlag = 0x00;
					hFlag = 0x01;
					(mmu.readMemory(HL) & 0x20) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x76:		//BIT 6, (HL)
					nFlag = 0x00;
					hFlag = 0x01;
					(mmu.readMemory(HL) & 0x40) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0x7E:		//BIT 7, (HL)
					nFlag = 0x00;
					hFlag = 0x01;
					(mmu.readMemory(HL) & 0x80) == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
					PC++;
					break;

				case 0xC7:		//SET 0, A
					A |= 0x01;
					PC++;
					break;

				case 0xCF:		//SET 1, A
					A |= 0x02;
					PC++;
					break;

				case 0xD7:		//SET 2, A
					A |= 0x04;
					PC++;
					break;

				case 0xDF:		//SET 3, A
					A |= 0x08;
					PC++;
					break;

				case 0xE7:		//SET 4, A
					A |= 0x10;
					PC++;
					break;

				case 0xEF:		//SET 5, A
					A |= 0x20;
					PC++;
					break;

				case 0xF7:		//SET 6, A
					A |= 0x40;
					PC++;
					break;

				case 0xFF:		//SET 7, A
					A |= 0x80;
					PC++;
					break;

				case 0xC0:		//SET 0, B
					B |= 0x01;
					PC++;
					break;

				case 0xC8:		//SET 1, B
					B |= 0x02;
					PC++;
					break;

				case 0xD0:		//SET 2, B
					B |= 0x04;
					PC++;
					break;

				case 0xD8:		//SET 3, B
					B |= 0x08;
					PC++;
					break;

				case 0xE0:		//SET 4, B
					B |= 0x10;
					PC++;
					break;

				case 0xE8:		//SET 5, B
					B |= 0x20;
					PC++;
					break;

				case 0xF0:		//SET 6, B
					B |= 0x40;
					PC++;
					break;

				case 0xF8:		//SET 7, B
					B |= 0x80;
					PC++;
					break;

				case 0xC1:		//SET 0, C
					C |= 0x01;
					PC++;
					break;

				case 0xC9:		//SET 1, C
					C |= 0x02;
					PC++;
					break;

				case 0xD1:		//SET 2, C
					C |= 0x04;
					PC++;
					break;

				case 0xD9:		//SET 3, C
					C |= 0x08;
					PC++;
					break;

				case 0xE1:		//SET 4, C
					C |= 0x10;
					PC++;
					break;

				case 0xE9:		//SET 5, C
					C |= 0x20;
					PC++;
					break;

				case 0xF1:		//SET 6, C
					C |= 0x40;
					PC++;
					break;

				case 0xF9:		//SET 7, C
					C |= 0x80;
					PC++;
					break;

				case 0xC2:		//SET 0, D
					D |= 0x01;
					PC++;
					break;

				case 0xCA:		//SET 1, D
					D |= 0x02;
					PC++;
					break;

				case 0xD2:		//SET 2, D
					D |= 0x04;
					PC++;
					break;

				case 0xDA:		//SET 3, D
					D |= 0x08;
					PC++;
					break;

				case 0xE2:		//SET 4, D
					D |= 0x10;
					PC++;
					break;

				case 0xEA:		//SET 5, D
					D |= 0x20;
					PC++;
					break;

				case 0xF2:		//SET 6, D
					D |= 0x40;
					PC++;
					break;

				case 0xFA:		//SET 7, D
					D |= 0x80;
					PC++;
					break;

				case 0xC3:		//SET 0, E
					E |= 0x01;
					PC++;
					break;

				case 0xCB:		//SET 1, E
					E |= 0x02;
					PC++;
					break;

				case 0xD3:		//SET 2, E
					E |= 0x04;
					PC++;
					break;

				case 0xDB:		//SET 3, E
					E |= 0x08;
					PC++;
					break;

				case 0xE3:		//SET 4, E
					E |= 0x10;
					PC++;
					break;

				case 0xEB:		//SET 5, E
					E |= 0x20;
					PC++;
					break;

				case 0xF3:		//SET 6, E
					E |= 0x40;
					PC++;
					break;

				case 0xFB:		//SET 7, E
					E |= 0x80;
					PC++;
					break;

				case 0xC4:		//SET 0, H
					H |= 0x01;
					PC++;
					break;

				case 0xCC:		//SET 1, H
					H |= 0x02;
					PC++;
					break;

				case 0xD4:		//SET 2, H
					H |= 0x04;
					PC++;
					break;

				case 0xDC:		//SET 3, H
					H |= 0x08;
					PC++;
					break;

				case 0xE4:		//SET 4, H
					H |= 0x10;
					PC++;
					break;

				case 0xEC:		//SET 5, H
					H |= 0x20;
					PC++;
					break;

				case 0xF4:		//SET 6, H
					H |= 0x40;
					PC++;
					break;

				case 0xFC:		//SET 7, H
					H |= 0x80;
					PC++;
					break;

				case 0xC5:		//SET 0, L
					L |= 0x01;
					PC++;
					break;

				case 0xCD:		//SET 1, L
					L |= 0x02;
					PC++;
					break;

				case 0xD5:		//SET 2, L
					L |= 0x04;
					PC++;
					break;

				case 0xDD:		//SET 3, L
					L |= 0x08;
					PC++;
					break;

				case 0xE5:		//SET 4, L
					L |= 0x10;
					PC++;
					break;

				case 0xED:		//SET 5, L
					L |= 0x20;
					PC++;
					break;

				case 0xF5:		//SET 6, L
					L |= 0x40;
					PC++;
					break;

				case 0xFD:		//SET 7, L
					L |= 0x80;
					PC++;
					break;

				case 0xC6:		//SET 0, (HL)
					mmu.writeMemory(HL, mmu.readMemory(HL) | 0x01);
					PC++;
					break;

				case 0xCE:		//SET 1, (HL)
					mmu.writeMemory(HL, mmu.readMemory(HL) | 0x02);
					PC++;
					break;

				case 0xD6:		//SET 2, (HL)
					mmu.writeMemory(HL, mmu.readMemory(HL) | 0x04);
					PC++;
					break;

				case 0xDE:		//SET 3, (HL)
					mmu.writeMemory(HL, mmu.readMemory(HL) | 0x08);
					PC++;
					break;

				case 0xE6:		//SET 4, (HL)
					mmu.writeMemory(HL, mmu.readMemory(HL) | 0x10);
					PC++;
					break;

				case 0xEE:		//SET 5, (HL)
					mmu.writeMemory(HL, mmu.readMemory(HL) | 0x20);
					PC++;
					break;

				case 0xF6:		//SET 6, (HL)
					mmu.writeMemory(HL, mmu.readMemory(HL) | 0x40);
					PC++;
					break;

				case 0xFE:		//SET 7, (HL)
					mmu.writeMemory(HL, mmu.readMemory(HL) | 0x80);
					PC++;
					break;

				case 0x87:		//RES 0, A
					A &= 0xFE;
					PC++;
					break;

				case 0x8F:		//RES 1, A
					A &= 0xFD;
					PC++;
					break;

				case 0x97:		//RES 2, A
					A &= 0xFB;
					PC++;
					break;

				case 0x9F:		//RES 3, A
					A &= 0xF7;
					PC++;
					break;

				case 0xA7:		//RES 4, A
					A &= 0xEF;
					PC++;
					break;

				case 0xAF:		//RES 5, A
					A &= 0xDF;
					PC++;
					break;

				case 0xB7:		//RES 6, A
					A &= 0xBF;
					PC++;
					break;

				case 0xBF:		//RES 7, A
					A &= 0x7F;
					PC++;
					break;

				case 0x80:		//RES 0, B
					B &= 0xFE;
					PC++;
					break;

				case 0x88:		//RES 1, B
					B &= 0xFD;
					PC++;
					break;

				case 0x90:		//RES 2, B
					B &= 0xFB;
					PC++;
					break;

				case 0x98:		//RES 3, B
					B &= 0xF7;
					PC++;
					break;

				case 0xA0:		//RES 4, B
					B &= 0xEF;
					PC++;
					break;

				case 0xA8:		//RES 5, B
					B &= 0xDF;
					PC++;
					break;

				case 0xB0:		//RES 6, B
					B &= 0xBF;
					PC++;
					break;

				case 0xB8:		//RES 7, B
					B &= 0x7F;
					PC++;
					break;

				case 0x81:		//RES 0, C
					C &= 0xFE;
					PC++;
					break;

				case 0x89:		//RES 1, C
					C &= 0xFD;
					PC++;
					break;

				case 0x91:		//RES 2, C
					C &= 0xFB;
					PC++;
					break;

				case 0x99:		//RES 3, C
					C &= 0xF7;
					PC++;
					break;

				case 0xA1:		//RES 4, C
					C &= 0xEF;
					PC++;
					break;

				case 0xA9:		//RES 5, C
					C &= 0xDF;
					PC++;
					break;

				case 0xB1:		//RES 6, C
					C &= 0xBF;
					PC++;
					break;

				case 0xB9:		//RES 7, C
					C &= 0x7F;
					PC++;
					break;

				case 0x82:		//RES 0, D
					D &= 0xFE;
					PC++;
					break;

				case 0x8A:		//RES 1, D
					D &= 0xFD;
					PC++;
					break;

				case 0x92:		//RES 2, D
					D &= 0xFB;
					PC++;
					break;

				case 0x9A:		//RES 3, D
					D &= 0xF7;
					PC++;
					break;

				case 0xA2:		//RES 4, D
					D &= 0xEF;
					PC++;
					break;

				case 0xAA:		//RES 5, D
					D &= 0xDF;
					PC++;
					break;

				case 0xB2:		//RES 6, D
					D &= 0xBF;
					PC++;
					break;

				case 0xBA:		//RES 7, D
					D &= 0x7F;
					PC++;
					break;

				case 0x83:		//RES 0, E
					E &= 0xFE;
					PC++;
					break;

				case 0x8B:		//RES 1, E
					E &= 0xFD;
					PC++;
					break;

				case 0x93:		//RES 2, E
					E &= 0xFB;
					PC++;
					break;

				case 0x9B:		//RES 3, E
					E &= 0xF7;
					PC++;
					break;

				case 0xA3:		//RES 4, E
					E &= 0xEF;
					PC++;
					break;

				case 0xAB:		//RES 5, E
					E &= 0xDF;
					PC++;
					break;

				case 0xB3:		//RES 6, E
					E &= 0xBF;
					PC++;
					break;

				case 0xBB:		//RES 7, E
					E &= 0x7F;
					PC++;
					break;

				case 0x84:		//RES 0, H
					H &= 0xFE;
					PC++;
					break;

				case 0x8C:		//RES 1, H
					H &= 0xFD;
					PC++;
					break;

				case 0x94:		//RES 2, H
					H &= 0xFB;
					PC++;
					break;

				case 0x9C:		//RES 3, H
					H &= 0xF7;
					PC++;
					break;

				case 0xA4:		//RES 4, H
					H &= 0xEF;
					PC++;
					break;

				case 0xAC:		//RES 5, H
					H &= 0xDF;
					PC++;
					break;

				case 0xB4:		//RES 6, H
					H &= 0xBF;
					PC++;
					break;

				case 0xBC:		//RES 7, H
					H &= 0x7F;
					PC++;
					break;

				case 0x85:		//RES 0, L
					L &= 0xFE;
					PC++;
					break;

				case 0x8D:		//RES 1, L
					L &= 0xFD;
					PC++;
					break;

				case 0x95:		//RES 2, L
					L &= 0xFB;
					PC++;
					break;

				case 0x9D:		//RES 3, L
					L &= 0xF7;
					PC++;
					break;

				case 0xA5:		//RES 4, L
					L &= 0xEF;
					PC++;
					break;

				case 0xAD:		//RES 5, L
					L &= 0xDF;
					PC++;
					break;

				case 0xB5:		//RES 6, L
					L &= 0xBF;
					PC++;
					break;

				case 0xBD:		//RES 7, L
					L &= 0x7F;
					PC++;
					break;

				case 0x86:		//RES 0, (HL)
					mmu.writeMemory(HL, mmu.readMemory(HL) & 0xFE);
					PC++;
					break;

				case 0x8E:		//RES 1, (HL)
					mmu.writeMemory(HL, mmu.readMemory(HL) & 0xFD);
					PC++;
					break;

				case 0x96:		//RES 2, (HL)
					mmu.writeMemory(HL, mmu.readMemory(HL) & 0xFB);
					PC++;
					break;

				case 0x9E:		//RES 3, (HL)
					mmu.writeMemory(HL, mmu.readMemory(HL) & 0xF7);
					PC++;
					break;

				case 0xA6:		//RES 4, (HL)
					mmu.writeMemory(HL, mmu.readMemory(HL) & 0xEF);
					PC++;
					break;

				case 0xAE:		//RES 5, (HL)
					mmu.writeMemory(HL, mmu.readMemory(HL) & 0xDF);
					PC++;
					break;

				case 0xB6:		//RES 6, (HL)
					mmu.writeMemory(HL, mmu.readMemory(HL) & 0xBF);
					PC++;
					break;

				case 0xBE:		//RES 7, (HL)
					mmu.writeMemory(HL, mmu.readMemory(HL) & 0x7F);
					PC++;
					break;
			}

			PC++;
			break;

		case 0x27:
			tempChar = A;
			cFlag == 0x01 ? result = 0x60 : result = 0x00;

			if (hFlag == 0x01 || (nFlag == 0x00 && ((tempChar & 0x0F) > 9)))
			{
				result |= 0x06;
			}

			if (cFlag == 0x01 || (nFlag == 0x00 && (tempChar > 0x99)))
			{
				result |= 0x60;
			}

			if (nFlag == 0x01)
			{
				tempChar = (unsigned char)(tempChar - result);
			}

			else
			{
				tempChar = (unsigned char)(tempChar + result);
			}

			if (((result << 2) & 0x100) != 0)
			{
				cFlag = 0x01;
			}

			hFlag = 0x00;
			tempChar == 0x00 ? zFlag = 0x01 : zFlag = 0x00;
			A = (unsigned char)tempChar;
			PC++;
			break;

		case 0x2F:
			nFlag = 0x01;
			hFlag = 0x01;
			A = ~A;
			PC++;
			break;

		case 0x3F:
			nFlag = 0x00;
			hFlag = 0x00;
			cFlag == 0x00 ? cFlag = 0x01 : cFlag = 0x00;
			PC++;
			break;

		case 0x37:
			nFlag = 0x00;
			hFlag = 0x00;
			cFlag = 0x01;
			PC++;
			break;

		case 0x76:		//HALT
			halted = true;
			PC++;
			break;

		case 0x10:		//STOP
						//TODO: Stop
			PC += 2;
			break;

		case 0xF3:		//DI
			IME = false;
			PC++;
			break;

		case 0xFB:		//EI
			IME = true;		//TODO: Proper time for this should be at the end of the tick maybe?
			PC++;
			break;

		case 0x07:		//RLCA
			RLC(A);
			zFlag = 0x00;
			PC++;
			break;

		case 0x17:		//RLA
			RL(A);
			zFlag = 0x00;
			PC++;
			break;

		case 0x0F:		//RRCA
			RRC(A);
			zFlag = 0x00;
			PC++;
			break;

		case 0x1F:		//RRA
			RR(A);
			zFlag = 0x00;
			PC++;
			break;

		case 0xC3:		//JP nn
			PC = immediate_16;
			break;

		case 0xC2:		//JP NZ, nn
			if (zFlag == 0x00)
			{
				PC = immediate_16;
				useCondOpcodeCycles = true;
			}

			else
			{
				PC += 3;
			}
			break;

		case 0xCA:		//JP Z, nn
						//zFlag == 0x01 ? PC = immediate_16 : PC += 3;
			if (zFlag == 0x01)
			{
				PC = immediate_16;
				useCondOpcodeCycles = true;
			}

			else
			{
				PC += 3;
			}
			break;

		case 0xD2:		//JP NC, nn
						//cFlag == 0x00 ? PC = immediate_16 : PC += 3;
			if (cFlag == 0x00)
			{
				PC = immediate_16;
				useCondOpcodeCycles = true;
			}

			else
			{
				PC += 3;
			}
			break;

		case 0xDA:		//JP C, nn
						//cFlag == 0x01 ? PC = immediate_16 : PC += 3;
			if (cFlag == 0x01)
			{
				PC = immediate_16;
				useCondOpcodeCycles = true;
			}

			else
			{
				PC += 3;
			}
			break;

		case 0xE9:		//JP (HL)
			PC = HL;
			break;

		case 0x18:		//JR n
			PC += signed_immediate_8;
			PC += 2;
			break;

		case 0x20:		//JR NZ, n //zFlag == 0x00 ? PC += signed_immediate_8 : PC += 2;
			if (zFlag == 0x00)
			{
				PC += signed_immediate_8;
				useCondOpcodeCycles = true;
			}

			PC += 2;
			break;

		case 0x28:		//JR Z, n
			if (zFlag == 0x01)
			{
				PC += signed_immediate_8;
				useCondOpcodeCycles = true;
			}

			PC += 2;
			break;

		case 0x30:		//JR NC, n
			if (cFlag == 0x00)
			{
				PC += signed_immediate_8;
				useCondOpcodeCycles = true;
			}

			PC += 2;
			break;

		case 0x38:		//JR C, n
			if (cFlag == 0x01)
			{
				PC += signed_immediate_8;
				useCondOpcodeCycles = true;
			}

			PC += 2;
			break;

		case 0xCD:		//CALL nn
			PC += 3;
			upperPC = PC >> 8;
			lowerPC = PC;
			mmu.writeMemory(SP - 0x01, upperPC);
			mmu.writeMemory(SP - 0x02, lowerPC);
			PC = immediate_16;
			SP -= 2;
			break;

		case 0xC4:		//CALL NZ, nn
			if (zFlag == 0x00)
			{
				//mmu.writeMemory(SP - 0x01, mmu.readMemory(PC + 0x02));
				//mmu.writeMemory(SP - 0x02, mmu.readMemory(PC + 0x01));
				mmu.writeMemory(SP - 0x01, upperPC);
				mmu.writeMemory(SP - 0x02, lowerPC + 3);
				PC = immediate_16;
				SP -= 2;
				useCondOpcodeCycles = true;
			}

			else
			{
				PC += 3;
			}
			break;

		case 0xCC:		//CALL Z, nn
			if (zFlag == 0x01)
			{
				//mmu.writeMemory(SP - 0x01, mmu.readMemory(PC + 0x02));
				//mmu.writeMemory(SP - 0x02, mmu.readMemory(PC + 0x01));
				mmu.writeMemory(SP - 0x01, upperPC);
				mmu.writeMemory(SP - 0x02, lowerPC + 3);
				PC = immediate_16;
				SP -= 2;
				useCondOpcodeCycles = true;
			}

			else
			{
				PC += 3;
			}
			break;

		case 0xD4:		//CALL NC, nn
			if (cFlag == 0x00)
			{
				//mmu.writeMemory(SP - 0x01, mmu.readMemory(PC + 0x02));
				//mmu.writeMemory(SP - 0x02, mmu.readMemory(PC + 0x01));
				mmu.writeMemory(SP - 0x01, upperPC);
				mmu.writeMemory(SP - 0x02, lowerPC + 3);
				PC = immediate_16;
				SP -= 2;
				useCondOpcodeCycles = true;
			}

			else
			{
				PC += 3;
			}
			break;

		case 0xDC:		//CALL C, nn
			if (cFlag == 0x01)
			{
				//mmu.writeMemory(SP - 0x01, mmu.readMemory(PC + 0x02));
				//mmu.writeMemory(SP - 0x02, mmu.readMemory(PC + 0x01));
				mmu.writeMemory(SP - 0x01, upperPC);
				mmu.writeMemory(SP - 0x02, lowerPC + 3);
				PC = immediate_16;
				SP -= 2;
				useCondOpcodeCycles = true;
			}

			else
			{
				PC += 3;
			}
			break;

		case 0xC7:		//RST 00
			mmu.writeMemory(SP - 0x01, upperPC);
			mmu.writeMemory(SP - 0x02, lowerPC + 1);
			SP -= 2;
			PC = 0x0000;
			break;

		case 0xCF:		//RST 08
			mmu.writeMemory(SP - 0x01, upperPC);
			mmu.writeMemory(SP - 0x02, lowerPC + 1);
			SP -= 2;
			PC = 0x0008;
			break;

		case 0xD7:		//RST 10
			mmu.writeMemory(SP - 0x01, upperPC);
			mmu.writeMemory(SP - 0x02, lowerPC + 1);
			SP -= 2;
			PC = 0x0010;
			break;

		case 0xDF:		//RST 18
			mmu.writeMemory(SP - 0x01, upperPC);
			mmu.writeMemory(SP - 0x02, lowerPC + 1);
			SP -= 2;
			PC = 0x0018;
			break;

		case 0xE7:		//RST 20
			mmu.writeMemory(SP - 0x01, upperPC);
			mmu.writeMemory(SP - 0x02, lowerPC + 1);
			SP -= 2;
			PC = 0x0020;
			break;

		case 0xEF:		//RST 28
			mmu.writeMemory(SP - 0x01, upperPC);
			mmu.writeMemory(SP - 0x02, lowerPC + 1);
			SP -= 2;
			PC = 0x0028;
			break;

		case 0xF7:		//RST 30
			mmu.writeMemory(SP - 0x01, upperPC);
			mmu.writeMemory(SP - 0x02, lowerPC + 1);
			SP -= 2;
			PC = 0x0030;
			break;

		case 0xFF:		//RST 38
			mmu.writeMemory(SP - 0x01, upperPC);
			mmu.writeMemory(SP - 0x02, lowerPC + 1);
			SP -= 2;
			PC = 0x0038;
			break;

		case 0xC9:		//RET
			PC = ((mmu.readMemory(SP + 1) << 8) | mmu.readMemory(SP));
			SP += 2;
			break;

		case 0xC0:		//RET NZ
			if (zFlag == 0x00)
			{
				PC = (mmu.readMemory(SP + 1) << 8) | mmu.readMemory(SP);
				SP += 2;
				useCondOpcodeCycles = true;
			}

			else
			{
				PC++;
			}
			break;

		case 0xC8:		//RET Z
			if (zFlag == 0x01)
			{
				PC = (mmu.readMemory(SP + 1) << 8) | mmu.readMemory(SP);
				SP += 2;
				useCondOpcodeCycles = true;
			}

			else
			{
				PC++;
			}
			break;

		case 0xD0:		//RET NC
			if (cFlag == 0x00)
			{
				PC = (mmu.readMemory(SP + 1) << 8) | mmu.readMemory(SP);
				SP += 2;
				useCondOpcodeCycles = true;
			}

			else
			{
				PC++;
			}
			break;

		case 0xD8:		//RET C
			if (cFlag == 0x01)
			{
				PC = (mmu.readMemory(SP + 1) << 8) | mmu.readMemory(SP);
				SP += 2;
				useCondOpcodeCycles = true;
			}

			else
			{
				PC++;
			}
			break;

		case 0xD9:		//RETI
			PC = (mmu.readMemory(SP + 1) << 8) | mmu.readMemory(SP);
			SP += 2;
			IME = true;
			break;

		default:
			std::cout << "ERROR: unrecognized opcode: " << std::hex << opcode << std::endl;
			break;
	}

	if (!poppedAF)
	{
		zFlag == 0x01 ? F = bitwise::set_bit(F, 7) : F = bitwise::clear_bit(F, 7);
		nFlag == 0x01 ? F = bitwise::set_bit(F, 6) : F = bitwise::clear_bit(F, 6);
		hFlag == 0x01 ? F = bitwise::set_bit(F, 5) : F = bitwise::clear_bit(F, 5);
		cFlag == 0x01 ? F = bitwise::set_bit(F, 4) : F = bitwise::clear_bit(F, 4);
	}

	else
	{
		bitwise::check_bit(F, 7) ? zFlag = 0x01 : zFlag = 0x00;
		bitwise::check_bit(F, 6) ? nFlag = 0x01 : nFlag = 0x00;
		bitwise::check_bit(F, 5) ? hFlag = 0x01 : hFlag = 0x00;
		bitwise::check_bit(F, 4) ? cFlag = 0x01 : cFlag = 0x00;
		poppedAF = false;
	}

	//zFlag == 0x01 ? F |= 0x80 : F &= 0x70;
	//nFlag == 0x01 ? F |= 0x40 : F &= 0xB0;
	//hFlag == 0x01 ? F |= 0x20 : F &= 0xD0;
	//cFlag == 0x01 ? F |= 0x10 : F &= 0xE0;

	if (opcode == 0x27)
	{
		//A = (AF & 0xFF00) >> 8;
		//F = AF & 0x00F0;
	}

	if (opcode == 0x01 || opcode == 0x03 || opcode == 0x0B)
	{
		B = (BC & 0xFF00) >> 8;
		C = BC & 0x00FF;
	}

	if (opcode == 0x11 || opcode == 0x13 || opcode == 0x1B)
	{
		D = (DE & 0xFF00) >> 8;
		E = DE & 0x00FF;
	}

	if (opcode == 0x09 || opcode == 0x19 || opcode == 0x21 || opcode == 0x22 || opcode == 0x23 || 
		opcode == 0x29 || opcode == 0x2A || opcode == 0x2B || opcode == 0x32 || opcode == 0x39 || 
		opcode == 0x3A || opcode == 0xC6 || opcode == 0xCE || opcode == 0xD6 || opcode == 0xF8)
	{
		H = (HL & 0xFF00) >> 8;
		L = HL & 0x00FF;
	}

	if (opcode == 0xCB)
	{
		//cyclesElapsed += opcodeCyclesCB[CB_opcode];
		cyclesElapsed += opcodeCyclesCB[CB_opcode];
	}

	else
	{
		if (useCondOpcodeCycles)
		{
			//cyclesElapsed += opcodeCyclesCond[opcode];
			cyclesElapsed += opcodeCyclesCond[opcode];
			//cyclesElapsed = opcodeCycles[opcode] / speedModifier;
			useCondOpcodeCycles = false;

		}

		else
		{
			//cyclesElapsed += opcodeCycles[opcode];
			cyclesElapsed += opcodeCycles[opcode];

		}
	}

	if (haltBug)
	{
		PC = prevPC;
		haltBug = false;
	}	
	
	clocksElapsed = cyclesElapsed;
	//DIV += clocksElapsed;
	divClocksElapsed += cyclesElapsed;
	//timerClocksElapsed += clocksElapsed;
	refreshClocksElapsed += clocksElapsed * 4;

	DIV += clocksElapsed * 4;
	mmu.memory[0xFF04] = DIV >> 8;

	return clocksElapsed;
}