#pragma once

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <initializer_list>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <memory>
#include <string>
#include <thread>
#include <vector>

using std::cout;
using std::endl;
using namespace std::string_literals;

namespace fs = std::filesystem;

constexpr double NTSC_FREQ(39375000.0 / 11 * 6 / 12); // NTSC CPU Clock Frequency
constexpr double PAL_FREQ(39375000.0 / 11 * 6 / 12); // PAL CPU Clock Frequency
constexpr double PAL_PERIOD(100.0 / 443361875 * 16 / 6); // PAL CPU Clock Period
constexpr double NTSC_PERIOD(11.0 / 39375000 * 12 / 6); // NTSC CPU Clock Period
constexpr auto NTSC_FRAME_PERIOD(
	std::chrono::duration<double, std::ratio<1, 1>>(29780.5 / NTSC_FREQ)); // NTSC FRAME PERIOD
constexpr double PAL_FRAME_PERIOD(29780.5 / PAL_FREQ); // NTSC FRAME PERIOD
