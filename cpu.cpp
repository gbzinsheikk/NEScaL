#include <iostrem>
#include "olc6502.h"

olc6502::olc6502()
{
	using a = olc6502;
	lookup = 
	{
		{"BRK", &a::BRK, &a::IMM, 7},{"ORA", &a::ORA, &a::IZX, 7}






	}
}

olc6502::~olc6502()
{

}

uint8_t olc6502::read(uint16_t a)
{
	return bus->read(a, false);
}

void olc6502::write(uint16_t a, uint8_t d)
{
	bus->write(a,d);
}

void olc6502::clock()
{
	if (cycles == 0)
	{
		opcode = read(pc);
		pc++

		cycles = lookup[opcode].cycles;

		uint8_t additional_cycle1 = (this->*lookup[opcode].addrmode)();

		uint8_t additional_cycle2 = (this->*lookup[opcode].operate)();

		cycles += (additional_cycle1 & additional_cycle2);
	}

	cycles--;
}

void olc6502::SetFlag (FLAGS6502 f, bool v);
{
	if (v)
		status |= f;
	else
		status &= ~f;
}


uint8_t olc6502::IMP()
{
	fetched = a;
	return 0;
}

uint8_t olc6502::IMM()
{
	addr_abs = pc++;
	return 0;
}

uint8_t olc6502::ZPO()
{
	addr_abs = read(pc);
	pc++;
	addr_abs &= 0x00FF;
	return 0;
}

uint8_t olc6502::ZPX()
{
	addr_abs = read(pc) + x;
	pc++;
	addr_abs &= 0x00FF;
	return 0;
}

uint8_t olc6502::ZPY()
{
	addr_abs = read(pc) + y;
	pc++;
	addr_abs &= 0x00FF;
	return 0;
}

uint8_t olc6502::ABS()
{
	uint16_t  lo = read(pc);
	pc++;
	uint16_t  hi = read(pc);
	pc++

	addr_abs = (h1 << 8) | lo;

	return 0;
}

uint8_t olc6502::ABX()
{
	uint16_t  lo = read(pc);
	pc++;
	uint16_t  hi = read(pc);
	pc++

	addr_abs = (h1 << 8) | lo;
	addr_abs += x;

	if ((addr_abs & 0xFF00) !=  (hi << 8))
		return 1;
	else
		return 0;
}

uint8_t olc6502::ABY()
{
	uint16_t  lo = read(pc);
	pc++;
	uint16_t  hi = read(pc);
	pc++

	addr_abs = (h1 << 8) | lo;
	addr_abs += x;

	if ((addr_abs & 0xFF00) !=  (hi << 8))
		return 1;
	else
		return 0;
}

uint8_t olc6502::IND()
{
	uint16_t  ptr_lo = read(pc);
	pc++;
	uint16_t  ptr_hi = read(pc);
	pc++

	uint16_t ptr = (ptr_hi << 8) | ptr_lo;
	
	if (ptr_lo == 0X00FF)
	{
			addr_abs = (read(ptr & 0xFF00) << 8) | read(ptr + 0);
	}
	else
	{
		addr_abs = (read(ptr+1) << 8 | readptr + 0); 
	}
	return 0;
}

uint8_t olc6502::IZX()
{
	uint16_t t = read(pc);
	pc++;

	uint16_t lo = read ((uint16_t)(t + (uint16_t)x) & 0x00FF);
	uint16_t hi = read ((uint16_t)(t + (uint16_t)x + 1) & 0x00FF);

	addr_abs = (hi << 8) | lo;
	return 0;
}

uint8_t olc6502::IZY()
{
	uint16_t t = read(pc);
	pc++;

	uint16_t lo = read ((uint16_t)(t + (uint16_t)x) & 0x00FF);
	uint16_t hi = read ((uint16_t)(t + (uint16_t)x + 1) & 0x00FF);

	addr_abs = (hi << 8) | lo;
	addr_abs += y;

	if ((addr_abs & 0x00FF) != (hi << 8))
		return 1;
	else 
		return 0;

}

uint8_t olc6502::REL()
{
	addr_rel = read(pc);
	pc++;
	if (addr_rel & 0x80)
		addr_rel |= 0xFF00;
	return 0;
}

uint8_t olc6502::fetch()
{
	if (!(lookup[opcode].addrmode == &olc6502::IMP))
		fetched = read(addr_abs);
	return fetched;
}

uint8_t olc6502::AND()
{
	fetch();
	a = a & fetched;	
	SetFlag(Z, a == 0x00);
	SetFlag(N, a & 0x80);
	return 1;
}



