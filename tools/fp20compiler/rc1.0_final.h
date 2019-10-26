#ifndef _RC10_FINAL_H
#define _RC10_FINAL_H

#include "rc1.0_general.h"
#include "rc1.0_register.h"

class FinalAlphaFunctionStruct {
public:
    void Init(MappedRegisterStruct _g) { g = _g; }
    void ZeroOut();
    MappedRegisterStruct g;
};

class FinalRgbFunctionStruct {
public:
    void Init(MappedRegisterStruct _a, MappedRegisterStruct _b, MappedRegisterStruct _c, MappedRegisterStruct _d)
    { a = _a; b = _b; c = _c; d = _d; }
    void ZeroOut();
    MappedRegisterStruct a;
    MappedRegisterStruct b;
    MappedRegisterStruct c;
    MappedRegisterStruct d;
};

class FinalProductStruct {
public:
    void Init(MappedRegisterStruct _e, MappedRegisterStruct _f) { e = _e; f = _f; }
    void ZeroOut();
    MappedRegisterStruct e;
    MappedRegisterStruct f;
};

class FinalCombinerStruct {
public:
    void Init(FinalRgbFunctionStruct _rgb, FinalAlphaFunctionStruct _alpha, int _clamp, FinalProductStruct _product)
    { rgb = _rgb; alpha = _alpha; clamp = _clamp; product = _product; hasProduct = true;}
    void Init(FinalRgbFunctionStruct _rgb, FinalAlphaFunctionStruct _alpha, int _clamp)
    { rgb = _rgb; alpha = _alpha; clamp = _clamp; hasProduct = false; product.ZeroOut();}

    int hasProduct;
    FinalProductStruct product;
    int clamp;
    FinalRgbFunctionStruct rgb;
    FinalAlphaFunctionStruct alpha;
    void Validate();
    void Invoke();
};

class ColoredFinalCombinerStruct {
public:
    void Init(FinalCombinerStruct _final, ConstColorStruct _cc0, ConstColorStruct _cc1)
    { final = _final; cc[0] = _cc0; cc[1] = _cc1; numConsts = 2; }
    void Init(FinalCombinerStruct _final, ConstColorStruct _cc0)
    { final = _final; cc[0] = _cc0; numConsts = 1; }
    void Init(FinalCombinerStruct _final)
    { final = _final; numConsts = 0; }

    FinalCombinerStruct final;
    ConstColorStruct cc[2];
    int numConsts;
    void Validate(int numGlobalConsts, ConstColorStruct *globalCCs);
    void Invoke();
};

#endif
