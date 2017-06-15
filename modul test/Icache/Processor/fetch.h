#include "systemc.h"
#include "directive.h"

struct fetch : sc_module { 
  sc_in<unsigned > 		ramdata;	// instruction from RAM    
  sc_in<unsigned > 		branch_address; // branch target address   
  sc_in<bool>  			next_pc;   	// pc ++
  sc_in<bool>  			branch_valid;  	// branch_valid
  sc_in<bool>  			stall_fetch;   	// STALL_FETCH
  sc_in<bool>  			interrupt;   	// interrrupt 
  sc_in<unsigned>  		int_vectno;   	// interrupt vector number
  sc_in<bool>  			icache_valid;  	// Icache input valid
  sc_out<bool>  		ram_cs;   	// RAM chip select
  sc_out<bool>  		ram_we; 	// RAM write enable for SMC
  sc_out<unsigned >  	address;    	// address send to RAM
  sc_out<unsigned>  	instruction; 	 // instruction send to ID
  sc_out<bool>  		instruction_valid; // inst valid
  sc_out<bool>  		interrupt_ack;   // interrupt acknowledge
  sc_out<bool>  		busy;   // interrupt acknowledge
  sc_in_clk             CLK;

  // Parameter
  int memory_latency; 

  void init_param(int given_delay_cycles) {
    memory_latency = given_delay_cycles; 
  }
 
  //Constructor 
  SC_CTOR(fetch) {
        SC_CTHREAD(entry, CLK.pos());
  }

  // Process functionality in member function below
  void entry();
};

void fetch::entry()
{
   unsigned addr_tmp=0;
   unsigned datao_tmp=0;
   unsigned datai_tmp=0;

   addr_tmp = 4;
   datao_tmp = 0xdeadbeef;

  while (true) {
	if (stall_fetch.read() == false) {
	busy=true;
	if (interrupt.read() == true) {
		ram_cs.write(true);
		addr_tmp = int_vectno.read();
		address.write(addr_tmp);
    	ram_we.write(false);
    	wait(memory_latency); 
		do { wait(); } while ( !(icache_valid == true) );
    	datai_tmp = ramdata.read();
		if (PRINT_IFU) {
			printf("IF ALERT: **INTERRUPT**\n");
   			cout.setf(ios::hex,ios::basefield);
   			cout << "------------------------" << endl;   
   			cout << "IFU :" << " mem=0x" << datai_tmp << endl;
   			cout << "IFU : pc= " << addr_tmp ;   
   			cout.setf(ios::dec,ios::basefield);
   			cout << " at CSIM " << sc_time_stamp() << endl;
   			cout << "------------------------" << endl;   
		}
		instruction_valid.write(true);
		instruction.write(datai_tmp);
    	ram_cs.write(false);
		interrupt_ack.write(true);
		if (next_pc.read() == true) { addr_tmp++; }
		wait();
        instruction_valid.write(false);
		interrupt_ack.write(false);
		wait();

	}

	if (branch_valid.read() == true) {
    	ram_cs.write(true);
		addr_tmp = branch_address.read();
		address.write(addr_tmp);
    	ram_we.write(false);
    	wait(memory_latency); 
		do { wait(); } while ( !(icache_valid == true) );
    		datai_tmp = ramdata.read();
		if (PRINT_IFU) {
			printf("IFU ALERT: **BRANCH**\n");
   			cout.setf(ios::hex,ios::basefield);
   			cout << "------------------------" << endl;   
   			cout << "IFU :" << " mem=0x" << datai_tmp << endl;
   			cout << "IFU : pc= " << addr_tmp ;   
   			cout.setf(ios::dec,ios::basefield);
   			cout << " at CSIM " << sc_time_stamp() << endl;
   			cout << "------------------------" << endl; 
		}

		instruction_valid.write(true);
		instruction.write(datai_tmp);
    	ram_cs.write(false);
		if (next_pc.read() == true) { addr_tmp++; }
		wait();
                instruction_valid.write(false);
		wait();
	} else {
    	ram_cs.write(true);
    	address.write(addr_tmp);
    	ram_we.write(false);
    	wait(memory_latency); // For data to appear
		do { wait(); } while ( !(icache_valid == true) );
    	datai_tmp = ramdata.read();

		if (PRINT_IFU) {
   			cout.setf(ios::hex,ios::basefield);
   			cout << "------------------------" << endl;   
   			cout << "IFU :" << " mem=0x" << datai_tmp << endl;
   			cout << "IFU : pc= " << addr_tmp ;   
   			cout.setf(ios::dec,ios::basefield);
   			cout << " at CSIM " << sc_time_stamp() << endl;
   			cout << "------------------------" << endl;   
		}

        instruction_valid.write(true);
        instruction.write(datai_tmp);
    	ram_cs.write(false);
		if (next_pc.read() == true) { addr_tmp++; }
            wait();
            instruction_valid.write(false);
    		wait();
        }
	wait();
  }
	else
	{
		cout << "------------------------" << endl;
		cout << "IFU: Pause Fetch" << endl;
		cout.setf(ios::dec,ios::basefield);
   		cout << " at CSIM " << sc_time_stamp() << endl;
		cout << "------------------------" << endl;
		busy=false;
		do{ wait(); } while ( !(stall_fetch.read() == false) );
	}
  }
} // end of entry function