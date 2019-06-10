#pragma once

#include "NESTest.h"

class TestFailException : public std::runtime_error {
public:
	TestFailException(const fs::path& framesDir, const std::string& error);

	fs::path framesFolder;
	std::string error;
};

class Test {
public:
	NESTest nes;
	explicit Test(const fs::path& testType, const fs::path& testCategory,
		const fs::path& testName);

	void makeTest();
	virtual void passed();
};

