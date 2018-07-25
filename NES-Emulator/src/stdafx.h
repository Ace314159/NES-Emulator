#pragma once

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <stdio.h>
#include <vector>

using std::cout;
using std::endl;

typedef std::vector<std::array<uint8_t, 0x4000>> PRGBank;
typedef std::vector<std::array<uint8_t, 0x2000>> CHRBank;
