#include "stdafx.h"

#include "ScreenTest.h"

#define MAKE_TEST(testClass, testType, testCategory, testName) \
		std::make_unique<testClass>(testType, testCategory, testName)->makeTest()

int main(int argc, char* argv[]) {
	try {

	} catch(const TestFailException& e) {
		fs::remove_all(e.framesFolder);
		fs::create_directory(e.framesFolder);
		std::ofstream file(e.framesFolder / "FAIL");
		file << e.error;
		std::cout << e.error;
	}
	return 0;
}
