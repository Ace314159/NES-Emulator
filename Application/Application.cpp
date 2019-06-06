#include "stdafx.h"

#include "NES.h"

#include <windows.h>

int main(int argc, char* argv[]) {
	cout << std::hex;

	try {
		NES nes("ROMs/donkey kong.nes");

		HRSRC paletteResource = ::FindResource(nullptr, MAKEINTRESOURCE(IDR_NTSCPALETTE), L"PALETTE");
		HGLOBAL paletteResourceData = ::LoadResource(nullptr, paletteResource);
		Color* paletteData = static_cast<Color*>(::LockResource(paletteResourceData));
		nes.ppu.setPaletteTable(paletteData);

		while(nes.isRunning()) {
			nes.tick();
		}
	} catch(std::runtime_error& e) {
		cout << e.what() << endl;
		return -1;
	}

	return 0;
}