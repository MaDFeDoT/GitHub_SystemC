#include "directive.h"
#include "systemc.h"
#include "icache.h"
#include <climits>
#include <cstdlib>
#include <time.h> 
#include <limits.h>

int sc_main(int ac, char *av[])
{
  sc_core::sc_report_handler::set_actions( "/IEEE_Std_1666/deprecated",
                                           sc_core::SC_DO_NOTHING );
  sc_trace_file *wf = sc_create_vcd_trace_file("Processor_waveform");
  // ************************ ICACHE ***********************************
  sc_signal<bool>			ram_cs("RAM_CS") ;
  sc_signal<bool>			ram_we("RAM_WE") ;
  sc_signal<unsigned >		addr("Address") ;
  sc_signal<unsigned >		ram_datain("RAM_DATAIN") ;
  sc_signal<unsigned >		ram_dataout("RAM_DATAOUT") ;
  sc_signal<bool>			icache_valid("ICACHE_VALID") ;
  const int delay_cycles = 2;
		// Dump the desired signals
		sc_trace(wf, ram_cs, "ICACHE_ram_cs");
		sc_trace(wf, ram_we, "ICACHE_ram_we");
		sc_trace(wf, addr, "ICACHE_addr");
		sc_trace(wf, ram_datain, "ICACHE_ram_datain");
		sc_trace(wf, ram_dataout, "ICACHE_ram_dataout");
		sc_trace(wf, icache_valid, "ICACHE_icache_valid");

  ////////////////////////////////////////////////////////////////////////////
  // 				MAIN PROGRAM 
  ////////////////////////////////////////////////////////////////////////////
  sc_clock clk("Clock", 1, SC_NS, 0.5, 0.0, SC_NS);
  sc_trace(wf, clk, "CLK");
  

  icache 	ICACHE("ICACHE_BLOCK"); 
		ICACHE.init_param(delay_cycles);
		ICACHE << ram_datain << ram_cs << ram_we << addr << ram_dataout 
		<< icache_valid << clk;

  time_t tbuffer = time(NULL);
  //6ns
	sc_start(6, SC_NS);
	ram_cs=1;
	ram_we=0;
	addr=3;
	cout << "@" << sc_time_stamp() << " Read mem[0x3]\n" << endl;

    sc_start(2, SC_NS);
//8ns
	assert(icache_valid.read() == true);
	assert(ram_dataout.read() == 0xfffffff0);
	cout << "@" << sc_time_stamp() << "mem[0x3]=0xfffffff0\n" << endl;
	ram_cs=0;
	sc_start(2, SC_NS);
//10ns
	ram_cs=1;
	ram_we=1;
	ram_datain=0xff;
	addr=3;
	cout << "@" << sc_time_stamp() << " Write mem[0x3]=0xff\n" << endl;
	sc_start(2, SC_NS);
	ram_cs=0;
//12ns
	sc_start(6, SC_NS);
	ram_cs=1;
	ram_we=0;
	addr=3;
	cout << "@" << sc_time_stamp() << " Read mem[0x3]\n" << endl;
    sc_start(2, SC_NS);
//14ns
	assert(icache_valid.read() == true);
	assert(ram_dataout.read() == 0xff);
	cout << "@" << sc_time_stamp() << "mem[0x3]=0xff\n" << endl;
	ram_cs=0;
	sc_start(2, SC_NS);

  cout << "Time for simulation = " << (time(NULL) - tbuffer) << endl;
  sc_close_vcd_trace_file(wf);
  return 0;	
}
