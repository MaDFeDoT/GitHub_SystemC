#include "counter.h"

void cnt::cnt_count() {
    // We check if reset is active    
    myregister = 0;
    cnt_register_out.write(myregister);
    wait();
    while (true) {
        if (ena.read()) {
        myregister += 1;
        cout << "@" << sc_time_stamp() << " :: Have count " << myregister << endl;
        cnt_register_out.write(myregister);
        }
        wait();
    }
}
