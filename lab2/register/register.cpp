//-----------------------------------------------------
// An 8 bit register with asynchronous active high reset
//-----------------------------------------------------
#include "register.h"

void eightbit_register::register_store() {
    // We check if reset is active    
    myregister = 0;
    wait();
    while (true) {
        myregister = register_in.read();
        cout << "@" << sc_time_stamp() << " :: Have stored " << myregister << endl;
        wait();
    }
}

void eightbit_register::calculate_parity() {
    parity = myregister.read().xor_reduce();
}

void eightbit_register::data_out_method() {
    register_out = myregister;
}
