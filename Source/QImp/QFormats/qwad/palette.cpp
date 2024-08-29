#include <qwad/palette.h>

#include <fstream>
#include <iterator>
#include <algorithm>

namespace qformats::wad
{
	color Palette::GetColor(int index)
	{
		if (index < 0 || index >= colors.size())
		{
			throw std::runtime_error("color index out of range");
		}

		return colors[index];
	}

}