#include "GameBoy.h"
#include "Debugger.h"
#include <stdexcept>
#include <thread>
#include <fstream>

int windowScale = 5;
sf::RenderWindow window(sf::VideoMode(160 * windowScale, 144 * windowScale + 20), "");
sf::Clock deltaClock;

static sf::Image screen;
static sf::Texture screenTexture;
static sf::Sprite screenSprite;

sf::Image background;
sf::Texture backgroundTexture;
sf::Sprite backgroundSprite;

GameBoy* zappyBoy;

bool load_rom_flag = false;
bool quit_flag = false;

bool windowFocused = true;

std::vector<unsigned char> loadCartridgeData(std::string fileName)
{
	std::ifstream ROM_File;
	std::vector<unsigned char> cartridgeData;
	char currentVal;

	ROM_File.open(fileName, std::ios::binary);

	if (!ROM_File.is_open())
	{
		std::cout << "Could not open ROM: " << fileName << std::endl;
	}

	else
	{
		while (ROM_File.get(currentVal))
		{
			cartridgeData.push_back((unsigned char)currentVal);
		}
	}

	ROM_File.close();

	return cartridgeData;
}

//static void setPixels(std::vector<sf::Color> &pixels)
static void setPixels(const Buffer &buffer)
{
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			sf::Color color = buffer.get_pixel(x, y);
			screen.setPixel(x, y, color);
		}
	}
}

void eventHandler()
{
	sf::Event event;

	/*
	if (sf::Joystick::isButtonPressed(0, 0))
		std::cout << "0 pressed" << std::endl;
	if (sf::Joystick::isButtonPressed(0, 1))
		std::cout << "1 pressed" << std::endl;
	if (sf::Joystick::isButtonPressed(0, 2))
		std::cout << "2 pressed" << std::endl;
	if (sf::Joystick::isButtonPressed(0, 3))
		std::cout << "3 pressed" << std::endl;
	if (sf::Joystick::isButtonPressed(0, 4))
		std::cout << "4 pressed" << std::endl;
	if (sf::Joystick::isButtonPressed(0, 5))
		std::cout << "5 pressed" << std::endl;
	if (sf::Joystick::isButtonPressed(0, 6))
		std::cout << "6 pressed" << std::endl;
	if (sf::Joystick::isButtonPressed(0, 7))
		std::cout << "7 pressed" << std::endl;
	if (sf::Joystick::isButtonPressed(0, 8))
		std::cout << "8 pressed" << std::endl;
	if (sf::Joystick::isButtonPressed(0, 9))
		std::cout << "9 pressed" << std::endl;

	if (sf::Joystick::getAxisPosition(0, sf::Joystick::PovY) > 50)
		std::cout << "up pressed" << std::endl;
	if (sf::Joystick::getAxisPosition(0, sf::Joystick::PovY) < -50)
		std::cout << "down pressed" << std::endl;
	if (sf::Joystick::getAxisPosition(0, sf::Joystick::PovX) < -50)
		std::cout << "left pressed" << std::endl;
	if (sf::Joystick::getAxisPosition(0, sf::Joystick::PovX) > 50)
		std::cout << "right pressed" << std::endl;
	*/

	if (windowFocused)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Joystick::getAxisPosition(0, sf::Joystick::PovY) > 50)
			zappyBoy->pressButton(0);
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Joystick::getAxisPosition(0, sf::Joystick::PovY) < -50)
			zappyBoy->pressButton(1);
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Joystick::getAxisPosition(0, sf::Joystick::PovX) < -50)
			zappyBoy->pressButton(2);
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Joystick::getAxisPosition(0, sf::Joystick::PovX) > 50)
			zappyBoy->pressButton(3);
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Joystick::isButtonPressed(0, 1))
			zappyBoy->pressButton(4);
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Joystick::isButtonPressed(0, 0))
			zappyBoy->pressButton(5);
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter) || sf::Joystick::isButtonPressed(0, 7))
			zappyBoy->pressButton(6);
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Joystick::isButtonPressed(0, 6))
			zappyBoy->pressButton(7);
	}

	if (window.pollEvent(event))
	{
		ImGui::SFML::ProcessEvent(event);

		if (event.type == sf::Event::Closed)
		{
			window.close();
		}

		if (event.type == sf::Event::LostFocus)
			windowFocused = false;

		if (event.type == sf::Event::GainedFocus)
			windowFocused = true;
	}
}

//static void draw(std::vector<sf::Color> &pixels)
static void draw(const Buffer &buffer)
{
	eventHandler();

	setPixels(buffer);
	
	screenTexture.loadFromImage(screen);
	screenSprite.setTexture(screenTexture);

	ImGui::SFML::Update(window, deltaClock.restart());

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Load ROM", NULL, &load_rom_flag);
			ImGui::MenuItem("Exit", NULL, &quit_flag);
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	window.clear();

	window.draw(screenSprite);
	ImGui::SFML::Render(window);

	window.display();
}

static bool checkWindow()
{
	return window.isOpen();
}

//int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
int main()
{
	/*
		Resources:
	 
		JS APU library: https://github.com/shamblesides/apu
		https://gbdev.io/
		gbdev emulator dev resources: https://gbdev.io/resources.html#emula...
		Pan Docs: https://gbdev.io/pandocs/
		https://gekkio.fi/
		GameBoy Complete Technical Reference: https://gekkio.fi/files/gb-docs/gbctr...
		RGBDS assembly reference: https://rgbds.gbdev.io/docs/v0.9.0/gb...
		Interactive Op code reference: https://meganesu.github.io/generate-g...
	*/

	char windowTitle[255] = "Zappy Boy";
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("01-special.gb");								//PASS
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("02-interrupts.gb");								//FAIL
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("03-op_sp_hl.gb");								//PASS
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("04-op_r_imm.gb");								//PASS
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("05-op_rp.gb");									//PASS	TODO: PPU default color???
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("06-ld_r_r.gb");									//PASS
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("07-jr_jp_call_ret_rst.gb");						//PASS
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("08-misc_instrs.gb");							//PASS
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("09-op_r_r.gb");									//PASS
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("10-bit_ops.gb");								//PASS
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("11-op_a_(hl).gb");								//PASS
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("cpu_instrs.gb");								//PASS

	//std::vector<unsigned char> cartridgeData = loadCartridgeData("halt_bug.gb");									//FAIL
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("interrupt_time/interrupt_time.gb");				//FAIL
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("instr_timing/instr_timing.gb");					//FAIL #2

	//std::vector<unsigned char> cartridgeData = loadCartridgeData("mem_timing/individual/01-read_timing.gb");		//FAIL
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("mem_timing/individual/02-write_timing.gb");		//FAIL
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("mem_timing/individual/03-modify_timing.gb");	//FAIL
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("mem_timing/mem_timing.gb");						//FAIL

	//std::vector<unsigned char> cartridgeData = loadCartridgeData("acceptance/bits/mem_oam.gb");					//PASS
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("acceptance/bits/reg_f.gb");						//PASS
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("acceptance/bits/unused_hwio-GS.gb");			//FAIL

	//std::vector<unsigned char> cartridgeData = loadCartridgeData("acceptance/interrupts/ie_push.gb");				//FAIL	CURRENT

	//std::vector<unsigned char> cartridgeData = loadCartridgeData("acceptance/oam_dma/basic.gb");					//PASS (weird graphical artifacts due to no read/write restrictions based on PPU mode)
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("acceptance/oam_dma/reg_read.gb");				//PASS

	//std::vector<unsigned char> cartridgeData = loadCartridgeData("acceptance/ppu/hblank_ly_scx_timing-GS.gb");	//FAIL

	std::vector<unsigned char> cartridgeData = loadCartridgeData("acceptance/timer/tim00.gb");					//PASS
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("acceptance/timer/tima_reload.gb");				//FAIL
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("acceptance/timer/div_write.gb");				//FAIL

	//std::vector<unsigned char> cartridgeData = loadCartridgeData("misc/ppu/vblank_stat_intr-C.gb");				//FAIL

	//std::vector<unsigned char> cartridgeData = loadCartridgeData("boot.bin");			

	//std::vector<unsigned char> cartridgeData = loadCartridgeData("Dr._Mario.gb");									//Title screen, flashes a few times and then hangs
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("Tetris.gb");									//99% functional, need to investigate breaking when "2PLAYER" is selected from menu screen
	
	//MBC carts
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("Super_Mario_Land.gb");							//Title screen, demo plays but draws incorrectly after a bit, doesn't cycle through to next levels on subsequent demo plays
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("Donkey_Kong.gb");								//Gray screen
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("Pokemon_Blue.gb");								//Gray screen
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("Kirby's Dream Land (USA, Europe).gb");			//Unrecognized opcode
	//std::vector<unsigned char> cartridgeData = loadCartridgeData("Legend of Zelda, The - Link's Awakening (USA, Europe) (Rev 2).gb");	//Gray screen

	int joystickIndex;
	sf::Image icon;
	icon.loadFromFile("zb_icon_3.png");
	ImGui::SFML::Init(window);
	window.resetGLStates();

	window.setTitle(windowTitle);
	window.setIcon(60, 60, icon.getPixelsPtr());
	window.setFramerateLimit(60);
	window.setVerticalSyncEnabled(true);
	//window.setKeyRepeatEnabled(false);

	screen.create(160, 144, sf::Color::White);
	screenTexture.loadFromImage(screen);
	screenSprite.setTexture(screenTexture);
	screenSprite.setPosition(0, 20);
	screenSprite.scale(sf::Vector2f(5, 5));

	background.create(256, 256, sf::Color::White);
	backgroundTexture.loadFromImage(background);
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setPosition(200, 20);

	//zappyBoy.loadGame("Tetris.gb");
	//zappyBoy.loadGame("boot_edit.bin");
	//zappyBoy.loadGame("special.gb");

	//sf::Clock deltaClock;

	window.setPosition(sf::Vector2i(100, 200));

	//window.display();

	zappyBoy = new GameBoy(cartridgeData);

	//zappyBoy->loadGame("boot.bin");

	//std::thread t1(eventHandler);

	for (int i = 0; i < 8; i++)
	{
		if (sf::Joystick::isConnected(i))
			joystickIndex = i;
	}

	zappyBoy->powerOn(&checkWindow, &draw);

	//t1.join();
	/*
	while (window.isOpen()) 
	{
		sf::Event event;

		if (window.pollEvent(event)) 
		{
			//ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed) 
			{
				window.close();
			}
		}

		//ImGui::SFML::Update(window, deltaClock.restart());
		window.clear();

		//zappyBoy->tick();

		//setPixels();
		zappyBoy->screenTexture.loadFromImage(zappyBoy->screen);
		zappyBoy->draw(&window, &draw);

		//ImGui::SFML::Render(window);
		window.display();
	}
	*/

	//delete zappyBoy;
	ImGui::SFML::Shutdown();

	/*
	std::ofstream outFile;

	outFile.open("cycles.txt");

	for (int i = 0; i < zappyBoy->cpu.debugStrings.size(); i++)
	{
		outFile << zappyBoy->cpu.debugStrings[i];
	}

	outFile.close();
	*/

	return 0;
}