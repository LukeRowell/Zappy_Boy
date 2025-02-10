#include "buffer.h"

Buffer::Buffer(unsigned int myWidth, unsigned int myHeight) : width(myWidth), height(myHeight), pixels(myWidth * myHeight, sf::Color::White)
{

}

Buffer::~Buffer()
{

}

void Buffer::set_pixel(unsigned int x, unsigned int y, sf::Color color)
{
	pixels[pixel_index(x, y)] = color;
}

sf::Color Buffer::get_pixel(unsigned int x, unsigned int y) const
{
	return pixels.at(pixel_index(x, y));
}

inline unsigned int Buffer::pixel_index(unsigned int x, unsigned int y) const
{
	return (y * width) + x;
}

void Buffer::reset()
{
	for (int i = 0; i < width * height; i++)
	{
		pixels[i] = sf::Color::White;
	}
}