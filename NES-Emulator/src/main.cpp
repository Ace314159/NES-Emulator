#include "nes.h"

#include <iostream>

using std::cout;
using std::endl;


const std::chrono::nanoseconds NTSC_FREQ(1000000000/(39375000 / 11 * 6 / 12)); // NTSC Clock Frequency
const std::chrono::nanoseconds PAL_FREQ(1000000000/(443361875/100 * 6 / 16)); // PAL Clock Frequency



int main() {
	cout << std::hex;

	try {
		Graphics::init();
		NES nes = NES(NTSC_FREQ, "Super Mario Bros (PC10).nes");
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
