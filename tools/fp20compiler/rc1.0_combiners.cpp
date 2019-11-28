// #include <glh/glh_extensions.h>

#include "rc1.0_combiners.h"
#include "nvparse_errors.h"
#include "nvparse_externs.h"

#include <cstdio>
#include <cstring>
#include <cassert>

void CombinersStruct::Validate()
{
    if (2 == numConsts &&
        cc[0].reg.bits.name == cc[1].reg.bits.name)
        errors.set("global constant set twice");

    generals.Validate(numConsts, &cc[0]);

    final.Validate();
}

static void PrintColorSetter(const char* cmd, const ConstColorStruct& cc) {
    //FIXME: Move to ConstColorStruct::Validate?
    assert(cc.v[0] >= 0.0f && cc.v[0] <= 1.0f);
    assert(cc.v[1] >= 0.0f && cc.v[1] <= 1.0f);
    assert(cc.v[2] >= 0.0f && cc.v[2] <= 1.0f);
    assert(cc.v[3] >= 0.0f && cc.v[3] <= 1.0f);

    //FIXME: Move to ConstColorStruct::Invoke(bool isFinal)?
    printf("pb_push1(p, %s,", cmd);
    printf("\n    MASK(0xFF000000, 0x%02X)", (unsigned char)(cc.v[3] * 0xFF));
    printf("\n    | MASK(0x00FF0000, 0x%02X)", (unsigned char)(cc.v[0] * 0xFF));
    printf("\n    | MASK(0x0000FF00, 0x%02X)", (unsigned char)(cc.v[1] * 0xFF));
    printf("\n    | MASK(0x000000FF, 0x%02X)", (unsigned char)(cc.v[2] * 0xFF));
    printf(");\n");
    printf("p += 2;\n");
}

void CombinersStruct::Invoke()
{
    assert(numConsts <= 2);
    for (int i = 0; i < numConsts; i++) {
    //     glCombinerParameterfvNV(cc[i].reg.bits.name, &(cc[i].v[0]));
        const char* general_cmd = NULL;
        const char* final_cmd = NULL;
        switch(cc[i].reg.bits.name) {
        case REG_CONSTANT_COLOR0:
            general_cmd = "NV097_SET_COMBINER_FACTOR0";
            final_cmd = "NV097_SET_SPECULAR_FOG_FACTOR + 0";
            break;
        case REG_CONSTANT_COLOR1:
            general_cmd = "NV097_SET_COMBINER_FACTOR1";
            final_cmd = "NV097_SET_SPECULAR_FOG_FACTOR + 4";
            break;
        default:
            assert(false);
            break;
        }

        // - If no local-constants are used, the general-combiners only use
        //   global-constants (so we use FACTOR#_SAME_FACTOR_ALL).
        //   NV2A takes those from the first stage, which we emit here.
        // - If any local-constants are used, we don't emit this. The locals
        //   just overlay the globals and each stage (including the first)
        //   will emit its own constants (for FACTOR#_EACH_STAGE).
        // Also see mode selection in GeneralCombinersStruct::Invoke() and
        // local-constant emitter in GeneralCombinerStruct::Invoke(int stage).
        if (generals.localConsts == 0) {
            PrintColorSetter(general_cmd, cc[i]);
        }

        // Global-constants are also used in final-combiner
        PrintColorSetter(final_cmd, cc[i]);
    }


    generals.Invoke();

    final.Invoke();
}

bool is_rc10(const char * s)
{
    return ! strncmp(s, "!!RC1.0", 7);
}


bool rc10_init_more()
{
    
    errors.reset();
    line_number = 1;

    return true;
}
