#include "chip8.h"

void Chip8::initialize()
{
	// Initialize registers and memory once
	pc = 0x200;  // Program counter starts at 0x200
	opcode = 0;      // Reset current opcode	
	I = 0;      // Reset index register
	sp = 0;      // Reset stack pointer

	// clear display, stack, register by dynamically get the number of elements
	std::fill_n(screen, sizeof(screen) / sizeof(unsigned char), 0);
	std::fill_n(stack, sizeof(stack) / sizeof(unsigned short), 0);
	std::fill_n(registers, sizeof(registers) / sizeof(unsigned short), 0);
	std::fill_n(memory, sizeof(memory) / sizeof(unsigned char), 0);

	
	// Load fontset
	for (int i = 0; i < 80; ++i)
		memory[i] = chip8_fontset[i];

	srand(time(0));

}

void Chip8::loadGame(std::string game_path)
{
	std::ifstream file(game_path, std::ios::binary);

	if (!file)
	{
		std::cerr << "Erreur lors de l'ouverture du fichier." << std::endl;
		return;
	}

	// Obtenez la taille du fichier
	file.seekg(0, std::ios::end);
	std::streamoff fileSize = file.tellg(); // TODO : Implement security check for size
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(fileSize);
	file.read(buffer.data(), fileSize);

	for (int i = 0; i < fileSize; ++i)
		memory[i + 512] = buffer[i];

}


void Chip8::emulateCycle(sf::RenderWindow& renderer)
{
	// Fetch Opcode : 0xA8 0x75 ->   0xA8__   ->   0xA875
	unsigned short opcode = memory[pc] << 8 | memory[pc + 1];
	unsigned char xRegisterIndex = opcode & 0x0F00 >> 8;
	unsigned char yRegisterIndex = opcode & 0x00F0 >> 4;

	switch (opcode & 0xF000)
	{

	case 0x0000:
		switch (opcode & 0x00FF)
		{
		case 0x00E0: // CLS
			renderer.clear(sf::Color::Black);
			memset(screen, 0, sizeof(screen));
			break;

		case 0x00EE: // RET
			//sp--;
			pc = stack[sp];
			sp--;
			break;

		default: // SYS
			printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
			break;
		}
		pc += 2;
		break;

	case 0x1000: // JMP
		pc = opcode & 0x0FFF;
		break;

	case 0x2000: // CALL addr
		stack[sp] = pc;
		sp++;
		pc = opcode & 0x0FFF;
		break;

	case 0x3000: // Skip next instruction if Vx == kk.
		if (registers[xRegisterIndex] == (opcode & 0x00FF))
			pc += 4;
		else
			pc += 2;
		break;

	case 0x4000: // Skip next instruction if Vx != kk.
		if (registers[xRegisterIndex] != (opcode & 0x00FF))
			pc += 4;
		else
			pc += 2;
		break;

	case 0x5000: // SE Skip next instruction if Vx == Vy.
		if (registers[xRegisterIndex] == registers[yRegisterIndex])
			pc += 4;
		else
			pc += 2;
		break;

	case 0x6000: // LD, set value for a register
		registers[xRegisterIndex] = opcode & 0x00FF;
		pc += 2;
		break;

	case 0x7000: // ADD, add value for a register
		registers[xRegisterIndex] += opcode & 0x00FF;
		pc += 2;
		break;


	case 0x8000: // Register & bits operation
		switch (opcode & 0x000F)
		{
		case 0x0: // Set
			registers[xRegisterIndex] = registers[yRegisterIndex];
			break;

		case 0x1: // OR
			registers[xRegisterIndex] |= registers[yRegisterIndex];
			break;

		case 0x2: // AND
			registers[xRegisterIndex] &= registers[yRegisterIndex];
			break;

		case 0x3: // XOR
			registers[xRegisterIndex] ^= registers[yRegisterIndex];
			break;

		case 0x4: // Plus
			if (registers[xRegisterIndex] + registers[yRegisterIndex] > 255)
				registers[0xF] = 1;
			else
				registers[0xF] = 0;
			registers[xRegisterIndex] += registers[yRegisterIndex];
			break;

		case 0x5: // SUB
			if (registers[xRegisterIndex] > registers[yRegisterIndex])
				registers[0xF] = 1;
			else
				registers[0xF] = 0;
			registers[xRegisterIndex] -= registers[yRegisterIndex];
			break;

		case 0x6: // SHR
			registers[0xF] = registers[xRegisterIndex] & 0x1;
			registers[xRegisterIndex] >>= 1;
			break;

		case 0x7: // SUBN
			if (registers[yRegisterIndex] > registers[xRegisterIndex])
				registers[0xF] = 1;
			else
				registers[0xF] = 0;
			registers[xRegisterIndex] = registers[yRegisterIndex] - registers[xRegisterIndex];
			break;

		case 0xE: // SHL
			registers[0xF] = registers[xRegisterIndex] & 0x1;
			registers[xRegisterIndex] <<= 1;
			break;

		default:
			printf("Unknown register or bits operation opcode [0x8000] : 0x%X\n", opcode);
			break;
		}
		pc += 2;
		break;

	case 0x9000: // SNE Skip next instruction if Vx == Vy.
		if (registers[xRegisterIndex] != registers[yRegisterIndex])
			pc += 4;
		else
			pc += 2;
		break;

	case 0xA000: // LD Sets I to adress
		I = opcode & 0x0FFF;
		pc += 2;
		break;

	case 0xB000: // JP V0, addr
		pc = registers[0] + (opcode & 0x0FFF);
		break;

	case 0xC000: // Random
		srand(time(0));
		registers[xRegisterIndex] = (rand() % 255) & (opcode & 0x00FF);
		pc += 2;
		break;

	case 0xD000:
	{
		unsigned short x = registers[xRegisterIndex];
		unsigned short y = registers[yRegisterIndex];
		unsigned short height = opcode & 0x000F;
		unsigned short pixel;

		registers[0xF] = 0;
		for (int yline = 0; yline < height; yline++)
		{
			pixel = memory[I + yline];
			for (int xline = 0; xline < 8; xline++)
			{
				if ((pixel & (0x80 >> xline)) != 0)
				{
					if (screen[(x + xline + ((y + yline) * 64))] == 1)
						registers[0xF] = 1;
					screen[x + xline + ((y + yline) * 64)] ^= 1;
				}
			}
		}
		//drawFlag = true;
		pc += 2;
		break;
	}

	case 0xE000:
		switch (opcode & 0x00FF)
		{

		case 0x009E:
			if (key[registers[xRegisterIndex]] == true)
				pc += 4;
			else
				pc += 2;
			break;

		case 0x00A1:
			if (key[registers[xRegisterIndex]] == false)
				pc += 4;
			else
				pc += 2;
			break;
		}

		break;

	case 0xF000:
		switch (opcode & 0x00FF)
		{

		case 0x0007: // Set Vx = delay timer value.
			registers[xRegisterIndex] = delay_timer;
			pc += 2;
			break;

		case 0x000A: // Wait for a key press
		{
			bool keyPress = false;
			for (int i = 0; i < 16; ++i)
			{
				if (key[i] != 0)
				{
					registers[xRegisterIndex] = i;
					keyPress = true;
				}
			}
			if (!keyPress)
				return;
			pc += 2;
			break;
		}

		case 0x0015: // Set delay timer = Vx.
			delay_timer = registers[xRegisterIndex];
			pc += 2;
			break;

		case 0x0018: // Set sound timer = Vx
			sound_timer = registers[xRegisterIndex];
			pc += 2;
			break;

		case 0x001E: // Set I = I + Vx.
			I += registers[xRegisterIndex];
			pc += 2;
			break;

		case 0x0029: //  location of sprite for digit Vx.
			I = registers[xRegisterIndex] * 0x5;
			pc += 2;
			break;
		
		case 0x0033: // LD B, Vx
			memory[I] = registers[xRegisterIndex] / 100;
			memory[I + 1] = (registers[xRegisterIndex] / 10) % 10;
			memory[I + 2] = (registers[xRegisterIndex] % 100) % 10;
			pc += 2;
			break;

		case 0x0055:
			for (int i = 0; i <= xRegisterIndex; ++i)
				memory[I + i] = registers[i];
			I += ((opcode & 0x0F00) >> 8) + 1;
			pc += 2;
			break;

		case 0x0065:
			for (int i = 0; i <= xRegisterIndex; ++i)
				registers[i] = memory[I + i];
			I += ((opcode & 0x0F00) >> 8) + 1;
			pc += 2;
			break;

		}
		break;

	default:
		printf("Unknown opcode: 0x%X\n", opcode);
		break;

	}
	std::cout << std::hex << opcode << std::endl;

	// Update timers
	if (delay_timer > 0)
		--delay_timer;

	if (sound_timer > 0)
	{
		if (sound_timer == 1)
			printf("BEEP!\n");
		--sound_timer;
	}


}