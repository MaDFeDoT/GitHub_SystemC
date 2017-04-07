#include "FIFOParam.h"

template <class T>
class FIFOParamExtended : public FIFOParam<T>{

public:
	sc_out<bool> almost_empty;
	sc_out<bool> almost_full;
	int almost_level;
	void almost_out();

	SC_HAS_PROCESS(FIFOParamExtended);
	FIFOParamExtended(sc_module_name name, int max, int almost=1) : FIFOParam(name, max) 
	{
		almost_level=almost;
		SC_METHOD(almost_out);
		sensitive << FIFOParam<T>::push_pointer << FIFOParam<T>::pop_pointer;
	};
};

template <class T>
void FIFOParamExtended<T>::almost_out() {
	if ((FIFOParam<T>::count_of_elements<almost_level) ||
		((FIFOParam<T>::count_of_elements==almost_level) && 
		((FIFOParam<T>::push_event==false) || (FIFOParam<T>::pop_event==true && FIFOParam<T>::push_event==true))) ||
		((FIFOParam<T>::count_of_elements==almost_level+1) && (FIFOParam<T>::pop_event==true && FIFOParam<T>::push_event==false))
		) almost_empty=1;
	else almost_empty=0;
	
	if ((FIFOParam<T>::count_of_elements>FIFOParam<T>::size-almost_level) ||
		((FIFOParam<T>::count_of_elements==FIFOParam<T>::size-almost_level) && ((FIFOParam<T>::pop_event==false) || (FIFOParam<T>::pop_event==true && FIFOParam<T>::push_event==true))) ||
		((FIFOParam<T>::count_of_elements==FIFOParam<T>::size-almost_level-1) && (FIFOParam<T>::push_event==true && FIFOParam<T>::pop_event==false))) almost_full=1;
	else almost_full=0;
}