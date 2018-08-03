#pragma once

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <SDL/SDL.h>
#include <stdio.h>
#include <string>
#include <vector>

using std::cout;
using std::endl;

constexpr double NTSC_FREQ(39375000 / 11 * 6 / 12); // NTSC CPU Clock Frequency
constexpr double PAL_FREQ(39375000 / 11 * 6 / 12); // PAL CPU Clock Frequency
constexpr double PAL_PERIOD(100.0 / 443361875 * 16 / 6); // PAL CPU Clock Period
constexpr double NTSC_PERIOD(11.0 / 39375000 * 12 / 6); // NTSC CPU Clock Period
