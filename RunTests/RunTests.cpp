#include "stdafx.h"

#include "ScreenTest.h"

#define RUN_TEST(testClass, testType, testCategory, testName) \
		try { std::make_unique<testClass>(testType, testCategory, testName)->passed(); SUCCEED(); } \
		catch(std::exception e) { FAIL() << e.what(); }

// CPU Tests
TEST(CPU_Tests, branchBasics) {
	RUN_TEST(ScreenTest, "CPU_Tests", "branch_timing_tests", "1.Branch_Basics");
	RUN_TEST(ScreenTest, "CPU_Tests", "branch_timing_tests", "2.Backward_Branch");
	RUN_TEST(ScreenTest, "CPU_Tests", "branch_timing_tests", "3.Forward_Branch");
}

TEST(CPU_Tests, cpuDummyReads) {
	RUN_TEST(ScreenTest, "CPU_Tests", "cpu_dummy_reads", "cpu_dummy_reads");
}

TEST(CPU_Tests, cpuDummyWrites) {
	RUN_TEST(ScreenTest, "CPU_Tests", "cpu_dummy_writes", "cpu_dummy_writes_oam");
	RUN_TEST(ScreenTest, "CPU_Tests", "cpu_dummy_writes", "cpu_dummy_writes_ppumem");
}

TEST(CPU_Tests, cpuExecSpace) {
	RUN_TEST(ScreenTest, "CPU_Tests", "cpu_exec_space", "test_cpu_exec_space_apu");
	RUN_TEST(ScreenTest, "CPU_Tests", "cpu_exec_space", "test_cpu_exec_space_ppuio");
}

TEST(CPU_Tests, cpuInterrupts) {
	RUN_TEST(ScreenTest, "CPU_Tests", "cpu_interrupts_v2", "1-cli_latency");
	RUN_TEST(ScreenTest, "CPU_Tests", "cpu_interrupts_v2", "2-nmi_and_brk");
	RUN_TEST(ScreenTest, "CPU_Tests", "cpu_interrupts_v2", "3-nmi_and_irq");
	RUN_TEST(ScreenTest, "CPU_Tests", "cpu_interrupts_v2", "4-irq_and_dma");
	RUN_TEST(ScreenTest, "CPU_Tests", "cpu_interrupts_v2", "5-branch_delays_irq");
}

TEST(CPU_Tests, cpuReset) {
	RUN_TEST(ScreenTest, "CPU_Tests", "cpu_reset", "ram_after_reset");
	RUN_TEST(ScreenTest, "CPU_Tests", "cpu_reset", "registers");
}

TEST(CPU_Tests, cpuTimingTest) {
	RUN_TEST(ScreenTest, "CPU_Tests", "cpu_timing_test6", "cpu_timing_test");
}

TEST(CPU_Tests, instrMisc) {
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_misc", "01-abs_x_wrap");
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_misc", "02-branch_wrap");
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_misc", "03-dummy_reads");
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_misc", "04-dummy_reads_apu");
}

TEST(CPU_Tests, instrTest) {
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_test-v5", "01-basics");
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_test-v5", "02-implied");
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_test-v5", "03-immediate");
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_test-v5", "04-zero_page");
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_test-v5", "05-zp_xy");
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_test-v5", "06-absolute");
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_test-v5", "07-abs_xy");
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_test-v5", "08-ind_x");
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_test-v5", "09-ind_y");
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_test-v5", "10-branches");
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_test-v5", "11-stack");
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_test-v5", "12-jmp_jsr");
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_test-v5", "13-rts");
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_test-v5", "14-rti");
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_test-v5", "15-brk");
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_test-v5", "16-special");

}

TEST(CPU_Tests, instrTiming) {
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_timing", "1-instr_timing");
	RUN_TEST(ScreenTest, "CPU_Tests", "instr_timing", "2-branch_timing");
}

// PPU Tests
TEST(PPU_Tests, blarggTests) {
	RUN_TEST(ScreenTest, "PPU_Tests", "blargg_ppu_tests", "palette_ram");
	RUN_TEST(ScreenTest, "PPU_Tests", "blargg_ppu_tests", "sprite_ram");
	RUN_TEST(ScreenTest, "PPU_Tests", "blargg_ppu_tests", "vbl_clear_time");
	RUN_TEST(ScreenTest, "PPU_Tests", "blargg_ppu_tests", "vram_access");
}

TEST(PPU_Tests, fullPalette) {
	RUN_TEST(ScreenTest, "PPU_Tests", "full_palette", "full_palette");
	RUN_TEST(ScreenTest, "PPU_Tests", "full_palette", "full_palette_alt");
}

TEST(PPU_Tests, NMISync) {
	RUN_TEST(ScreenTest, "PPU_Tests", "nmi_sync", "demo_ntsc");
	RUN_TEST(ScreenTest, "PPU_Tests", "nmi_sync", "demo_pal");
}

TEST(PPU_Tests, NTSCTorture) {
	RUN_TEST(ScreenTest, "PPU_Tests", "ntsc_torture", "ntsc_torture");
}

TEST(PPU_Tests, OAMRead) {
	RUN_TEST(ScreenTest, "PPU_Tests", "oam_read", "oam_read");
}

TEST(PPU_Tests, OAMStress) {
	RUN_TEST(ScreenTest, "PPU_Tests", "oam_stress", "oam_stress");
}

TEST(PPU_Tests, openBus) {
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_open_bus", "ppu_open_bus");
}

TEST(PPU_Tests, readBuffer) {
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_read_buffer", "test_ppu_read_buffer");
}

TEST(PPU_Tests, spriteHit) {
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_sprite_hit", "01-basics");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_sprite_hit", "02-alignment");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_sprite_hit", "03-corners");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_sprite_hit", "04-flip");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_sprite_hit", "05-left_clip");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_sprite_hit", "06-right_edge");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_sprite_hit", "07-screen_bottom");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_sprite_hit", "08-double_height");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_sprite_hit", "09.timing_basics");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_sprite_hit", "10-timing_order");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_sprite_hit", "11.edge_timing");
}

TEST(PPU_Tests, spriteOverflow) {
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_sprite_overflow", "01-basics");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_sprite_overflow", "02-details");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_sprite_overflow", "03-timing");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_sprite_overflow", "04-obscure");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_sprite_overflow", "05-emulator");
}

TEST(PPU_Tests, VBLNMI) {
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_vbl_nmi", "01-vbl_basics");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_vbl_nmi", "02-vbl_set_time");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_vbl_nmi", "03-vbl_clear_time");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_vbl_nmi", "04-nmi_control");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_vbl_nmi", "05-nmi_timing");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_vbl_nmi", "06-suppression");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_vbl_nmi", "07-nmi_on_timing");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_vbl_nmi", "08-nmi_off_timing");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_vbl_nmi", "09-even_odd_frames");
	RUN_TEST(ScreenTest, "PPU_Tests", "ppu_vbl_nmi", "10-even_odd_timing");
}

TEST(PPU_Tests, scanline) {
	RUN_TEST(ScreenTest, "PPU_Tests", "scanline", "scanline");
}

TEST(PPU_Tests, sprdmaAndDMCDMA) {
	RUN_TEST(ScreenTest, "PPU_Tests", "sprdma_and_dmc_dma", "sprdma_and_dmc_dma");
	RUN_TEST(ScreenTest, "PPU_Tests", "sprdma_and_dmc_dma", "sprdma_and_dmc_dma_512");
}
