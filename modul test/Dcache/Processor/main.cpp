#include "directive.h"
#include "systemc.h"
#include "dcache.h"
#include <climits>
#include <cstdlib>
#include <time.h> 
#include <limits.h>

int sc_main(int ac, char *av[])
{
  sc_core::sc_report_handler::set_actions( "/IEEE_Std_1666/deprecated",
                                           sc_core::SC_DO_NOTHING );
  sc_trace_file *wf = sc_create_vcd_trace_file("Processor_waveform");

  const int delay_cycles = 2;

  sc_signal<signed>			dram_dataout("DRAM_DATAOUT") ;
  sc_signal<bool>			dram_rd_valid("DRAM_RD_VALID") ;
  sc_signal<bool>			mem_access("MEM_ACCESS") ;
  sc_signal<unsigned >		mem_address("MEM_ADDRESS") ;
  sc_signal<bool>			mem_write("MEM_WRITE") ;
  sc_signal<signed int>     dramdata("dramdata");
		sc_trace(wf, dram_dataout, "DCACHE_dram_dataout");
		sc_trace(wf, dram_rd_valid, "DCACHE_dram_rd_valid");
		sc_trace(wf, mem_access, "DCACHE_mem_access");
		sc_trace(wf, mem_address, "DCACHE_mem_address");
		sc_trace(wf, mem_write, "DCACHE_mem_write");
		sc_trace(wf, dramdata, "DCACHE_dramdata");

  ////////////////////////////////////////////////////////////////////////////
  // 				MAIN PROGRAM 
  ////////////////////////////////////////////////////////////////////////////
  sc_clock clk("Clock", 1, SC_NS, 0.5, 0.0, SC_NS);
  sc_trace(wf, clk, "CLK");

  dcache 	DCACHE("DCACHE_BLOCK");
		DCACHE.init_param(delay_cycles);
		DCACHE << dramdata << mem_access << mem_write << mem_address << dram_dataout 
		<< dram_rd_valid << clk;

  time_t tbuffer = time(NULL);
//6ns
	sc_start(6, SC_NS);
	mem_access=1;
	mem_write=0;
	mem_address=3;
	cout << "@" << sc_time_stamp() << " Read mem[0x3]\n" << endl;

    sc_start(2, SC_NS);
//8ns
	assert(dram_rd_valid.read() == true);
	assert(dram_dataout.read() == 3);
	cout << "@" << sc_time_stamp() << "mem[0x3]=0x3\n" << endl;
	mem_access=0;
	sc_start(2, SC_NS);
//10ns
	mem_access=1;
	mem_write=1;
	dramdata=0xff;
	mem_address=3;
	cout << "@" << sc_time_stamp() << " Write mem[0x3]=0xff\n" << endl;
	sc_start(2, SC_NS);
	mem_access=0;
//12ns
	sc_start(6, SC_NS);
	mem_access=1;
	mem_write=0;
	mem_address=3;
	cout << "@" << sc_time_stamp() << " Read mem[0x3]\n" << endl;
    sc_start(2, SC_NS);
//14ns
	assert(dram_rd_valid.read() == true);
	assert(dram_dataout.read() == 0xff);
	cout << "@" << sc_time_stamp() << "mem[0x3]=0xff\n" << endl;
	mem_access=0;
	sc_start(2, SC_NS);

  cout << "Time for simulation = " << (time(NULL) - tbuffer) << endl;
  sc_close_vcd_trace_file(wf);

  
  return 0;	
}
