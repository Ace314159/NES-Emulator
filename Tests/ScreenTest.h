#pragma once

#include "Test.h"

class ScreenTest : public Test {
public:
	using Test::Test;
	bool passed() override;
};

