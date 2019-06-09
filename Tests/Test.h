#pragma once

#include "NESTest.h"

class Test {
protected:
	NESTest nes;
public:
	explicit Test(const std::string& testFileName);

	void makeTest();
	virtual bool passed() = 0;
};

