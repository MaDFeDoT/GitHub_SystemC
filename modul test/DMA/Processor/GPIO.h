#include "systemc.h"
#include "directive.h"

struct gpio : sc_module { 
  	sc_in<signed> 		datain; 		// input data from processor
	sc_in<signed>       gpio_data_in;	//input data from gpio
  	sc_in<bool>  		cs;    			// chip select
  	sc_in<bool>  		we;    			// write enable 
  	sc_out<signed>  	dataout;  		// data out to processor
	sc_out<signed>      gpio_dataout;	//output data from gpio
  	sc_out<bool>  		out_valid;		// output valid
    sc_in_clk 			CLK;


  // Parameter
  unsigned gpio_out;	 			// data out
  unsigned gpio_in;					// data in
  int wait_cycles; 					// cycles # it takes to access

  void init_param(int given_delay_cycles) {
    wait_cycles = given_delay_cycles; 
  }
 
  //Constructor 
  SC_CTOR(gpio) {
        SC_CTHREAD(entry, CLK.pos());
		gpio_in=0;
		gpio_out=0;
  }
  // Process functionality in member function below
  void entry();
};

void gpio::entry()
{
  while (true) {
    	do { 
			wait();
			gpio_dataout.write(gpio_out);
			gpio_in=gpio_data_in.read();
		} while ( !(cs == true) ); 

    	if (we.read() == true) { 		// Write operation
      		wait();
      		out_valid.write(false);
      		gpio_out = datain.read();
			if (PRINT_GPIO) {
				cout << "\t\t\t\t\t\t\t-------------------------------" << endl;
				printf("\t\t\t\t\t\t\tGPIO :wr %x->gpio out\n", gpio_out);
				cout << "\t\t\t\t\t\t\tat CSIM " << sc_time_stamp() << endl;
				cout << "\t\t\t\t\t\t\t-------------------------------" << endl;
			}
			wait();
    	}
    	else { 					// Read operation
			wait(); 	
			dataout.write(gpio_in);
        	out_valid.write(true);
			if (PRINT_DCU) {
				cout << "\t\t\t\t\t\t\t-------------------------------" << endl;
				printf("\t\t\t\t\t\t\tGPIO :ld 0x%x(%d)<-gpio in\n", gpio_in, gpio_in);
				cout << "\t\t\t\t\t\t\tat CSIM " << sc_time_stamp() << endl;
				cout << "\t\t\t\t\t\t\t-------------------------------" << endl;
			}
        	wait();
        	out_valid.write(false);
        	wait();
    	}    
  }
} // end of entry function