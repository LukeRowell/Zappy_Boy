#pragma once
#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <SFML/Graphics.hpp>

class Buffer
{
	public:

		Buffer(unsigned int width, unsigned int height);
		~Buffer();

		void set_pixel(unsigned int x, unsigned int y, sf::Color color);
		sf::Color get_pixel(unsigned int x, unsigned int y) const;

		void reset();

	private:

		unsigned int width;
		unsigned int height;

		unsigned int pixel_index(unsigned int x, unsigned int y) const;

		std::vector<sf::Color> pixels;
};

#endif