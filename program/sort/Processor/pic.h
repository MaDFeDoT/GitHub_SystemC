#include "systemc.h"

struct pic : sc_module { 
  	sc_in<bool>  			ireq0; 		// interrupt request 0   	
  	sc_in<bool>  			ireq1;    	// interrupt request 1
  	sc_in<bool>  			ireq2;    	// interrupt request 2
  	sc_in<bool>  			ireq3;    	// interrupt request 3
  	sc_in<bool>  			intack_cpu;    	// interrupt acknowledge from CPU
  	sc_out<bool>  			intreq;         // interrupt request to CPU			
  	sc_out<unsigned>        vectno;         // vector number		


  //Constructor 
  SC_CTOR(pic) {
      SC_METHOD(entry);
      dont_initialize();
      sensitive << ireq0 << ireq1 << ireq2 << ireq3 << intack_cpu;
  }


  // Process functionality in member function below
  void entry();
};

void pic::entry(){

	if (ireq0.read() == true) {
		intreq.write(true);
		vectno.write(0);
	} else if (ireq1.read() == true) {
			intreq.write(true);
			vectno.write(1);
		} else if (ireq2.read() == true) {
				intreq.write(true);
				vectno.write(2);
			} else if (ireq3.read() == true) {
						intreq.write(true);
						vectno.write(3);
				}
	if (intack_cpu.read() == true) {
			intreq.write(false);
	}
  }