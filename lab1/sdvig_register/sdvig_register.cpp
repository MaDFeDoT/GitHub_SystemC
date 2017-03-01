#include "sdvig_register.h"

void sd_register::reg_sd() {
    // We check if reset is active    
    myregister = 0;
    register_out.write(myregister);
    wait();
    while (true) {
		if (left_right.read()) {
			myregister = myregister>>1;
			if(d.read()){
				myregister = myregister|128;
			}
			else
			{
				myregister = myregister&127;
			}
			//myregister = myregister&127;
			cout << "@" << sc_time_stamp() << " :: Have out " << myregister << endl;
			register_out.write(myregister);
        }
		else
		{
			myregister = myregister<<1;
			if(d.read()){
				myregister = myregister|1;
			}
			else
			{
				myregister = myregister&254;
			}
			cout << "@" << sc_time_stamp() << " :: Have out " << myregister << endl;
			register_out.write(myregister);
		}
        wait();
    }
}
