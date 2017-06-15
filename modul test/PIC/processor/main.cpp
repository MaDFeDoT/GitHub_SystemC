#include "directive.h"
#include "systemc.h"
#include "pic.h"
#include <climits>
#include <cstdlib>
#include <time.h> 
#include <limits.h>

int sc_main(int ac, char *av[])
{
  sc_core::sc_report_handler::set_actions( "/IEEE_Std_1666/deprecated",
                                           sc_core::SC_DO_NOTHING );
  sc_trace_file *wf = sc_create_vcd_trace_file("Processor_waveform");
  // ************************ PIC *****************************************
  sc_signal<bool> 		ireq0("IREQ0") ;
  sc_signal<bool> 		ireq1("IREQ1") ;
  sc_signal<bool> 		ireq2("IREQ2") ;
  sc_signal<bool> 		ireq3("IREQ3") ;
  sc_signal<bool> 		intreq("INTREQ") ;
  sc_signal<unsigned> 	vectno("VECTNO") ;
  sc_signal<bool> 		intack_cpu("INTACK_CPU") ;
		sc_trace(wf, ireq0, "PIC_ireq0");
		sc_trace(wf, ireq1, "PIC_ireq1");
		sc_trace(wf, ireq2, "PIC_ireq2");
		sc_trace(wf, ireq3, "PIC_ireq3");
		sc_trace(wf, intreq, "PIC_intreq");
		sc_trace(wf, vectno, "PIC_vectno");
		sc_trace(wf, intack_cpu, "PIC_intack_cpu");
  

  ////////////////////////////////////////////////////////////////////////////
  // 				MAIN PROGRAM 
  ////////////////////////////////////////////////////////////////////////////
  sc_clock clk("Clock", 1, SC_NS, 0.5, 0.0, SC_NS);
  sc_trace(wf, clk, "CLK");
  
		

  pic		APIC("PIC_BLOCK");
		APIC << ireq0 << ireq1 << ireq2 << ireq3 <<intack_cpu
		<< intreq << vectno;
  time_t tbuffer = time(NULL);

//6ns
	sc_start(6, SC_NS);
	ireq0=1;
	cout << "@" << sc_time_stamp() << " Interrapt 0\n" << endl;

    sc_start(2, SC_NS);
	ireq0=0;
//8ns
	assert(intreq.read() == true);
	assert(vectno.read() == 0x0);
	cout << "@" << sc_time_stamp() << " Sucess interrapt 0\n" << endl;
	intack_cpu=1;
	sc_start(2, SC_NS);
//10ns
	assert(intreq.read() == false);
	intack_cpu=0;
//6ns
	sc_start(6, SC_NS);
	ireq1=1;
	cout << "@" << sc_time_stamp() << " Interrapt 1\n" << endl;

    sc_start(2, SC_NS);
	ireq1=0;
//8ns
	assert(intreq.read() == true);
	assert(vectno.read() == 0x1);
	cout << "@" << sc_time_stamp() << " Sucess interrapt 1\n" << endl;
	intack_cpu=1;
	sc_start(2, SC_NS);
//10ns
	assert(intreq.read() == false);
	intack_cpu=0;
//6ns
	sc_start(6, SC_NS);
	ireq2=1;
	cout << "@" << sc_time_stamp() << " Interrapt 2\n" << endl;

    sc_start(2, SC_NS);
	ireq2=0;
//8ns
	assert(intreq.read() == true);
	assert(vectno.read() == 0x2);
	cout << "@" << sc_time_stamp() << " Sucess interrapt 2\n" << endl;
	intack_cpu=1;
	sc_start(2, SC_NS);
//10ns
	assert(intreq.read() == false);
	
	intack_cpu=0;
//6ns
	sc_start(6, SC_NS);
	ireq3=1;
	cout << "@" << sc_time_stamp() << " Interrapt 3\n" << endl;

    sc_start(2, SC_NS);
	ireq3=0;
//8ns
	assert(intreq.read() == true);
	assert(vectno.read() == 0x3);
	cout << "@" << sc_time_stamp() << " Sucess interrapt 3\n" << endl;
	intack_cpu=1;
	sc_start(2, SC_NS);
//10ns
	assert(intreq.read() == false);

	sc_close_vcd_trace_file(wf);

  return 0;	
}
