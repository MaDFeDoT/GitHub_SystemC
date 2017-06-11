#include "systemc.h"
#include "directive.h"

struct dcache : sc_module { 
  	sc_in<signed> 		datain; 	// input data
  	sc_in<bool>  		cs;    		// chip select
  	sc_in<bool>  		we;    		// write enable 
  	sc_in<unsigned >  	addr;  		// address
  	sc_out<signed>  	dataout;  	// dataram data out
  	sc_out<bool>  		out_valid;    	// output valid
    sc_in_clk 			CLK;


  // Parameter
  unsigned *dmemory;	 			// data memory
  int wait_cycles; 					// cycles # it takes to access dmemory

  void init_param(int given_delay_cycles) {
    wait_cycles = given_delay_cycles; 
  }
 
  //Constructor 
  SC_CTOR(dcache) {
        SC_CTHREAD(entry, CLK.pos());

	// initialize instruction dmemory from external file
	FILE *fp = fopen("dcache","r");
	int size=0;
	int i=0;
	int mem_word;
  	dmemory = new unsigned[4000];
	printf("** ALERT ** DCU: initialize Data Cache\n");
	while (fscanf(fp,"%x", &mem_word) != EOF) {
		dmemory[size] = mem_word;
		size++;
	}
	for (i=size; i<4000; i++) {
		dmemory[i] = 0xdeadbeef;
	}
  }
  // Process functionality in member function below
  void entry();
};

void dcache::entry()
{
  unsigned int	address;			// address to DataCache

  while (true) {
    	do { wait(); } while ( !(cs == true) ); 
    	address = addr.read();
    	if (we.read() == true) { 		// Write operation
      		wait();
      		out_valid.write(false);
      		dmemory[address] = datain.read();
			cout << "\t\t\t\t\t\t\t-------------------------------" << endl;
			printf("\t\t\t\t\t\t\tDCU :wr %x->mem[%d]\n", dmemory[address], address);
			cout << "\t\t\t\t\t\t\tat CSIM " << sc_time_stamp() << endl;
			cout << "\t\t\t\t\t\t\t-------------------------------" << endl;
			wait();
    	}
    	else { 					// Read operation
			wait(); 	
        	dataout.write(dmemory[address]);
        	out_valid.write(true);
			cout << "\t\t\t\t\t\t\t-------------------------------" << endl;
			printf("\t\t\t\t\t\t\tDCU :ld 0x%x(%d)<-mem[%d]\n", dmemory[address], dmemory[address], address);
			cout << "\t\t\t\t\t\t\tat CSIM " << sc_time_stamp() << endl;
			cout << "\t\t\t\t\t\t\t-------------------------------" << endl;
        	wait();
        	out_valid.write(false);
        	wait();
    	}    
  }
} // end of entry function