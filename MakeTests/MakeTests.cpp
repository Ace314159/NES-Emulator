#include "stdafx.h"

#include "ScreenTest.h"

int main(int argc, char* argv[]) {
	try {
		std::make_unique<ScreenTest>
			("CPU_Tests"s, "cpu_dummy_writes"s, "cpu_dummy_writes_oam"s)->makeTest();
	} catch(const TestFailException& e) {
		fs::remove_all(e.framesFolder);
		fs::create_directory(e.framesFolder);
		std::ofstream file(e.framesFolder / "FAIL");
		file << e.error;
		std::cout << e.error;
		return -1;
	}
	return 0;
}
