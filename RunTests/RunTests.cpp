#include "stdafx.h"

#include "ScreenTest.h"

#define RUN_TEST(testClass, testType, testCategory, testName) \
		try { std::make_unique<testClass>(testType, testCategory, testName)->passed(); SUCCEED(); } \
		catch(std::exception e) { FAIL() << e.what(); }

// CPU Tests
TEST(CPUTests, branchBasics) {
	RUN_TEST(ScreenTest, "CPU_Tests"s, "branch_timing_tests"s, "1.Branch_Basics"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "branch_timing_tests"s, "2.Backward_Branch"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "branch_timing_tests"s, "3.Forward_Branch"s);
}

TEST(CPUTests, cpuDummyReads) {
	RUN_TEST(ScreenTest, "CPU_Tests"s, "cpu_dummy_reads"s, "cpu_dummy_reads"s);
}

TEST(CPUTests, cpuDummyWrites) {
	RUN_TEST(ScreenTest, "CPU_Tests"s, "cpu_dummy_writes"s, "cpu_dummy_writes_oam"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "cpu_dummy_writes"s, "cpu_dummy_writes_ppumem"s);
}

TEST(CPUTests, cpuExecSpace) {
	RUN_TEST(ScreenTest, "CPU_Tests"s, "cpu_exec_space"s, "test_cpu_exec_space_apu"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "cpu_exec_space"s, "test_cpu_exec_space_ppuio"s);
}

TEST(CPUTests, cpuInterrupts) {
	RUN_TEST(ScreenTest, "CPU_Tests"s, "cpu_interrupts_v2"s, "1-cli_latency"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "cpu_interrupts_v2"s, "2-nmi_and_brk"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "cpu_interrupts_v2"s, "3-nmi_and_irq"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "cpu_interrupts_v2"s, "4-irq_and_dma"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "cpu_interrupts_v2"s, "5-branch_delays_irq"s);
}

TEST(CPUTests, cpuReset) {
	RUN_TEST(ScreenTest, "CPU_Tests"s, "cpu_reset"s, "ram_after_reset"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "cpu_reset"s, "registers"s);
}

TEST(CPUTests, cpuTimingTest) {
	RUN_TEST(ScreenTest, "CPU_Tests"s, "cpu_timing_test6"s, "cpu_timing_test"s);
}

TEST(CPUTests, instrMisc) {
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_misc"s, "01-abs_x_wrap"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_misc"s, "02-branch_wrap"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_misc"s, "03-dummy_reads"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_misc"s, "04-dummy_reads_apu"s);
}

TEST(CPUTests, instrTest) {
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_test-v5"s, "01-basics"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_test-v5"s, "02-implied"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_test-v5"s, "03-immediate"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_test-v5"s, "04-zero_page"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_test-v5"s, "05-zp_xy"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_test-v5"s, "06-absolute"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_test-v5"s, "07-abs_xy"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_test-v5"s, "08-ind_x"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_test-v5"s, "09-ind_y"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_test-v5"s, "10-branches"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_test-v5"s, "11-stack"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_test-v5"s, "12-jmp_jsr"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_test-v5"s, "13-rts"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_test-v5"s, "14-rti"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_test-v5"s, "15-brk"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_test-v5"s, "16-special"s);

}

TEST(CPUTests, instrTiming) {
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_timing"s, "1-instr_timing"s);
	RUN_TEST(ScreenTest, "CPU_Tests"s, "instr_timing"s, "2-branch_timing"s);
}