#define MAX_CODE_LENGTH 1000
#include <systemc.h>
#include "directive.h"

struct icache : sc_module { 
  	sc_in<unsigned>  	datain; 		// modified instruction
  	sc_in<bool>  		cs;    			// chip select
  	sc_in<bool>  		we;    			// write enable for SMC(Static Memory Controller)
  	sc_in<unsigned >  	addr;  			// address
  	sc_out<unsigned >  	dataout;  		// ram data out
  	sc_out<bool>  		icache_valid; 	// output valid
  	sc_in_clk 			CLK;

  // Parameter
  unsigned *icmemory;	 				// icache data memory

  int wait_cycles; // Number of cycles it takes to access imemory

  void init_param(int given_delay_cycles) {
    wait_cycles = given_delay_cycles;
  }
 
  //Constructor
  SC_CTOR(icache) {
        SC_CTHREAD(entry, CLK.pos());

	// initialize instruction icmemory from external file
	FILE *fp = fopen("icache","r");
	int size=0;
	int mem_word;
  	icmemory 	= new unsigned[MAX_CODE_LENGTH];
	for (size = 0; size < MAX_CODE_LENGTH; size++) {	// initialize bad data
		icmemory[size] = 0xeeeeeeee;
	}
	size = 0;
	while (fscanf(fp,"%x", &mem_word) != EOF) {
		icmemory[size] = mem_word;
		size++;
	}
  }

  // Process functionality in member function below
  void entry();
};

void icache::entry()
{
    unsigned int 	address;

	while (true) {
    	do { wait(); } while ( !(cs == true) ); 
    	address = addr.read();
    		if (we.read() == true) { // Write operation
      			wait();
				if (address < MAX_CODE_LENGTH && address >= 0)
				{
					icmemory[address] = datain.read();
					printf("------------------------\n");
					printf("ICU: write to mem[%d]=%x\n", address, datain.read());
					printf("------------------------\n");
				}
                else
					printf("ICU ALERT: **MEMORY OUT OF RANGE**\n");
      			wait();
    		}
    		else { // Read operation
				wait(); // Introduce delay needed
				if (address >= MAX_CODE_LENGTH || address < 0) {
					dataout.write(0xffffffff);
					printf("ICU ALERT: **MEMORY OUT OF RANGE**\n");
				}
                else
        			dataout.write(icmemory[address]);
				icache_valid.write(true);

				if (PRINT_ICU) {
					printf("------------------------\n");
					printf("ICU: fetching mem[%d]\n", address);
                    if (address < MAX_CODE_LENGTH && address >= 0) 
						printf("ICU: (%0x)", icmemory[address]); 
       		        cout.setf(ios::dec,ios::basefield);
                    cout << " at CSIM " << sc_time_stamp() << endl;
					printf("------------------------\n");
				}

      			wait();
				icache_valid.write(false);
      			wait();
    		}    
	
	}
}