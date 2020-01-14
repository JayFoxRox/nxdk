#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <hal/video.h>
#include <hal/debug.h>
#include <windows.h>

static void draw_title(const char* title)
{
    extern int nextCol;
    extern int nextRow;
    nextCol = 25;
    nextRow = 25;
    debugPrint("%s", title);
    for(int i = strlen(title); i < 60; i++) {
        debugPrint(" ");
    }
}

int main(void)
{
    unsigned int bpp = 16;

    while(1) {

        /* Toggle between 15 and 16 bpp mode */
        bpp ^= 0x1F;
        XVideoSetMode(640, 480, bpp, REFRESH_DEFAULT);

        /* Create test image in framebuffer */
        uint16_t *rgbx = (uint16_t*)XVideoGetFB();
        for(unsigned int y = 0; y < 480; y++) {
            unsigned int stripe = y / (480/3);

            /* Border left */
            for(unsigned int x = 0; x < (640-512)/2; x++) {
                *rgbx++ = 0xFFFF;
            }
            /* 512 pixels gradient */
            for(unsigned int x = 0; x < 512; x++) {
                int color = x / 8;
                if (bpp == 16) {
                    if (stripe == 0) { *rgbx++ = (color / 2) << 11; }
                    if (stripe == 1) { *rgbx++ = (color / 1) << 5; }
                    if (stripe == 2) { *rgbx++ = (color / 2) << 0; }
                } else {
                    assert(bpp == 15);
                    if (stripe == 0) { *rgbx++ = (color / 2) << 10; }
                    if (stripe == 1) { *rgbx++ = (color / 2) << 5; }
                    if (stripe == 2) { *rgbx++ = (color / 2) << 0; }
                }
            }
            /* Border right */
            for(unsigned int x = 0; x < (640-512)/2; x++) {
                *rgbx++ = 0x0000;
            }
        }

        char title[32];
        sprintf(title, "Linear %dbpp", bpp);
        draw_title(title);

        /* Wait for capture hardware and start "clean" */
        Sleep(3000);
        XVideoWaitForVBlank();

        for(int mark = 0; mark < 256; mark++) {

#if 0
            /* HCI */
            Sleep(250);
#endif

            /* Enable dropping up to 1 frame */
            XVideoWaitForVBlank();
            XVideoWaitForVBlank();

            sprintf(title, "Mark %dbpp at %d", bpp, mark);
            draw_title(title);

            GAMMA_RAMP_ENTRY entries[256];
            int hits5 = 0;
            int hits6 = 0;
            for(int i = 0; i < 256; i++) {
                entries[i].red   = i;
                entries[i].green = (i == mark) ? 0xFF : 0x00;
                entries[i].blue  = (i == mark) ? 0xFF : 0x00;

#if 0
                // Mark all possible values in 15bpp and 16bpp mode
                unsigned int msb3 = i >> 5; // Keep 3 bits
                unsigned int msb2 = i >> 6; // Keep 2 bits
                unsigned int msb5 = i >> 3; // Keep 5 bits
                unsigned int msb6 = i >> 2; // Keep 6 bits
                bool hit5 = (i == ((msb5 << 3) | msb3));
                bool hit6 = (i == ((msb6 << 2) | msb2));
                if (bpp == 15) {
                    entries[i].green = i + (hit5 ? 0x00 : 0x80);
                } else {
                    entries[i].green = i + (hit6 ? 0x00 : 0x80);
                }
                entries[i].blue = i + (hit5 ? 0x00 : 0x80);

                if (hit5) { hits5++; }
                if (hit6) { hits6++; }
            }
            assert(hits5 == 32);
            assert(hits6 == 64);
#else
            }
#endif
            XVideoSetGammaRamp(0, entries, 256);

        }

        /* Wait for capture hardware to finish processing */
        Sleep(1000);

    }

    return 0;
}
