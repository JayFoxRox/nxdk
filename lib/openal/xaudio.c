/**
 * OpenAL cross platform audio library
 * Copyright (C) 1999-2007 by authors.
 * This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 *  License along with this library; if not, write to the
 *  Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *  Boston, MA  02111-1307, USA.
 * Or go to http://www.gnu.org/copyleft/lgpl.html
 */

#include <assert.h>

#include <xboxkrnl/xboxkrnl.h>
#include <hal/audio.h>
#include <hal/debug.h>

#include "alMain.h"
#include "AL/al.h"
#include "AL/alc.h"

// We'll use 1024 samples, with 2 channels and 16-bit each
#define FRAME_SIZE (2 * 2)
#define BUFFER_SIZE (1024 * FRAME_SIZE)
#define BUFFER_COUNT 2
static unsigned char* buffers[BUFFER_COUNT];
static int buffer_index = 0;

static void XAudioProc(void *pac97Device, void *data)
{
    ALCdevice *pDevice = (ALCdevice*)data;

    /* This is run from a DPC, so store the FPU state */
    KFLOATING_SAVE float_save;
    NTSTATUS status = KeSaveFloatingPointState(&float_save);
    assert(status == STATUS_SUCCESS);

    // If we have an active context, mix data directly into output buffer otherwise fill with silence
    SuspendContext(NULL);
    aluMixData(pDevice->Context, buffers[buffer_index], BUFFER_SIZE, pDevice->Format);
    XAudioProvideSamples(buffers[buffer_index], BUFFER_SIZE, 0);
    ProcessContext(NULL);

    // Use another buffer next time
    buffer_index += 1;
    buffer_index %= BUFFER_COUNT;

    /* This is run from a DPC, so restore the FPU state */
    status = KeRestoreFloatingPointState(&float_save);
    assert(status == STATUS_SUCCESS);
}

static ALCboolean XAudioOpenPlayback(ALCdevice *device, const ALCchar *deviceName)
{
    int i;

    device->szDeviceName = "XAudio";
    device->Format = AL_FORMAT_STEREO16;
    device->Frequency = 48000;
    device->UpdateSize = BUFFER_SIZE / FRAME_SIZE;

    XAudioInit(16, 2, XAudioProc, device);

    for(i = 0; i < BUFFER_COUNT; i++) {
      buffers[i] = MmAllocateContiguousMemory(BUFFER_SIZE);
      memset(buffers[i], 0x00, BUFFER_SIZE);
      XAudioProvideSamples(buffers[i], BUFFER_SIZE, 0);
    }

    XAudioPlay();

    return ALC_TRUE;
}

static void XAudioClosePlayback(ALCdevice *device)
{
    XAudioPause();
}


static ALCboolean XAudioOpenCapture(ALCdevice *pDevice, const ALCchar *deviceName, ALCuint frequency, ALCenum format, ALCsizei SampleSize)
{
    (void)pDevice;
    (void)deviceName;
    (void)frequency;
    (void)format;
    (void)SampleSize;
    return ALC_FALSE;
}

static void XAudioCloseCapture(ALCdevice *pDevice)
{
    (void)pDevice;
}

static void XAudioStartCapture(ALCdevice *pDevice)
{
    (void)pDevice;
}

static void XAudioStopCapture(ALCdevice *pDevice)
{
    (void)pDevice;
}

static void XAudioCaptureSamples(ALCdevice *pDevice, ALCvoid *pBuffer, ALCuint lSamples)
{
    (void)pDevice;
    (void)pBuffer;
    (void)lSamples;
}

static ALCuint XAudioAvailableSamples(ALCdevice *pDevice)
{
    (void)pDevice;
    return 0;
}


BackendFuncs XAudioFuncs = {
    XAudioOpenPlayback,
    XAudioClosePlayback,
    XAudioOpenCapture,
    XAudioCloseCapture,
    XAudioStartCapture,
    XAudioStopCapture,
    XAudioCaptureSamples,
    XAudioAvailableSamples
};


// This is injected instead of XAudio, so we don't have to modify the OpenAL submodule
void alcDSoundInit(BackendFuncs *FuncList)
{
    *FuncList = XAudioFuncs;
}
