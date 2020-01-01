// Licensed under GPLv2 or (at your option) any later version.
// Copyright (C) 2020 Jannik Vogel

#include "Exe.h"
#include "Common.h"

#include <string.h>
#include <stdio.h>

// program entry point
int main(int argc, char *argv[])
{
    char szErrorMessage[ERROR_LEN+1] = {0};
    char szExeFilename[OPTION_LEN+1] = {0};
    char szDxtFilename[OPTION_LEN+1] = {0};

    const char* program = argv[0];
    const char* program_desc = "CDXT: EXE to DXT Relinker";
    static Option options[] = {
        { szExeFilename, NULL,  "exefile"  },
        { szDxtFilename, "OUT", "filename" },
        { NULL }
    };

    if(ParseOptions(argv, argc, options, szErrorMessage)) {
        goto cleanup;
    }

    // verify we recieved the required parameters
    if(szExeFilename[0] == '\0')
    {
        ShowUsage(program, program_desc, options);
        return 1;
    }

    // if we don't have an Dxt filename, generate one from szExeFilename
    if(szDxtFilename[0] == '\0')
    {
        GenerateFilename(szDxtFilename, ".dxt", szExeFilename, ".exe");
    }

    // open and convert Exe file
    {
        Exe *ExeFile = new Exe(szExeFilename);

        if(ExeFile->GetError() != 0)
        {
            strncpy(szErrorMessage, ExeFile->GetError(), ERROR_LEN);
            goto cleanup;
        }

        // Set up subsystem (will be ignored)
        ExeFile->m_OptionalHeader.m_subsystem_version_major = 1;
        ExeFile->m_OptionalHeader.m_subsystem_version_minor = 0;
        ExeFile->m_OptionalHeader.m_subsystem = IMAGE_SUBSYSTEM_XBOX;

        // Set up alignments (DXTs typically use 32 byte alignments)
        //FIXME: Report error if FileAlignment > SectionAlignment
        ExeFile->m_OptionalHeader.m_file_alignment =
            ExeFile->m_OptionalHeader.m_section_alignment;

        // DXTs are EXE files, which are in-memory images.
        // The loader in XBDM.dll loads the file into one large memory area.
        // It does not load section-by-section and will ignore where the
        // section should be loaded at.
        // Therefore the raw and virtual address must match.
        // If our DXT do not respect this, the DXT loader will crash during
        // relocation (using .reloc) when trying to access sections.
        for(uint32 v=0;v<ExeFile->m_Header.m_sections;v++)
        {
            ExeFile->m_SectionHeader[v].m_raw_addr =
                ExeFile->m_SectionHeader[v].m_virtual_addr;
        }

        ExeFile->Export(szDxtFilename);

        if(ExeFile->GetError() != 0)
        {
            strncpy(szErrorMessage, ExeFile->GetError(), ERROR_LEN);
            goto cleanup;
        }
    }

cleanup:

    if(szErrorMessage[0] != 0)
    {
        ShowUsage(program, program_desc, options);
    
        printf("\n");
        printf(" *  Error : %s\n", szErrorMessage);

        return 1;
    }

    return 0;
}
