#include "PPU.h"
#include "GameBoy.h"
#include "bitwise.h"

PPU::PPU(CPU &zbCPU, MMU &zbMMU) : cpu(zbCPU), mmu(zbMMU), buffer(SCREEN_WIDTH, SCREEN_HEIGHT)
{
	PPU_Mode = 0;
	line = 0x00;
}

PPU::~PPU()
{

}

void PPU::setupDrawFunction(std::function<void(const Buffer &buffer)> drawFunc)
{
	draw = drawFunc;
}

int PPU::getMode()
{
	return PPU_Mode;
}

sf::Color PPU::getColor(bool highBit, bool lowBit)
{
	if (!highBit && !lowBit)
		return sf::Color(232, 232, 232);
	if (!highBit && lowBit)
		return sf::Color(160, 160, 160);
	if (highBit && !lowBit)
		return sf::Color(88, 88, 88);

	return sf::Color(16, 16, 16);
}

//0 for OBP0, 1 for OBP1, 2 for BGP
sf::Color PPU::getColorFromPalette(unsigned char pixel, int paletteSelection)
{
	sf::Color pixelColor;
	unsigned char BGP = *(mmu.BGP);
	unsigned char OBP0 = *(mmu.OBP0);
	unsigned char OBP1 = *(mmu.OBP1);
	int highBitIndex = 0;
	int lowBitIndex = 0;

	switch (pixel)
	{
		case 0:
			highBitIndex = 1;
			lowBitIndex = 0;
			break;
		case 1:
			highBitIndex = 3;
			lowBitIndex = 2;
			break;
		case 2:
			highBitIndex = 5;
			lowBitIndex = 4;
			break;
		case 3:
			highBitIndex = 7;
			lowBitIndex = 6;
			break;
	};

	switch (paletteSelection)
	{
		case 0:
			pixelColor = getColor(bitwise::check_bit(OBP0, highBitIndex), bitwise::check_bit(OBP0, lowBitIndex));
			break;
		case 1:
			pixelColor = getColor(bitwise::check_bit(OBP1, highBitIndex), bitwise::check_bit(OBP1, lowBitIndex));
			break;
		case 2:
			pixelColor = getColor(bitwise::check_bit(BGP, highBitIndex), bitwise::check_bit(BGP, lowBitIndex));
			break;
	};

	return pixelColor;
}

void PPU::tick(int cyclesRemaining)
{
	unsigned char LCDC = *(mmu.LCDC);

	if (!bitwise::check_bit(LCDC, 7))
		return;

	unsigned char SCX = *(mmu.SCX);
	unsigned char SCY = *(mmu.SCY);
	unsigned char WX = *(mmu.WX);
	unsigned char WY = *(mmu.WY);
	unsigned char LY = *(mmu.LY);
	unsigned char LYC = *(mmu.LYC);
	int spriteHeight = bitwise::check_bit(LCDC, 2) ? 16 : 8;
	bool objEnable = bitwise::check_bit(LCDC, 1);
	bool bgEnable = bitwise::check_bit(LCDC, 0);
	bool requestSTAT = false;

	while (cyclesRemaining > 0)
	{
		unsigned char STAT = *(mmu.STAT);
		unsigned char realPPUMode = (PPU_Mode + 2) % 4;
		unsigned char prevSTATSources = (realPPUMode == 0 && bitwise::check_bit(STAT, 3)) ||
			(realPPUMode == 1 && bitwise::check_bit(STAT, 4)) ||
			(realPPUMode == 2 && bitwise::check_bit(STAT, 5)) ||
			(LYEqualLYC && bitwise::check_bit(STAT, 6));

		LYEqualLYC = (LY == LYC);
		requestSTAT = LYEqualLYC;

		if (newScanline)
			discardCount = SCX % 8;

		switch (PPU_Mode)
		{
			case 0:		//Real mode 2, OAM search (80 dots)
				curSprite.yPos = mmu.read(oamStartAddr + oamIndex);
				curSprite.xPos = mmu.read(oamStartAddr + oamIndex + 1);
				curSprite.tileIndex = mmu.read(oamStartAddr + oamIndex + 2) << 4;
				curSprite.attributeFlags = mmu.read(oamStartAddr + oamIndex + 3);

				if (curSprite.xPos > 0 && (LY + 16) >= curSprite.yPos && (LY + 16) < (curSprite.yPos + spriteHeight) && spriteBuffer.size() < 10)
					spriteBuffer.push_back(curSprite);

				oamIndex += 4;
				cycleCount++;

				if (cycleCount >= CLOCKS_PER_SCANLINE_OAM)
				{
					cycleCount = cycleCount % CLOCKS_PER_SCANLINE_OAM;

					mmu.write(0xFF41, mmu.read(0xFF41) & 0xFC);
					mmu.write(0xFF41, mmu.read(0xFF41) | 0x03);

					PPU_Mode = 1;
					oamIndex = 0;

					savedSprites = spriteBuffer;
				}

				cyclesRemaining--;
				break;

			case 1:		//Real mode 3, data transfer to LCD driver (174 - 289 dots)
				if (!spriteFetch && !spriteFetchWaiting && objEnable)
				{
					for (int i = 0; i < spriteBuffer.size(); i++)
					{
						if (spriteBuffer[i].xPos <= LX + 8)
						{
							bgFetcherState == 3 ? spriteFetch = true : spriteFetchWaiting = true;
							spriteIndex = i;
							break;
						}
					}
				}

				if (spriteFetch)	//Fetching sprite
				{
					switch (spriteFetcherState)
					{
						case 0:
							spriteFetchWaiting = false;
							objTileID = spriteBuffer[spriteIndex].tileIndex;
							spriteFetcherState++;
							break;
						case 1:
							objTileFetchAddr = 0x8000 + objTileID;
							objLowerNibble = 2 * ((LY + SCY) % 8);
							objTileFetchAddr |= objLowerNibble;

							objTileDataLow = mmu.read(objTileFetchAddr);

							spriteFetcherState++;
							break;
						case 2:
							objTileDataHigh = mmu.read(objTileFetchAddr + 1);
							spriteFetcherState++;
							break;
						case 3:
							int curPixels = spriteFIFO.size();
							//int pixelsOffScreen = 8 - spriteBuffer[spriteIndex].xPos;

							for (int i = 7 - curPixels; i >= 0; i--)
							{
								if (!bitwise::check_bit(objTileDataLow, i) && !bitwise::check_bit(objTileDataHigh, i))
									spriteFIFO.push(0x00);
								else if (!bitwise::check_bit(objTileDataLow, i) && bitwise::check_bit(objTileDataHigh, i))
									spriteFIFO.push(0x02);
								else if (bitwise::check_bit(objTileDataLow, i) && !bitwise::check_bit(objTileDataHigh, i))
									spriteFIFO.push(0x01);
								else
									spriteFIFO.push(0x03);
							}

							if (!spriteBuffer.empty())
								spriteBuffer.erase(spriteBuffer.begin() + spriteIndex);

							spriteFetcherState = 0;
							spriteFetch = false;
							break;
					}
				}

				if (!spriteFetch)
				{
					switch (bgFetcherState)		//Fetching background
					{
						case 0:
							if (!windowMode)	//Background mode
							{
								tilemapAddr = !bitwise::check_bit(LCDC, 3) ? 0x9800 : 0x9C00;

								upperNibble = (LY + SCY) / 8;
								lowerNibble = (LX + SCX) / 8;
							}

							else	//Window mode
							{
								tilemapAddr = !bitwise::check_bit(LCDC, 6) ? 0x9800 : 0x9C00;

								upperNibble = WY / 8;
								lowerNibble = WX / 8;
							}

							upperNibble = upperNibble << 5;

							tilemapAddr += upperNibble;
							tilemapAddr += lowerNibble;

							if (tilemapAddr == prevAddr)
								tilemapAddr++;

							tileID = mmu.read(tilemapAddr);
							signedTileID = mmu.read(tilemapAddr);

							tileID = tileID << 4;

							prevAddr = tilemapAddr;

							bgFetcherState++;
							break;
						case 1:
							if (bitwise::check_bit(LCDC, 4))
								tileFetchAddr = 0x8000 + tileID;
							else
								tileFetchAddr = 0x9000 + (signedTileID * 16);

							//tileFetchAddr |= tileID;

							lowerNibble = !windowMode ? 2 * ((LY + SCY) % 8) : 2 * (WY % 8);
							tileFetchAddr |= lowerNibble;

							//TODO: Horizontal and vertical flip tile data here

							tileDataLow = mmu.read(tileFetchAddr);

							bgFetcherState++;
							break;
						case 2:
							tileDataHigh = mmu.read(tileFetchAddr + 1);

							if (spriteFetchWaiting)
							{
								spriteFetch = true;
								spriteFetchWaiting = false;
							}

							bgFetcherState++;
							break;
						case 3:
							if (backgroundFIFO.empty())
							{
								for (int i = 7; i >= 0; i--)
								{
									if (!bitwise::check_bit(tileDataLow, i) && !bitwise::check_bit(tileDataHigh, i))
										backgroundFIFO.push(0x00);
									else if (!bitwise::check_bit(tileDataLow, i) && bitwise::check_bit(tileDataHigh, i))
										backgroundFIFO.push(0x02);
									else if (bitwise::check_bit(tileDataLow, i) && !bitwise::check_bit(tileDataHigh, i))
										backgroundFIFO.push(0x01);
									else
										backgroundFIFO.push(0x03);
								}

								bgFetcherState = 0;
							}
							break;
					};
				}
				
				if (!backgroundFIFO.empty() && !spriteFetchWaiting && !spriteFetch && !pixelPushed)
				{
					//sf::Color backgroundPixel = backgroundFIFO.front();
					unsigned char backgroundPixel = backgroundFIFO.front();
					backgroundFIFO.pop();

					totalDiscarded++;

					if (totalDiscarded == discardCount)
						doneDiscarding = true;

					if (doneDiscarding)
					{
						if (spriteFIFO.empty())
							buffer.set_pixel(LX, LY, getColorFromPalette(backgroundPixel, 2));
						else
						{
							unsigned char spritePixel = spriteFIFO.front();
							spriteFIFO.pop();

							bool priorityBit = bitwise::check_bit(savedSprites[spriteIndex].attributeFlags, 7);
							
							if (!bgEnable)
								buffer.set_pixel(LX, LY, getColorFromPalette(spritePixel, bitwise::check_bit(savedSprites[spriteIndex].attributeFlags, 4)));
							else if (!objEnable || spritePixel == 0x00 || (priorityBit && (backgroundPixel != 0x00)))
								buffer.set_pixel(LX, LY, getColorFromPalette(backgroundPixel, 2));
							else
								buffer.set_pixel(LX, LY, getColorFromPalette(spritePixel, bitwise::check_bit(savedSprites[spriteIndex].attributeFlags, 4)));
						}

						LX++;
						pixelPushed = true;

						if (bitwise::check_bit(LCDC, 5) && LYWY && (LX >= WX - 7))
						{
							windowMode = true;
							LX = 0;
							std::queue<unsigned char>().swap(backgroundFIFO);
							bgFetcherState = 0;
						}

						else
							windowMode = false;
					}
				}

				cycleCount++;

				if (LX == 160)
				{
					std::queue<unsigned char>().swap(backgroundFIFO);
					std::queue<unsigned char>().swap(spriteFIFO);
					spriteBuffer.clear();
					savedSprites.clear();
					LX = 0;
					bgFetcherState = 0;
					spriteFetcherState = 0;
					doneDiscarding = false;
					newScanline = true;
					spriteFetch = false;
					spriteFetchWaiting = false;

					cycleCount = cycleCount % CLOCKS_PER_SCANLINE_VRAM;
					PPU_Mode = 2;
					
					bool LYEqualLYCEnable = bitwise::check_bit(*(mmu.STAT), 6);
					
					if (LYEqualLYC)
						mmu.write(0xFF41, mmu.read(0xFF0F) | 0x02);
					if (LYEqualLYC && LYEqualLYCEnable)
						requestSTAT = true;
						//mmu.write(0xFF0F, mmu.read(0xFF0F) | 0x02);
					
					mmu.write(0xFF41, mmu.read(0xFF41) & 0xFC);

					if (bitwise::check_bit(*(mmu.STAT), 3))
						requestSTAT = true;
						//mmu.write(0xFF0F, mmu.read(0xFF0F) | 0x02);
				}

				pixelPushed = false;
				cyclesRemaining--;
				break;

			case 2:		//Real mode 0, H-Blank (87 - 294 dots)
				cycleCount++;

				if (cycleCount >= CLOCKS_PER_HBLANK)
				{
					line++;
					mmu.updateLY(line);

					cycleCount = cycleCount % CLOCKS_PER_HBLANK;

					if (line == 144)
					{
						PPU_Mode = 3;
						mmu.write(0xFF41, mmu.read(0xFF41) & 0xFC);
						mmu.write(0xFF41, mmu.read(0xFF41) | 0x01);

						mmu.write(0xFF0F, mmu.read(0xFF0F) | 0x01);

						if (bitwise::check_bit(*(mmu.STAT), 4))
							requestSTAT = true;
							//mmu.write(0xFF0F, mmu.read(0xFF0F) | 0x02);
					}

					else
					{
						PPU_Mode = 0;
						mmu.write(0xFF41, mmu.read(0xFF41) & 0xFC);
						mmu.write(0xFF41, mmu.read(0xFF41) | 0x02);

						if (bitwise::check_bit(*(mmu.STAT), 5))
							requestSTAT = true;
							//mmu.write(0xFF0F, mmu.read(0xFF0F) | 0x02);
					}
				}

				cyclesRemaining--;
				break;

			case 3:		//Real mode 1, V-Blank (4560 dots total, 10 lines with 456 per line)
				cycleCount++;

				if (cycleCount >= CLOCKS_PER_SCANLINE)
				{
					line++;
					cycleCount = cycleCount % CLOCKS_PER_SCANLINE;

					if (line == 154)
					{
						if (cpu.getRefreshClocksElapsed() >= 70224)
						{
							draw(buffer);		
							cpu.setRefreshClocksElapsed(cpu.getRefreshClocksElapsed() - 70224);
						}

						buffer.reset();
						line = 0;
						mmu.write(0xFF44, 0x00);
						PPU_Mode = 0;
						mmu.write(0xFF41, mmu.read(0xFF41) & 0xFE);
						mmu.write(0xFF41, mmu.read(0xFF41) | 0x02);

						if (bitwise::check_bit(*(mmu.STAT), 5))
							requestSTAT = true;

						LYWY = false;
					}
				}

				cyclesRemaining--;
				break;

			default:		//Invalid PPU mode
				break;
		}

		if (LY == WY)
			LYWY = true;

		if (prevSTATSources == 0x00 && requestSTAT)
			mmu.write(0xFF0F, mmu.read(0xFF0F) | 0x02);
	}
}