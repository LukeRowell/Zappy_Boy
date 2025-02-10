#include "Debugger.h"
#include "Gameboy.h"

Debugger::Debugger()
{

}

Debugger::~Debugger()
{

}

void Debugger::setPreviousValues(GameBoy& zappyBoy)
{
	prev_A = zappyBoy.cpu.A;
	prev_F = zappyBoy.cpu.F;
	prev_B = zappyBoy.cpu.B;
	prev_C = zappyBoy.cpu.C;
	prev_D = zappyBoy.cpu.D;
	prev_E = zappyBoy.cpu.E;
	prev_H = zappyBoy.cpu.H;
	prev_L = zappyBoy.cpu.L;
	prev_AF = zappyBoy.cpu.AF;
	prev_BC = zappyBoy.cpu.BC;
	prev_DE = zappyBoy.cpu.DE;
	prev_HL = zappyBoy.cpu.HL;
	prev_SP = zappyBoy.cpu.SP;
	prev_PC = zappyBoy.cpu.PC;
	prev_memoryAF = zappyBoy.mmu.memory[zappyBoy.cpu.AF];
	prev_memoryBC = zappyBoy.mmu.memory[zappyBoy.cpu.BC];
	prev_memoryDE = zappyBoy.mmu.memory[zappyBoy.cpu.DE];
	prev_memoryHL = zappyBoy.mmu.memory[zappyBoy.cpu.HL];
	prev_memorySP = zappyBoy.mmu.memory[zappyBoy.cpu.SP];
	prev_memoryNN = zappyBoy.mmu.memory[zappyBoy.cpu.immediate_16];
	prev_memoryFFXX = zappyBoy.mmu.memory[0xFF00 + zappyBoy.cpu.immediate_8];
	prev_memoryFFCC = zappyBoy.mmu.memory[0xFF00 + zappyBoy.cpu.C];
	prev_zFlag = zappyBoy.cpu.zFlag;
	prev_nFlag = zappyBoy.cpu.nFlag;
	prev_hFlag = zappyBoy.cpu.hFlag;
	prev_cFlag = zappyBoy.cpu.cFlag;
}

void Debugger::printPC(unsigned short PC)
{
	std::cout << std::right << "[" << std::setfill('0') << std::setw(4) << std::hex << PC << "]: ";
}

void Debugger::printVar8(unsigned char var)
{
	std::cout << std::right << std::setfill('0') << std::setw(2) << std::hex << (unsigned)var;
}

void Debugger::printVar16(unsigned short var)
{
	std::cout << std::right << std::setfill('0') << std::setw(4) << std::hex << (unsigned)var;
}

void Debugger::printChangedVars(GameBoy& zappyBoy)
{
	if (prev_A != zappyBoy.cpu.A)
	{
		std::cout << "A: ";
		printVar8(prev_A);
		std::cout << " -> ";
		printVar8(zappyBoy.cpu.A);
		std::cout << "  ";
	}

	if (prev_F != zappyBoy.cpu.F)
	{
		std::cout << "F: ";
		printVar8(prev_F);
		std::cout << " -> ";
		printVar8(zappyBoy.cpu.F);
		std::cout << "  ";
	}

	if (prev_B != zappyBoy.cpu.B)
	{
		std::cout << "B: ";
		printVar8(prev_B);
		std::cout << " -> ";
		printVar8(zappyBoy.cpu.B);
		std::cout << "  ";
	}

	if (prev_C != zappyBoy.cpu.C)
	{
		std::cout << "C: ";
		printVar8(prev_C);
		std::cout << " -> ";
		printVar8(zappyBoy.cpu.C);
		std::cout << "  ";
	}

	if (prev_D != zappyBoy.cpu.D)
	{
		std::cout << "D: ";
		printVar8(prev_D);
		std::cout << " -> ";
		printVar8(zappyBoy.cpu.D);
		std::cout << "  ";
	}

	if (prev_E != zappyBoy.cpu.E)
	{
		std::cout << "E: ";
		printVar8(prev_E);
		std::cout << " -> ";
		printVar8(zappyBoy.cpu.E);
		std::cout << "  ";
	}

	if (prev_H != zappyBoy.cpu.H)
	{
		std::cout << "H: ";
		printVar8(prev_H);
		std::cout << " -> ";
		printVar8(zappyBoy.cpu.H);
		std::cout << "  ";
	}

	if (prev_L != zappyBoy.cpu.L)
	{
		std::cout << "L: ";
		printVar8(prev_L);
		std::cout << " -> ";
		printVar8(zappyBoy.cpu.L);
		std::cout << "  ";
	}

	if (prev_AF != zappyBoy.cpu.AF)
	{
		std::cout << "AF: ";
		printVar16(prev_AF);
		std::cout << " -> ";
		printVar16(zappyBoy.cpu.AF);
		std::cout << "  ";
	}

	if (prev_BC != zappyBoy.cpu.BC)
	{
		std::cout << "BC: ";
		printVar16(prev_BC);
		std::cout << " -> ";
		printVar16(zappyBoy.cpu.BC);
		std::cout << "  ";
	}

	if (prev_DE != zappyBoy.cpu.DE)
	{
		std::cout << "DE: ";
		printVar16(prev_DE);
		std::cout << " -> ";
		printVar16(zappyBoy.cpu.DE);
		std::cout << "  ";
	}

	if (prev_HL != zappyBoy.cpu.HL)
	{
		std::cout << "HL: ";
		printVar16(prev_HL);
		std::cout << " -> ";
		printVar16(zappyBoy.cpu.HL);
		std::cout << "  ";
	}

	if (prev_SP != zappyBoy.cpu.SP)
	{
		std::cout << "SP: ";
		printVar16(prev_SP);
		std::cout << " -> ";
		printVar16(zappyBoy.cpu.SP);
		std::cout << "  ";
	}

	if (prev_memoryAF != zappyBoy.mmu.memory[zappyBoy.cpu.AF])
	{
		std::cout << "(";
		printVar16(zappyBoy.cpu.AF);
		std::cout << "): ";
		printVar8(prev_memoryAF);
		std::cout << " -> ";
		printVar8(zappyBoy.mmu.memory[zappyBoy.cpu.AF]);
		std::cout << "  ";
	}

	if (prev_memoryBC != zappyBoy.mmu.memory[zappyBoy.cpu.BC])
	{
		std::cout << "(";
		printVar16(zappyBoy.cpu.BC);
		std::cout << "): ";
		printVar8(prev_memoryBC);
		std::cout << " -> ";
		printVar8(zappyBoy.mmu.memory[zappyBoy.cpu.BC]);
		std::cout << "  ";
	}

	if (prev_memoryDE != zappyBoy.mmu.memory[zappyBoy.cpu.DE])
	{
		std::cout << "(";
		printVar16(zappyBoy.cpu.DE);
		std::cout << "): ";
		printVar8(prev_memoryDE);
		std::cout << " -> ";
		printVar8(zappyBoy.mmu.memory[zappyBoy.cpu.DE]);
		std::cout << "  ";
	}

	if (prev_memoryHL != zappyBoy.mmu.memory[zappyBoy.cpu.HL])
	{
		std::cout << "(";
		printVar16(zappyBoy.cpu.HL);
		std::cout << "): ";
		printVar8(prev_memoryHL);
		std::cout << " -> ";
		printVar8(zappyBoy.mmu.memory[zappyBoy.cpu.HL]);
		std::cout << "  ";
	}

	if (prev_memorySP != zappyBoy.mmu.memory[zappyBoy.cpu.SP])
	{
		std::cout << "(";
		printVar16(zappyBoy.cpu.SP);
		std::cout << "): ";
		printVar8(prev_memorySP);
		std::cout << " -> ";
		printVar8(zappyBoy.mmu.memory[zappyBoy.cpu.SP]);
		std::cout << "  ";
	}

	if (prev_memoryNN != zappyBoy.mmu.memory[zappyBoy.cpu.immediate_16])
	{
		std::cout << "(";
		printVar16(zappyBoy.cpu.immediate_16);
		std::cout << "): ";
		printVar8(prev_memoryNN);
		std::cout << " -> ";
		printVar8(zappyBoy.mmu.memory[zappyBoy.cpu.immediate_16]);
		std::cout << "  ";
	}

	if (prev_memoryFFXX != zappyBoy.mmu.memory[0xFF00 + zappyBoy.cpu.immediate_8])
	{
		std::cout << "(";
		printVar16(0xFF00 + zappyBoy.cpu.immediate_8);
		std::cout << "): ";
		printVar8(zappyBoy.mmu.memory[prev_memoryFFXX]);
		std::cout << " -> ";
		printVar8(zappyBoy.mmu.memory[0xFF00 + zappyBoy.cpu.immediate_8]);
		std::cout << "  ";
	}

	if (prev_memoryFFCC != zappyBoy.mmu.memory[0xFF00 + zappyBoy.cpu.C])
	{
		std::cout << "(";
		printVar16(0xFF00 + zappyBoy.cpu.C);
		std::cout << "): ";
		printVar8(prev_memoryFFCC);
		std::cout << " -> ";
		printVar8(zappyBoy.mmu.memory[0xFF00 + zappyBoy.cpu.C]);
		std::cout << "  ";
	}

	if (prev_zFlag != zappyBoy.cpu.zFlag)
	{
		std::cout << "zFlag: ";
		printVar8(prev_zFlag);
		std::cout << " -> ";
		printVar8(zappyBoy.cpu.zFlag);
		std::cout << "  ";
	}

	if (prev_nFlag != zappyBoy.cpu.nFlag)
	{
		std::cout << "nFlag: ";
		printVar8(prev_nFlag);
		std::cout << " -> ";
		printVar8(zappyBoy.cpu.nFlag);
		std::cout << "  ";
	}

	if (prev_hFlag != zappyBoy.cpu.hFlag)
	{
		std::cout << "hFlag: ";
		printVar8(prev_hFlag);
		std::cout << " -> ";
		printVar8(zappyBoy.cpu.hFlag);
		std::cout << "  ";
	}

	if (prev_cFlag != zappyBoy.cpu.cFlag)
	{
		std::cout << "cFlag: ";
		printVar8(prev_cFlag);
		std::cout << " -> ";
		printVar8(zappyBoy.cpu.cFlag);
		std::cout << "  ";
	}

	std::cout << std::endl;
}

void Debugger::printMessage(CPU& ZBCPU)
{
	//std::string tabs = "\t\t\t ; ";

	//setPreviousValues(zappyBoy);
	printPC(ZBCPU.PC);

	switch (ZBCPU.opcode)
	{
		case 0x00:		//NOP
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "NOP";
			break;

		case 0x06:		//LD B, n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD B, n";
			break;

		case 0x0E:		//LD C, n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD C, n";
			break;

		case 0x16:		//LD D, n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD D, n";
			break;

		case 0x1E:		//LD E, n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD E, n";
			break;

		case 0x26:		//LD H, n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD H, n";
			break;

		case 0x2E:		//LD L, n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD L, n";
			break;

		case 0x7F:		//LD A, A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD A, A";
			break;

		case 0x78:		//LD A, B
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD A, B";
			break;

		case 0x79:		//LD A, C
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD A, C";
			break;

		case 0x7A:		//LD A, D
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD A, D";
			break;

		case 0x7B:		//LD A, E
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD A, E";
			break;

		case 0x7C:		//LD A, H
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD A, H";
			break;

		case 0x7D:		//LD A, L
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD A, L";
			break;

		case 0x40:		//LD B, B
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD B, B";
			break;

		case 0x41:		//LD B, C
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD B, C";
			break;

		case 0x42:		//LD B, D
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD B, D";
			break;

		case 0x43:		//LD B, E
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD B, E";
			break;

		case 0x44:		//LD B, H
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD B, H";
			break;

		case 0x45:		//LD B, L
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD B, L";
			break;

		case 0x46:		//LD B, (HL)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD B, (HL)";
			break;

		case 0x48:		//LD C, B
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD C, B";
			break;

		case 0x49:		//LD C, C
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD C, C";
			break;

		case 0x4A:		//LD C, D
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD C, D";
			break;

		case 0x4B:		//LD C, E
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD C, E";
			break;

		case 0x4C:		//LD C, H
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD C, H";
			break;

		case 0x4D:		//LD C, L
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD C, L";
			break;

		case 0x4E:		//LD C, (HL)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD C, (HL)";
			break;

		case 0x50:		//LD D, B
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD D, B";
			break;

		case 0x51:		//LD D, C
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD D, C";
			break;

		case 0x52:		//LD D, D
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD D, D";
			break;

		case 0x53:		//LD D, E
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD D, E";
			break;

		case 0x54:		//LD D, H
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD D, H";
			break;

		case 0x55:		//LD D, L
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD D, L";
			break;

		case 0x56:		//LD D, (HL)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD D, (HL)";
			break;

		case 0x58:		//LD E, B
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD E, B";
			break;

		case 0x59:		//LD E, C
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD E, C";
			break;

		case 0x5A:		//LD E, D
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD E, D";
			break;

		case 0x5B:		//LD E, E
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD E, E";
			break;

		case 0x5C:		//LD E, H
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD E, H";
			break;

		case 0x5D:		//LD E, L
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD E, L";
			break;

		case 0x5E:		//LD E, (HL)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD E, (HL)";
			break;

		case 0x60:		//LD H, B
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD H, B";
			break;

		case 0x61:		//LD H, C
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD H, C";
			break;

		case 0x62:		//LD H, D
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD H, D";
			break;

		case 0x63:		//LD H, E
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD H, E";
			break;

		case 0x64:		//LD H, H
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD H, H";
			break;

		case 0x65:		//LD H, L
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD H, L";
			break;

		case 0x66:		//LD H, (HL)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD H, (HL)";
			break;

		case 0x68:		//LD L, B
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD L, B";
			break;

		case 0x69:		//LD L, C
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD L, C";
			break;

		case 0x6A:		//LD L, D
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD L, D";
			break;

		case 0x6B:		//LD L, E
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD L, E";
			break;

		case 0x6C:		//LD L, H
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD L, H";
			break;

		case 0x6D:		//LD L, L
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD L, L";
			break;

		case 0x6E:		//LD L, (HL)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD L, (HL)";
			break;

		case 0x70:		//LD (HL), B
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD (HL), B";
			break;

		case 0x71:		//LD (HL), C
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD (HL), C";
			break;

		case 0x72:		//LD (HL), D
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD (HL), D";
			break;

		case 0x73:		//LD (HL), E
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD (HL), E";
			break;

		case 0x74:		//LD (HL), H
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD (HL), H";
			break;

		case 0x75:		//LD (HL), L
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD (HL), L";
			break;

		case 0x36:		//LD (HL), n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD (HL), n";
			break;

		case 0x0A:		//LD A, (BC)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD A, (BC)";
			break;

		case 0x1A:		//LD A, (DE)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD A, (DE)";
			break;

		case 0x7E:		//LD A, (HL)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD A, (HL)";
			break;

		case 0xFA:		//LD A, (nn)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD A, (nn)";
			break;

		case 0x3E:		//LD A, n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD A, n";
			break;

		case 0x47:		//LD B, A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD B, A";
			break;

		case 0x4F:		//LD C, A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD C, A";
			break;

		case 0x57:		//LD D, A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD D, A";
			break;

		case 0x5F:		//LD E, A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD E, A";
			break;

		case 0x67:		//LD H, A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD H, A";
			break;

		case 0x6F:		//LD L, A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD L, A";
			break;

		case 0x02:		//LD (BC), A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD (BC), A";
			break;

		case 0x12:		//LD (DE), A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD (DE), A";
			break;

		case 0x77:		//LD (HL), A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD (HL), A";
			break;

		case 0xEA:		//LD (nn), A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD (nn), A";
			break;

		case 0xF2:		//LD A, (0xFF00 + C)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD A, (0xFF00 + C)";
			break;

		case 0xE2:		//LD (0xFF00 + C), A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD (0xFF00 + C), A";
			break;

		case 0x32:		//LD (HL-), A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD (HL-), A";
			break;

		case 0x2A:		//LD A, (HL+)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD A, (HL+)";
			break;

		case 0x22:		//LD (HL+), A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD (HL+), A";
			break;

		case 0xE0:		//LD (0xFF00 + n), A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD (0xFF00 + n), A";
			break;

		case 0xF0:		//LD A, (0xFF00 + n)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD A, (0xFF00 + n)";
			break;

		case 0x01:		//LD BC, nn
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD BC, nn";
			break;

		case 0x11:		//LD DE, nn
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD DE, nn";
			break;

		case 0x21:		//LD HL, nn
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD HL, nn";
			break;

		case 0x31:		//LD SP, nn
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD SP, nn";
			break;

		case 0xF9:		//LD SP, HL
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD SP, HL";
			break;

		case 0xF8:		//LD HL, SP + n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD HL, SP + n";
			break;

		case 0x08:		//LD (nn), SP
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "LD (nn), SP";
			break;

		case 0xF5:		//PUSH AF
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "PUSH AF";
			break;

		case 0xC5:		//PUSH BC
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "PUSH BC";
			break;

		case 0xD5:		//PUSH DE
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "PUSH DE";
			break;

		case 0xE5:		//PUSH HL
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "PUSH HL";
			break;

		case 0xF1:		//POP AF
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "POP AF";
			break;

		case 0xC1:		//POP BC
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "POP BC";
			break;

		case 0xD1:		//POP DE
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "POP DE";
			break;

		case 0xE1:		//POP HL
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "POP HL";
			break;

		case 0x87:		//ADD A, A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADD A, A";
			break;

		case 0x80:		//ADD A, B
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADD A, B";
			break;

		case 0x81:		//ADD A, C
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADD A, C";
			break;

		case 0x82:		//ADD A, D
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADD A, D";
			break;

		case 0x83:		//ADD A, E
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADD A, E";
			break;

		case 0x84:		//ADD A, H
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADD A, H";
			break;

		case 0x85:		//ADD A, L
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADD A, L";
			break;

		case 0x86:		//ADD A, (HL)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADD A, (HL)";
			break;

		case 0xC6:		//ADD A, n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADD A, n";
			break;

		case 0x8F:		//ADC A, A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADC A, A";
			break;

		case 0x88:		//ADC A, B
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADC A, B";
			break;

		case 0x89:		//ADC A, C
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADC A, C";
			break;

		case 0x8A:		//ADC A, D
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADC A, D";
			break;

		case 0x8B:		//ADC A, E
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADC A, E";
			break;

		case 0x8C:		//ADC A, H
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADC A, H";
			break;

		case 0x8D:		//ADC A, L
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADC A, L";
			break;

		case 0x8E:		//ADC A, (HL)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADC A, (HL)";
			break;

		case 0xCE:		//ADC A, n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADC A, n";
			break;

		case 0x97:		//SUB A, A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "SUB A, A";
			break;

		case 0x90:		//SUB A, B
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "SUB A, B";
			break;

		case 0x91:		//SUB A, C
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "SUB A, C";
			break;

		case 0x92:		//SUB A, D
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "SUB A, D";
			break;

		case 0x93:		//SUB A, E
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "SUB A, E";
			break;

		case 0x94:		//SUB A, H
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "SUB A, H";
			break;

		case 0x95:		//SUB A, L
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "SUB A, L";
			break;

		case 0x96:		//SUB A, (HL)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "SUB A, (HL)";
			break;

		case 0xD6:		//SUB A, n 
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "SUB A, n";
			break;

		case 0x9F:		//SBC A, A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "SBC A, A";
			break;

		case 0x98:		//SBC A, B
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "SBC A, B";
			break;

		case 0x99:		//SBC A, C
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "SBC A, C";
			break;

		case 0x9A:		//SBC A, D
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "SBC A, D";
			break;

		case 0x9B:		//SBC A, E
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "SBC A, E";
			break;

		case 0x9C:		//SBC A, H
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "SBC A, H";
			break;

		case 0x9D:		//SBC A, L
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "SBC A, L";
			break;

		case 0x9E:		//SBC A, (HL)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "SBC A, (HL)";
			break;

		case 0xA7:		//AND A, A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "AND A, A";
			break;

		case 0xA0:		//AND A, B
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "AND A, B";
			break;

		case 0xA1:		//AND A, C
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "AND A, C";
			break;

		case 0xA2:		//AND A, D
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "AND A, D";
			break;

		case 0xA3:		//AND A, E
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "AND A, E";
			break;

		case 0xA4:		//AND A, H
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "AND A, H";
			break;

		case 0xA5:		//AND A, L
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "AND A, L";
			break;

		case 0xA6:		//AND A, (HL)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "AND A, (HL)";
			break;

		case 0xE6:		//AND A, n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "AND A, n";
			break;

		case 0xB7:		//OR A, A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "OR A, A";
			break;

		case 0xB0:		//OR A, B
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "OR A, B";
			break;

		case 0xB1:		//OR A, C
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "OR A, C";
			break;

		case 0xB2:		//OR A, D
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "OR A, D";
			break;

		case 0xB3:		//OR A, E
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "OR A, E";
			break;

		case 0xB4:		//OR A, H
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "OR A, H";
			break;

		case 0xB5:		//OR A, L
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "OR A, L";
			break;

		case 0xB6:		//OR A, (HL)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "OR A, (HL)";
			break;

		case 0xF6:		//OR A, n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "OR A, n";
			break;

		case 0xAF:		//XOR A, A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "XOR A, A";
			break;

		case 0xA8:		//XOR A, B
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "XOR A, B";
			break;

		case 0xA9:		//XOR A, C
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "XOR A, C";
			break;

		case 0xAA:		//XOR A, D
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "XOR A, D";
			break;

		case 0xAB:		//XOR A, E
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "XOR A, E";
			break;

		case 0xAC:		//XOR A, H
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "XOR A, H";
			break;

		case 0xAD:		//XOR A, L
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "XOR A, L";
			break;

		case 0xAE:		//XOR A, (HL)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "XOR A, (HL)";
			break;

		case 0xEE:		//XOR A, n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "XOR A, n";
			break;

		case 0xBF:		//CP A, A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "CP A, A";
			break;

		case 0xB8:		//CP A, B
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "CP A, B";
			break;

		case 0xB9:		//CP A, C
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "CP A, C";
			break;

		case 0xBA:		//CP A, D
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "CP A, D";
			break;

		case 0xBB:		//CP A, E
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "CP A, E";
			break;

		case 0xBC:		//CP A, H
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "CP A, H";
			break;

		case 0xBD:		//CP A, L
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "CP A, L";
			break;

		case 0xBE:		//CP A, (HL)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "CP A, (HL)";
			break;

		case 0xFE:		//CP A, n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "CP A, n";
			break;

		case 0x3C:		//INC A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "INC A";
			break;

		case 0x04:		//INC B
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "INC B";
			break;

		case 0x0C:		//INC C
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "INC C";
			break;

		case 0x14:		//INC D
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "INC D";
			break;

		case 0x1C:		//INC E
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "INC E";
			break;

		case 0x24:		//INC H
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "INC H";
			break;

		case 0x2C:		//INC L
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "INC L";
			break;

		case 0x34:		//INC (HL)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "INC (HL)";
			break;

		case 0x3D:		//DEC A
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "DEC A";
			break;

		case 0x05:		//DEC B
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "DEC B";
			break;

		case 0x0D:		//DEC C
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "DEC C";
			break;

		case 0x15:		//DEC D
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "DEC D";
			break;

		case 0x1D:		//DEC E
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "DEC E";
			break;

		case 0x25:		//DEC H
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "DEC H";
			break;

		case 0x2D:		//DEC L
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "DEC L";
			break;

		case 0x35:		//DEC (HL)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "DEC (HL)";
			break;

		case 0x09:		//ADD HL, BC
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADD HL, BC";
			break;

		case 0x19:		//ADD HL, DE
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADD HL, DE";
			break;

		case 0x29:		//ADD HL, HL
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADD HL, HL";
			break;

		case 0x39:		//ADD HL, SP
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADD HL, SP";
			break;

		case 0xE8:		//ADD SP, n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "ADD SP, n";
			break;

		case 0x03:		//INC BC
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "INC BC";
			break;

		case 0x13:		//INC DE
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "INC DE";
			break;

		case 0x23:		//INC HL
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "INC HL";
			break;

		case 0x33:		//INC SP
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "INC SP";
			break;

		case 0x0B:		//DEC BC
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "DEC BC";
			break;

		case 0x1B:		//DEC DE
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "DEC DE";
			break;

		case 0x2B:		//DEC HL
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "DEC HL";
			break;

		case 0x3B:		//DEC SP
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "DEC SP";
			break;

		case 0xCB:
			switch (ZBCPU.CB_opcode)
			{
				case 0x37:		//SWAP A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SWAP A";
					break;

				case 0x30:		//SWAP B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SWAP B";
					break;

				case 0x31:		//SWAP C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SWAP C";
					break;

				case 0x32:		//SWAP D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SWAP D";
					break;

				case 0x33:		//SWAP E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SWAP E";
					break;

				case 0x34:		//SWAP H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SWAP H";
					break;

				case 0x35:		//SWAP L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SWAP L";
					break;

				case 0x36:		//SWAP (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SWAP (HL)";
					break;

				case 0x07:		//RLC A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RLC A";
					break;

				case 0x00:		//RLC B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RLC B";
					break;

				case 0x01:		//RLC C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RLC C";
					break;

				case 0x02:		//RLC D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RLC D";
					break;

				case 0x03:		//RLC E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RLC E";
					break;

				case 0x04:		//RLC H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RLC H";
					break;

				case 0x05:		//RLC L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RLC L";
					break;

				case 0x06:		//RLC (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RLC (HL)";
					break;

				case 0x17:		//RL A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RL A";
					break;

				case 0x10:		//RL B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RL B";
					break;

				case 0x11:		//RL C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RL C";
					break;

				case 0x12:		//RL D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RL D";
					break;

				case 0x13:		//RL E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RL E";
					break;

				case 0x14:		//RL H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RL H";
					break;

				case 0x15:		//RL L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RL L";
					break;

				case 0x16:		//RL (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RL (HL)";
					break;

				case 0x0F:		//RRC A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RRC A";
					break;

				case 0x08:		//RRC B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RRC B";
					break;

				case 0x09:		//RRC C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RRC C";
					break;

				case 0x0A:		//RRC D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RRC D";
					break;

				case 0x0B:		//RRC E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RRC E";
					break;

				case 0x0C:		//RRC H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RRC H";
					break;

				case 0x0D:		//RRC L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RRC L";
					break;

				case 0x0E:		//RRC (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RRC (HL)";
					break;

				case 0x1F:		//RR A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RR A";
					break;

				case 0x18:		//RR B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RR B";
					break;

				case 0x19:		//RR C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RR C";
					break;

				case 0x1A:		//RR D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RR D";
					break;

				case 0x1B:		//RR E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RR E";
					break;

				case 0x1C:		//RR H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RR H";
					break;

				case 0x1D:		//RR L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RR L";
					break;

				case 0x1E:		//RR (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RR (HL)";
					break;

				case 0x27:		//SLA A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SLA A";
					break;

				case 0x20:		//SLA B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SLA B";
					break;

				case 0x21:		//SLA C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SLA C";
					break;

				case 0x22:		//SLA D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SLA D";
					break;

				case 0x23:		//SLA E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SLA E";
					break;

				case 0x24:		//SLA H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SLA H";
					break;

				case 0x25:		//SLA L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SLA L";
					break;

				case 0x26:		//SLA (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SLA (HL)";
					break;

				case 0x2F:		//SRA A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SRA A";
					break;

				case 0x28:		//SRA B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SRA B";
					break;

				case 0x29:		//SRA C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SRA C";
					break;

				case 0x2A:		//SRA D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SRA D";
					break;

				case 0x2B:		//SRA E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SRA E";
					break;

				case 0x2C:		//SRA H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SRA H";
					break;

				case 0x2D:		//SRA L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SRA L";
					break;

				case 0x2E:		//SRA (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SRA (HL)";
					break;

				case 0x3F:		//SRL A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SRL A";
					break;

				case 0x38:		//SRL B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SRL B";
					break;

				case 0x39:		//SRL C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SRL C";
					break;

				case 0x3A:		//SRL D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SRL D";
					break;

				case 0x3B:		//SRL E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SRL E";
					break;

				case 0x3C:		//SRL H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SRL H";
					break;

				case 0x3D:		//SRL L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SRL L";
					break;

				case 0x3E:		//SRL (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SRL (HL)";
					break;

				case 0x47:		//BIT 0, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 0, A";
					break;

				case 0x4F:		//BIT 1, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 1, A";
					break;

				case 0x57:		//BIT 2, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 2, A";
					break;

				case 0x5F:		//BIT 3, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 3, A";
					break;

				case 0x67:		//BIT 4, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 4, A";
					break;

				case 0x6F:		//BIT 5, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 5, A";
					break;

				case 0x77:		//BIT 6, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 6, A";
					break;

				case 0x7F:		//BIT 7, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 7, A";
					break;

				case 0x40:		//BIT 0, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 0, B";
					break;

				case 0x48:		//BIT 1, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 1, B";
					break;

				case 0x50:		//BIT 2, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 2, B";
					break;

				case 0x58:		//BIT 3, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 3, B";
					break;

				case 0x60:		//BIT 4, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 4, B";
					break;

				case 0x68:		//BIT 5, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 5, B";
					break;

				case 0x70:		//BIT 6, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 6, B";
					break;

				case 0x78:		//BIT 7, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 7, B";
					break;

				case 0x41:		//BIT 0, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 0, C";
					break;

				case 0x49:		//BIT 1, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 1, C";
					break;

				case 0x51:		//BIT 2, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 2, C";
					break;

				case 0x59:		//BIT 3, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 3, C";
					break;

				case 0x61:		//BIT 4, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 4, C";
					break;

				case 0x69:		//BIT 5, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 5, C";
					break;

				case 0x71:		//BIT 6, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 6, C";
					break;

				case 0x79:		//BIT 7, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 7, C";
					break;

				case 0x42:		//BIT 0, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 0, D";
					break;

				case 0x4A:		//BIT 1, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 1, D";
					break;

				case 0x52:		//BIT 2, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 2, D";
					break;

				case 0x5A:		//BIT 3, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 3, D";
					break;

				case 0x62:		//BIT 4, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 4, D";
					break;

				case 0x6A:		//BIT 5, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 5, D";
					break;

				case 0x72:		//BIT 6, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 6, D";
					break;

				case 0x7A:		//BIT 7, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 7, D";
					break;

				case 0x43:		//BIT 0, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 0, E";
					break;

				case 0x4B:		//BIT 1, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 1, E";
					break;

				case 0x53:		//BIT 2, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 2, E";
					break;

				case 0x5B:		//BIT 3, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 3, E";
					break;

				case 0x63:		//BIT 4, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 4, E";
					break;

				case 0x6B:		//BIT 5, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 5, E";
					break;

				case 0x73:		//BIT 6, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 6, E";
					break;

				case 0x7B:		//BIT 7, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 7, E";
					break;

				case 0x44:		//BIT 0, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 0, H";
					break;

				case 0x4C:		//BIT 1, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 1, H";
					break;

				case 0x54:		//BIT 2, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 2, H";
					break;

				case 0x5C:		//BIT 3, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 3, H";
					break;

				case 0x64:		//BIT 4, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 4, H";
					break;

				case 0x6C:		//BIT 5, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 5, H";
					break;

				case 0x74:		//BIT 6, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 6, H";
					break;

				case 0x7C:		//BIT 7, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 7, H";
					break;

				case 0x45:		//BIT 0, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 0, L";
					break;

				case 0x4D:		//BIT 1, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 1, L";
					break;

				case 0x55:		//BIT 2, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 2, L";
					break;

				case 0x5D:		//BIT 3, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 3, L";
					break;

				case 0x65:		//BIT 4, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 4, L";
					break;

				case 0x6D:		//BIT 5, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 5, L";
					break;

				case 0x75:		//BIT 6, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 6, L";
					break;

				case 0x7D:		//BIT 7, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 7, L";
					break;

				case 0x46:		//BIT 0, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 0, (HL)";
					break;

				case 0x4E:		//BIT 1, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 1, (HL)";
					break;

				case 0x56:		//BIT 2, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 2, (HL)";
					break;

				case 0x5E:		//BIT 3, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 3, (HL)";
					break;

				case 0x66:		//BIT 4, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 4, (HL)";
					break;

				case 0x6E:		//BIT 5, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 5, (HL)";
					break;

				case 0x76:		//BIT 6, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 6, (HL)";
					break;

				case 0x7E:		//BIT 7, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "BIT 7, (HL)";
					break;

				case 0xC7:		//SET 0, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 0, A";
					break;

				case 0xCF:		//SET 1, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 1, A";
					break;

				case 0xD7:		//SET 2, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 2, A";
					break;

				case 0xDF:		//SET 3, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 3, A";
					break;

				case 0xE7:		//SET 4, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 4, A";
					break;

				case 0xEF:		//SET 5, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 5, A";
					break;

				case 0xF7:		//SET 6, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 6, A";
					break;

				case 0xFF:		//SET 7, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 7, A";
					break;

				case 0xC0:		//SET 0, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 0, B";
					break;

				case 0xC8:		//SET 1, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 1, B";
					break;

				case 0xD0:		//SET 2, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 2, B";
					break;

				case 0xD8:		//SET 3, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 3, B";
					break;

				case 0xE0:		//SET 4, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 4, B";
					break;

				case 0xE8:		//SET 5, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 5, B";
					break;

				case 0xF0:		//SET 6, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 6, B";
					break;

				case 0xF8:		//SET 7, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 7, B";
					break;

				case 0xC1:		//SET 0, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 0, C";
					break;

				case 0xC9:		//SET 1, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 1, C";
					break;

				case 0xD1:		//SET 2, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 2, C";
					break;

				case 0xD9:		//SET 3, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 3, C";
					break;

				case 0xE1:		//SET 4, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 4, C";
					break;

				case 0xE9:		//SET 5, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 5, C";
					break;

				case 0xF1:		//SET 6, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 6, C";
					break;

				case 0xF9:		//SET 7, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 7, C";
					break;

				case 0xC2:		//SET 0, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 0, D";
					break;

				case 0xCA:		//SET 1, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 1, D";
					break;

				case 0xD2:		//SET 2, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 2, D";
					break;

				case 0xDA:		//SET 3, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 3, D";
					break;

				case 0xE2:		//SET 4, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 4, D";
					break;

				case 0xEA:		//SET 5, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 5, D";
					break;

				case 0xF2:		//SET 6, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 6, D";
					break;

				case 0xFA:		//SET 7, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 7, D";
					break;

				case 0xC3:		//SET 0, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 0, E";
					break;

				case 0xCB:		//SET 1, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 1, E";
					break;

				case 0xD3:		//SET 2, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 2, E";
					break;

				case 0xDB:		//SET 3, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 3, E";
					break;

				case 0xE3:		//SET 4, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 4, E";
					break;

				case 0xEB:		//SET 5, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 5, E";
					break;

				case 0xF3:		//SET 6, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 6, E";
					break;

				case 0xFB:		//SET 7, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 7, E";
					break;

				case 0xC4:		//SET 0, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 0, H";
					break;

				case 0xCC:		//SET 1, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 1, H";
					break;

				case 0xD4:		//SET 2, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 2, H";
					break;

				case 0xDC:		//SET 3, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 3, H";
					break;

				case 0xE4:		//SET 4, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 4, H";
					break;

				case 0xEC:		//SET 5, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 5, H";
					break;

				case 0xF4:		//SET 6, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 6, H";
					break;

				case 0xFC:		//SET 7, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 7, H";
					break;

				case 0xC5:		//SET 0, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 0, L";
					break;

				case 0xCD:		//SET 1, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 1, L";
					break;

				case 0xD5:		//SET 2, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 2, L";
					break;

				case 0xDD:		//SET 3, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 3, L";
					break;

				case 0xE5:		//SET 4, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 4, L";
					break;

				case 0xED:		//SET 5, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 5, L";
					break;

				case 0xF5:		//SET 6, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 6, L";
					break;

				case 0xFD:		//SET 7, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 7, L";
					break;

				case 0xC6:		//SET 0, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 0, (HL)";
					break;

				case 0xCE:		//SET 1, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 1, (HL)";
					break;

				case 0xD6:		//SET 2, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 2, (HL)";
					break;

				case 0xDE:		//SET 3, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 3, (HL)";
					break;

				case 0xE6:		//SET 4, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 4, (HL)";
					break;

				case 0xEE:		//SET 5, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 5, (HL)";
					break;

				case 0xF6:		//SET 6, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 6, (HL)";
					break;

				case 0xFE:		//SET 7, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "SET 7, (HL)";
					break;

				case 0x87:		//RES 0, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 0, A";
					break;

				case 0x8F:		//RES 1, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 1, A";
					break;

				case 0x97:		//RES 2, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 2, A";
					break;

				case 0x9F:		//RES 3, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 3, A";
					break;

				case 0xA7:		//RES 4, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 4, A";
					break;

				case 0xAF:		//RES 5, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 5, A";
					break;

				case 0xB7:		//RES 6, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 6, A";
					break;

				case 0xBF:		//RES 7, A
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 7, A";
					break;

				case 0x80:		//RES 0, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 0, B";
					break;

				case 0x88:		//RES 1, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 1, B";
					break;

				case 0x90:		//RES 2, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 2, B";
					break;

				case 0x98:		//RES 3, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 3, B";
					break;

				case 0xA0:		//RES 4, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 4, B";
					break;

				case 0xA8:		//RES 5, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 5, B";
					break;

				case 0xB0:		//RES 6, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 6, B";
					break;

				case 0xB8:		//RES 7, B
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 7, B";
					break;

				case 0x81:		//RES 0, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 0, C";
					break;

				case 0x89:		//RES 1, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 1, C";
					break;

				case 0x91:		//RES 2, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 2, C";
					break;

				case 0x99:		//RES 3, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 3, C";
					break;

				case 0xA1:		//RES 4, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 4, C";
					break;

				case 0xA9:		//RES 5, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 5, C";
					break;

				case 0xB1:		//RES 6, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 6, C";
					break;

				case 0xB9:		//RES 7, C
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 7, C";
					break;

				case 0x82:		//RES 0, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 0, D";
					break;

				case 0x8A:		//RES 1, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 1, D";
					break;

				case 0x92:		//RES 2, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 2, D";
					break;

				case 0x9A:		//RES 3, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 3, D";
					break;

				case 0xA2:		//RES 4, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 4, D";
					break;

				case 0xAA:		//RES 5, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 5, D";
					break;

				case 0xB2:		//RES 6, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 6, D";
					break;

				case 0xBA:		//RES 7, D
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 7, D";
					break;

				case 0x83:		//RES 0, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 0, E";
					break;

				case 0x8B:		//RES 1, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 1, E";
					break;

				case 0x93:		//RES 2, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 2, E";
					break;

				case 0x9B:		//RES 3, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 3, E";
					break;

				case 0xA3:		//RES 4, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 4, E";
					break;

				case 0xAB:		//RES 5, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 5, E";
					break;

				case 0xB3:		//RES 6, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 6, E";
					break;

				case 0xBB:		//RES 7, E
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 7, E";
					break;

				case 0x84:		//RES 0, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 0, H";
					break;

				case 0x8C:		//RES 1, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 1, H";
					break;

				case 0x94:		//RES 2, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 2, H";
					break;

				case 0x9C:		//RES 3, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 3, H";
					break;

				case 0xA4:		//RES 4, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 4, H";
					break;

				case 0xAC:		//RES 5, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 5, H";
					break;

				case 0xB4:		//RES 6, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 6, H";
					break;

				case 0xBC:		//RES 7, H
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 7, H";
					break;

				case 0x85:		//RES 0, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 0, L";
					break;

				case 0x8D:		//RES 1, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 1, L";
					break;

				case 0x95:		//RES 2, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 2, L";
					break;

				case 0x9D:		//RES 3, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 3, L";
					break;

				case 0xA5:		//RES 4, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 4, L";
					break;

				case 0xAD:		//RES 5, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 5, L";
					break;

				case 0xB5:		//RES 6, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 6, L";
					break;

				case 0xBD:		//RES 7, L
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 7, L";
					break;

				case 0x86:		//RES 0, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 0, (HL)";
					break;

				case 0x8E:		//RES 1, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 1, (HL)";
					break;

				case 0x96:		//RES 2, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 2, (HL)";
					break;

				case 0x9E:		//RES 3, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 3, (HL)";
					break;

				case 0xA6:		//RES 4, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 4, (HL)";
					break;

				case 0xAE:		//RES 5, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 5, (HL)";
					break;

				case 0xB6:		//RES 6, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 6, (HL)";
					break;

				case 0xBE:		//RES 7, (HL)
					std::cout << std::left << std::setfill(' ') << std::setw(30) << "RES 7, (HL)";
					break;
			}
			break;

		case 0x27:		//DAA
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "DAA";
			break;

		case 0x2F:		//CPL
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "CPL A";
			break;

		case 0x3F:		//CCF
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "CCF";
			break;

		case 0x37:		//SCF
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "SCF";
			break;

		case 0x76:		//HALT
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "HALT";
			break;

		case 0x10:		//STOP
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "STOP";
			break;

		case 0xF3:		//DI
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "DI";
			break;

		case 0xFB:		//EI
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "EI";
			break;

		case 0x07:		//RLCA
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "RLCA";
			break;

		case 0x17:		//RLA
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "RLA";
			break;

		case 0x0F:		//RRCA
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "RRCA";
			break;

		case 0x1F:		//RRA
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "RRA";
			break;

		case 0xC3:		//JP nn
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "JP nn";
			break;

		case 0xC2:		//JP NZ, nn
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "JP NZ, nn";
			break;

		case 0xCA:		//JP Z, nn
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "JP Z, nn";
			break;

		case 0xD2:		//JP NC, nn
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "JP NC, nn";
			break;

		case 0xDA:		//JP C, nn
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "JP C, nn";
			break;

		case 0xE9:		//JP (HL)
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "JP (HL)";
			break;

		case 0x18:		//JR n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "JR n";
			break;

		case 0x20:		//JR NZ, n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "JR NZ, n";
			break;

		case 0x28:		//JR Z, n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "JR Z, n";
			break;

		case 0x30:		//JR NC, n
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "JR NC, n";
			break;

		case 0x38:		//JR C, n			
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "JR C, n";
			break;

		case 0xCD:		//CALL nn
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "CALL nn";
			break;

		case 0xC4:		//CALL NZ, nn
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "CALL NZ, nn";
			break;

		case 0xCC:		//CALL Z, nn
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "CALL Z, nn";
			break;

		case 0xD4:		//CALL NC, nn
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "CALL NC, nn";
			break;

		case 0xDC:		//CALL C, nn
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "CALL C, nn";
			break;

		case 0xC7:		//RST 00
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "RST 00";
			break;

		case 0xCF:		//RST 08
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "RST 08";
			break;

		case 0xD7:		//RST 10
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "RST 10";
			break;

		case 0xDF:		//RST 18
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "RST 18";
			break;

		case 0xE7:		//RST 20
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "RST 20";
			break;

		case 0xEF:		//RST 28
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "RST 28";
			break;

		case 0xF7:		//RST 30
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "RST 30";
			break;

		case 0xFF:		//RST 38
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "RST 38";
			break;

		case 0xC9:		//RET
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "RET";
			break;

		case 0xC0:		//RET NZ
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "RET NZ";
			break;

		case 0xC8:		//RET Z
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "RET Z";
			break;

		case 0xD0:		//RET NC
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "RET NC";
			break;

		case 0xD8:		//RET C
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "RET C";
			break;

		case 0xD9:		//RETI
			std::cout << std::left << std::setfill(' ') << std::setw(30) << "RETI";
			break;

		default:
			std::cout << "ERROR: unrecognized opcode";
			break;
	}

	std::cout << std::endl;
}