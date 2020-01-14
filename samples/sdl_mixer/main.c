/*
  Based on SDL_mixer playmus.c (c05644b29179ff534efe24a548a151e305b688ea)

  PLAYMUS:  A test application for the SDL mixer library.
  Copyright (C) 1997-2019 Sam Lantinga <slouken@libsdl.org>
  Copyright (C) 2020 Jannik Vogel

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

/* Quiet windows compiler warnings */
#define _CRT_SECURE_NO_WARNINGS

/* $Id$ */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "SDL.h"
#include "SDL_mixer.h"

#include <hal/video.h>
#include <hal/debug.h>

static const char* songs[] = {
    "D:\\nxdk.wav",
//    "D:\\song.ogg",
    NULL
};


static int audio_open = 0;
static Mix_Music *music = NULL;

static void CleanUp(int exitcode)
{
    if(Mix_PlayingMusic()) {
        Mix_FadeOutMusic(1500);
        SDL_Delay(1500);
    }
    if (music) {
        Mix_FreeMusic(music);
        music = NULL;
    }
    if (audio_open) {
        Mix_CloseAudio();
        audio_open = 0;
    }

    debugPrint("Will exit in 2 seconds (Status: %d).\n", exitcode);
    SDL_Delay(2000);

    SDL_Quit();
    exit(exitcode);
}

static void Menu(void)
{
#if 0
    char buf[10];

    printf("Available commands: (p)ause (r)esume (h)alt volume(v#) > ");
    fflush(stdin);
    if (scanf("%s",buf) == 1) {
        switch(buf[0]){
        case '0': Mix_SetMusicPosition(0); break;
        case '1': Mix_SetMusicPosition(10);break;
        case '2': Mix_SetMusicPosition(20);break;
        case '3': Mix_SetMusicPosition(30);break;
        case '4': Mix_SetMusicPosition(40);break;
        case 'p': case 'P':
            Mix_PauseMusic();
            break;
        case 'r': case 'R':
            Mix_ResumeMusic();
            break;
        case 'h': case 'H':
            Mix_HaltMusic();
            break;
        case 'v': case 'V':
            Mix_VolumeMusic(atoi(buf+1));
            break;
        }
    }
#endif
}

static const char* GetMusicTypename(Mix_MusicType type)
{
    switch (type) {
    case MUS_CMD:
        return "CMD";
    case MUS_WAV:
        return "WAV";
    case MUS_MOD:
    case MUS_MODPLUG_UNUSED:
        return "MOD";
    case MUS_FLAC:
        return "FLAC";
    case MUS_MID:
        return "MIDI";
    case MUS_OGG:
        return "OGG Vorbis";
    case MUS_MP3:
    case MUS_MP3_MAD_UNUSED:
        return "MP3";
    case MUS_OPUS:
        return "OPUS";
    case MUS_NONE:
        return "NONE";
    default:
        break;
    }
    return "(Unknown)";
}

static void PrintMusicInformation()
{
    Mix_MusicType type = Mix_GetMusicType(music);
    debugPrint("Detected music type: %s\n", GetMusicTypename(type));

    const char* tag_title = Mix_GetMusicTitleTag(music);
    if (tag_title && SDL_strlen(tag_title) > 0) {
        debugPrint("Title: %s\n", tag_title);
    }

    const char* tag_artist = Mix_GetMusicArtistTag(music);
    if (tag_artist && SDL_strlen(tag_artist) > 0) {
        debugPrint("Artist: %s\n", tag_artist);
    }

    const char* tag_album = Mix_GetMusicAlbumTag(music);
    if (tag_album && SDL_strlen(tag_album) > 0) {
        debugPrint("Album: %s\n", tag_album);
    }

    const char *tag_copyright = Mix_GetMusicCopyrightTag(music);
    if (tag_copyright && SDL_strlen(tag_copyright) > 0) {
        debugPrint("Copyright: %s\n", tag_copyright);
    }
}

int main(void)
{
    double loop_start, loop_end, loop_length;

    /* Initialize variables */
    int audio_rate = MIX_DEFAULT_FREQUENCY;
    Uint16 audio_format = MIX_DEFAULT_FORMAT;
    int audio_channels = MIX_DEFAULT_CHANNELS;
    int audio_buffers = 4096;
    int audio_volume = MIX_MAX_VOLUME;
    int looping = 0;

    /* Set up Xbox video */
    XVideoSetMode(640, 480, 32, REFRESH_DEFAULT);

    /* Initialize the SDL library */
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        debugPrint("Couldn't initialize SDL: %s\n",SDL_GetError());
        return(255);
    }

    /* Open the audio device */
    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers) < 0) {
        debugPrint("Couldn't open audio: %s\n", SDL_GetError());
        return(2);
    } else {
        Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
        debugPrint("Opened audio at %d Hz %d bit%s %s %d bytes audio buffer\n", audio_rate,
            (audio_format&0xFF),
            (SDL_AUDIO_ISFLOAT(audio_format) ? " (float)" : ""),
            (audio_channels > 2) ? "surround" : (audio_channels > 1) ? "stereo" : "mono",
            audio_buffers);
    }
    audio_open = 1;

    /* Set the music volume */
    debugPrint("Setting volume\n");
    Mix_VolumeMusic(audio_volume);

    int i = 0;
    while (songs[i]) {

        /* Load the requested music file */
        debugPrint("Opening %s\n", songs[i]);
        SDL_RWops *rw = SDL_RWFromFile(songs[i], "rb");
        if (rw == NULL) {
            debugPrint("Couldn't open %s: %s\n",
                songs[i], SDL_GetError());
            CleanUp(2);
        }
        debugPrint("Loading %s\n", songs[i]);
        music = Mix_LoadMUS_RW(rw, SDL_TRUE);
        if (music == NULL) {
            debugPrint("Couldn't load %s: %s\n",
                songs[i], SDL_GetError());
            CleanUp(3);
        }
        debugPrint("Loaded %s\n", songs[i]);

        PrintMusicInformation();

        loop_start = Mix_GetMusicLoopStartTime(music);
        loop_end = Mix_GetMusicLoopEndTime(music);
        loop_length = Mix_GetMusicLoopLengthTime(music);

        /* Play and then exit */
        debugPrint("Playing %s, duration %d\n", songs[i], (int)Mix_MusicDuration(music) * 1000);
        if (loop_start > 0.0 && loop_end > 0.0 && loop_length > 0.0) {
            debugPrint("Loop points: start %d ms, end %d ms, length %d ms\n", (int)loop_start * 1000, (int)loop_end * 1000, (int)loop_length * 1000);
        }
        Mix_FadeInMusic(music,looping,2000);
        while (Mix_PlayingMusic() || Mix_PausedMusic()) {
            Menu();

            double current_position = Mix_GetMusicPosition(music);

            debugPrint("Music playing: %s Paused: %s Position: %d\n", Mix_PlayingMusic() ? "yes" : "no",
            Mix_PausedMusic() ? "yes" : "no", (int)current_position * 1000);

            SDL_Delay(100);
        }
        Mix_FreeMusic(music);
        music = NULL;

//        i++;
    }
    CleanUp(0);

    /* Not reached, but fixes compiler warnings */
    return 0;
}

/* vi: set ts=4 sw=4 expandtab: */
