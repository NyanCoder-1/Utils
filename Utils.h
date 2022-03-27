#pragma once

#include <tuple>
#include <string>

namespace Util {
	std::tuple<unsigned char, unsigned char, unsigned char, unsigned char> textToColor(const std::string& colorText);
}