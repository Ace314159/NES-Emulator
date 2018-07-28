#include "stdafx.h"
#include "nes.h"

const std::chrono::nanoseconds NTSC_FREQ(1000000000/(39375000 / 11 * 6 / 12)); // NTSC Clock Frequency
const std::chrono::nanoseconds PAL_FREQ(1000000000/(443361875/100 * 6 / 16)); // PAL Clock Frequency



int main() {
	cout << std::hex;

	try {
		Graphics::init();
		NES nes = NES(NTSC_FREQ, "Legend of Zelda, The (U) (PRG1) [!].nes");
		while(!glfwWindowShouldClose(Graphics::window)) {
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
