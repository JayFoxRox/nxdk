/*
 * OpenAL Source Play Example
 *
 * Copyright (c) 2017 by Chris Robinson <chris.kcat@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/* This file contains an example for playing a sound buffer. */

#include <stdio.h>
#include <assert.h>
#include <hal/debug.h>
#include <hal/video.h>
#include <windows.h>

#include <SDL.h>

#include "AL/al.h"
#include "AL/alc.h"

#include "alhelpers.h"


/* LoadBuffer loads the named audio file into an OpenAL buffer object, and
 * returns the new buffer ID.
 */
static ALuint LoadSound(const char *filename)
{
    SDL_AudioSpec wav_spec;
    Uint32 wav_length;
    Uint8 *wav_buffer;
    ALenum err, format;
    ALuint buffer;

    /* Load the WAV */
    if (SDL_LoadWAV(filename, &wav_spec, &wav_buffer, &wav_length) == NULL) {
        debugPrint("error: Could not open audio in %s: %s\n", filename, SDL_GetError());
        return 0;
    }

    /* Get the sound format, and figure out the OpenAL format */
    if(wav_spec.channels == 1)
    {
        if(wav_spec.format == AUDIO_U8)
            format = AL_FORMAT_MONO8;
        else if(wav_spec.format == AUDIO_S16SYS)
            format = AL_FORMAT_MONO16;
        else
        {
            debugPrint("error: Unsupported sample format: 0x%04x\n", wav_spec.format);
            SDL_FreeWAV(wav_buffer);
            return 0;
        }
    }
    else if(wav_spec.channels == 2)
    {
        if(wav_spec.format == AUDIO_U8)
            format = AL_FORMAT_STEREO8;
        else if(wav_spec.format == AUDIO_S16SYS)
            format = AL_FORMAT_STEREO16;
        else
        {
            debugPrint("error: Unsupported sample format: 0x%04x\n", wav_spec.format);
            SDL_FreeWAV(wav_buffer);
            return 0;
        }
    }
    else
    {
        debugPrint("error: Unsupported channel count: %d\n", wav_spec.channels);
        SDL_FreeWAV(wav_buffer);
        return 0;
    }

    /* Buffer the audio data into a new buffer object, then free the data and
     * close the file. */
    buffer = 0;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, wav_buffer, wav_length, wav_spec.freq);
    SDL_FreeWAV(wav_buffer);

    /* Check if an error occured, and clean up if so. */
    err = alGetError();
    if(err != AL_NO_ERROR)
    {
        debugPrint("error: OpenAL Error: %s\n", alGetString(err));
        if(buffer && alIsBuffer(buffer))
            alDeleteBuffers(1, &buffer);
        return 0;
    }

    return buffer;
}


int main(int argc, char **argv)
{
    ALuint source, buffer;
    ALfloat offset;
    ALenum state;

    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

    debugPrint("Sample starting\n");

    /* Initialize OpenAL. */
    if(InitAL() != 0) {
        Sleep(5000);
        return 1;
    }

    /* Load the sound into a buffer. */
    buffer = LoadSound("D:\\nxdk.wav");
    if(!buffer) {
        Sleep(5000);
        CloseAL();
        return 1;
    }

    /* Create the source to play the sound with. */
    source = 0;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);
    assert(alGetError()==AL_NO_ERROR && "Failed to setup sound source");

    /* Play the sound until it finishes. */
    alSourcePlay(source);
    do {
        Sleep(1);
        alGetSourcei(source, AL_SOURCE_STATE, &state);

        /* Get the source offset. */
        alGetSourcef(source, AL_SEC_OFFSET, &offset);
        debugPrint("Offset: %llu ms\n", (unsigned long long)(offset * 1000ULL));
        fflush(stdout);
    } while(alGetError() == AL_NO_ERROR && state == AL_PLAYING);
    debugPrint("\n");

    /* All done. Delete resources, and close down OpenAL. */
    alDeleteSources(1, &source);
    alDeleteBuffers(1, &buffer);

    CloseAL();

    debugPrint("Sample finished\n");
    Sleep(5000);
    return 0;
}
