#pragma once

#include "NESTest.h"

class Test {
protected:
	NESTest nes;
public:
	explicit Test(const fs::path& testType, const fs::path& testCategory,
		const fs::path& testName);

	void makeTest();
	virtual bool passed() = 0;
};

