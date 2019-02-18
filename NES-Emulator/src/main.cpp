#include "stdafx.h"
#include "nes.h"


int main(int argc, char* argv[]) {
	// cout << std::hex;

	try {
		NES nes = NES("Legend of Zelda, The (U) (PRG1) [!].nes");
		while(!glfwWindowShouldClose(nes.ppu.window.window)) {
			nes.tick();
		}
	} catch(std::runtime_error& e) {
		cout << e.what() << endl;
		glfwTerminate();
		return -1;
	}

	glfwTerminate();
	return 0;
}