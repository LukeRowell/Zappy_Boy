#include "Color.h"

GBColor get_color(unsigned char pixel_value)
{
	switch (pixel_value)
	{
		case 0:
			return GBColor::Color0;
			break;

		case 1:
			return GBColor::Color1;
			break;

		case 2:
			return GBColor::Color2;
			break;

		case 3:
			return GBColor::Color3;
			break;

		default:
			break;
	}
}