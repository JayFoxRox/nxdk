/* API declaration export attribute */
#define AL_API
#define ALC_API

/* Define a restrict macro for non-aliased pointers */
//FIXME: #define RESTRICT ${RESTRICT_DECL}

/* Define if HRTF data is embedded in the library */
//FIXME: #cmakedefine ALSOFT_EMBED_HRTF_DATA

/* Define if we have the posix_memalign function */
//FIXME: #cmakedefine HAVE_POSIX_MEMALIGN

/* Define if we have the _aligned_malloc function */
//FIXME: #cmakedefine HAVE__ALIGNED_MALLOC

/* Define if we have the proc_pidpath function */
//FIXME: #cmakedefine HAVE_PROC_PIDPATH

/* Define if we have the getopt function */
//FIXME: #cmakedefine HAVE_GETOPT

/* Define if we have SSE CPU extensions */
//FIXME: #cmakedefine HAVE_SSE
#undef HAVE_SSE2
#undef HAVE_SSE3
#undef HAVE_SSE4_1

/* Define if we have ARM Neon CPU extensions */
#undef HAVE_NEON

/* Define if we have the ALSA backend */
#undef HAVE_ALSA

/* Define if we have the OSS backend */
#undef HAVE_OSS

/* Define if we have the Solaris backend */
#undef HAVE_SOLARIS

/* Define if we have the SndIO backend */
#undef HAVE_SNDIO

/* Define if we have the QSA backend */
#undef HAVE_QSA

/* Define if we have the WASAPI backend */
#undef HAVE_WASAPI

/* Define if we have the DSound backend */
#undef HAVE_DSOUND

/* Define if we have the Windows Multimedia backend */
#undef HAVE_WINMM

/* Define if we have the PortAudio backend */
#undef HAVE_PORTAUDIO

/* Define if we have the PulseAudio backend */
#undef HAVE_PULSEAUDIO

/* Define if we have the JACK backend */
#undef HAVE_JACK

/* Define if we have the CoreAudio backend */
#undef HAVE_COREAUDIO

/* Define if we have the OpenSL backend */
#undef HAVE_OPENSL

/* Define if we have the Wave Writer backend */
#undef HAVE_WAVE

/* Define if we have the SDL2 backend */
#define HAVE_SDL2

/* Define if we have the stat function */
//FIXME: #cmakedefine HAVE_STAT

/* Define to the size of a long int type */
#define SIZEOF_LONG sizeof(long)

/* Define if we have GCC's format attribute */
//FIXME: #cmakedefine HAVE_GCC_FORMAT

/* Define if we have dlfcn.h */
//FIXME: #cmakedefine HAVE_DLFCN_H

/* Define if we have pthread_np.h */
//FIXME: #cmakedefine HAVE_PTHREAD_NP_H

/* Define if we have malloc.h */
//FIXME: #cmakedefine HAVE_MALLOC_H

/* Define if we have dirent.h */
#undef HAVE_DIRENT_H

/* Define if we have cpuid.h */
//FIXME: #cmakedefine HAVE_CPUID_H

/* Define if we have intrin.h */
//FIXME: #cmakedefine HAVE_INTRIN_H

/* Define if we have sys/sysconf.h */
//FIXME: #cmakedefine HAVE_SYS_SYSCONF_H

/* Define if we have guiddef.h */
//FIXME: #cmakedefine HAVE_GUIDDEF_H

/* Define if we have initguid.h */
//FIXME: #cmakedefine HAVE_INITGUID_H

/* Define if we have GCC's __get_cpuid() */
//FIXME: #cmakedefine HAVE_GCC_GET_CPUID

/* Define if we have the __cpuid() intrinsic */
//FIXME: #cmakedefine HAVE_CPUID_INTRINSIC

/* Define if we have the _BitScanForward64() intrinsic */
//FIXME: #cmakedefine HAVE_BITSCANFORWARD64_INTRINSIC

/* Define if we have the _BitScanForward() intrinsic */
//FIXME: #cmakedefine HAVE_BITSCANFORWARD_INTRINSIC

/* Define if we have SSE intrinsics */
//FIXME: #cmakedefine HAVE_SSE_INTRINSICS

/* Define if we have pthread_setschedparam() */
//FIXME: #cmakedefine HAVE_PTHREAD_SETSCHEDPARAM

/* Define if we have pthread_setname_np() */
//FIXME: #cmakedefine HAVE_PTHREAD_SETNAME_NP

/* Define if pthread_setname_np() only accepts one parameter */
//FIXME: #cmakedefine PTHREAD_SETNAME_NP_ONE_PARAM

/* Define if pthread_setname_np() accepts three parameters */
//FIXME: #cmakedefine PTHREAD_SETNAME_NP_THREE_PARAMS

/* Define if we have pthread_set_name_np() */
//FIXME: #cmakedefine HAVE_PTHREAD_SET_NAME_NP
