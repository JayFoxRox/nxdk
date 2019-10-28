// #include <glh/glh_extensions.h>

#include "rc1.0_combiners.h"
#include "nvparse_errors.h"
#include "nvparse_externs.h"

#include <cstdio>
#include <cstring>
#include <cassert>

void CombinersStruct::Validate()
{
    generalCcs.Validate();
    generals.Validate(&generalCcs);

    finalCcs.Validate();
    finalCcs.SetUnusedConst(REG_CONSTANT_COLOR0, &generalCcs);
    finalCcs.SetUnusedConst(REG_CONSTANT_COLOR1, &generalCcs);
    final.Validate();
}

void CombinersStruct::Invoke()
{
    printf("#pragma push_macro(\"MASK\")\n");
    printf("#undef MASK\n");
    printf("#define MASK(mask, val) (((val) << (__builtin_ffs(mask)-1)) & (mask))\n");
    printf("\n");

    assert(generalCcs.numConsts <= 2);
    for (int i = 0; i < generalCcs.numConsts; i++) {
    //     glCombinerParameterfvNV(cc[i].reg.bits.name, &(cc[i].v[0]));
        const char* cmd = NULL;
        int localConstCount = 0;
        switch(generalCcs.cc[i].reg.bits.name) {
        case REG_CONSTANT_COLOR0:
            cmd = "NV097_SET_COMBINER_FACTOR0";
            localConstCount = generals.localConst0Count;
            break;
        case REG_CONSTANT_COLOR1:
            cmd = "NV097_SET_COMBINER_FACTOR1";
            localConstCount = generals.localConst1Count;
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
        if (localConstCount == 0) {
            printf("pb_push1(p, %s,", cmd);
            printf("\n    MASK(0xFF000000, 0x%02X)", (unsigned char)(generalCcs.cc[i].v[3] * 0xFF));
            printf("\n    | MASK(0x00FF0000, 0x%02X)", (unsigned char)(generalCcs.cc[i].v[0] * 0xFF));
            printf("\n    | MASK(0x0000FF00, 0x%02X)", (unsigned char)(generalCcs.cc[i].v[1] * 0xFF));
            printf("\n    | MASK(0x000000FF, 0x%02X)", (unsigned char)(generalCcs.cc[i].v[2] * 0xFF));
            printf(");\n");
            printf("p += 2;\n");
        }
    }

    generals.Invoke();

    assert(finalCcs.numConsts <= 2);
    for (int i = 0; i < finalCcs.numConsts; i++) {
        const char* cmd = NULL;
        switch(finalCcs.cc[i].reg.bits.name) {
        case REG_CONSTANT_COLOR0:
            cmd = "NV097_SET_SPECULAR_FOG_FACTOR + 0";
            break;
        case REG_CONSTANT_COLOR1:
            cmd = "NV097_SET_SPECULAR_FOG_FACTOR + 4";
            break;
        default:
            assert(false);
            break;
        }

        printf("pb_push1(p, %s,", cmd);
        printf("\n    MASK(0xFF000000, 0x%02X)", (unsigned char)(finalCcs.cc[i].v[3] * 0xFF));
        printf("\n    | MASK(0x00FF0000, 0x%02X)", (unsigned char)(finalCcs.cc[i].v[0] * 0xFF));
        printf("\n    | MASK(0x0000FF00, 0x%02X)", (unsigned char)(finalCcs.cc[i].v[1] * 0xFF));
        printf("\n    | MASK(0x000000FF, 0x%02X)", (unsigned char)(finalCcs.cc[i].v[2] * 0xFF));
        printf(");\n");
        printf("p += 2;\n");
    }

    final.Invoke();

    printf("\n");
    printf("#pragma pop_macro(\"MASK\")\n");
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
