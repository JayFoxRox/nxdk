#ifndef _RC10_COMBINERS_H
#define _RC10_COMBINERS_H

#include "rc1.0_general.h"
#include "rc1.0_final.h"

class CombinersStruct {
public:
    void Init(GeneralCombinersStruct _gcs, FinalCombinerStruct _fc, ConstColorsStruct _ccs)
    { generals = _gcs; final = _fc; ccs = _ccs;}
    void Validate();
    void Invoke();
private:
    GeneralCombinersStruct generals;
    FinalCombinerStruct final;
    ConstColorsStruct ccs;
};

#endif
