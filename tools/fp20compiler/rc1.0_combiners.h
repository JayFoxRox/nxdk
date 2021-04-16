#ifndef _RC10_COMBINERS_H
#define _RC10_COMBINERS_H

#include "rc1.0_general.h"
#include "rc1.0_final.h"

class CombinersStruct {
public:
    void Init(GeneralCombinersStruct _gcs, FinalCombinerStruct _fc, ConstColorsStruct _gccs, ConstColorsStruct _fccs)
    { generals = _gcs; final = _fc; generalCcs = _gccs; finalCcs = _fccs;}
    void Validate();
    void Invoke();
private:
    GeneralCombinersStruct generals;
    FinalCombinerStruct final;
    ConstColorsStruct generalCcs;
    ConstColorsStruct finalCcs;
};

#endif
