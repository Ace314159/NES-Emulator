#include "stdafx.h"

#include "NES.h"

#include <windows.h>

int main(int argc, char* argv[]) {
	// cout << std::hex;

	try {
		auto nes = std::make_unique<NES>("ppu_vbl_nmi - Passed/01-vbl_basics.nes");

		HRSRC paletteResource = ::FindResource(nullptr, MAKEINTRESOURCE(IDR_NTSCPALETTE), L"PALETTE");
		HGLOBAL paletteResourceData = ::LoadResource(nullptr, paletteResource);
		Color* paletteData = static_cast<Color*>(::LockResource(paletteResourceData));
		nes->ppu.setPaletteTable(paletteData);

		while(nes->isRunning()) {
			nes->tick();
		}
	} catch(const std::runtime_error& e) {
		cout << e.what() << endl;
		return -1;
	}

	return 0;
}