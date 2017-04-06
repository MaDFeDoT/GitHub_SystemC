#include "FIFOParam.h"

template <class T>
class FIFOParamExtended : public FIFOParam<T>{

public:
	sc_out<bool> almost_empty;
	sc_out<bool> almost_full;

	void almost_out();

	SC_HAS_PROCESS(FIFOParamExtended);
	FIFOParamExtended(sc_module_name name, int max) : FIFOParam(name, max) 
	{
		SC_METHOD(almost_out);
		sensitive << FIFOParam<T>::push_pointer << FIFOParam<T>::pop_pointer;
	};
};

template <class T>
void FIFOParamExtended<T>::almost_out() {
	if (FIFOParam<T>::push_pointer.read()-FIFOParam<T>::pop_pointer.read()==1) almost_empty=1;
	else almost_empty=0;
	if (FIFOParam<T>::pop_pointer.read()-FIFOParam<T>::push_pointer.read()==1) almost_full=1;
	else almost_full=0;
}