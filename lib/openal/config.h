/* API declaration export attribute */
#define AL_API
#define ALC_API

/* Define any available alignment declaration */
#define ALIGN(x) __attribute__((aligned(x)))

/* Define a built-in call indicating an aligned data pointer */
//FIXME: #define ASSUME_ALIGNED(x, y) ${ASSUME_ALIGNED_DECL}

/* Define if HRTF data is embedded in the library */
//FIXME: #cmakedefine ALSOFT_EMBED_HRTF_DATA

/* Define if we have the sysconf function */
//FIXME: #cmakedefine HAVE_SYSCONF

/* Define if we have the C11 aligned_alloc function */
//FIXME: #cmakedefine HAVE_ALIGNED_ALLOC

/* Define if we have the posix_memalign function */
//FIXME: #cmakedefine HAVE_POSIX_MEMALIGN

/* Define if we have the _aligned_malloc function */
//FIXME: #cmakedefine HAVE__ALIGNED_MALLOC

/* Define if we have the proc_pidpath function */
//FIXME: #cmakedefine HAVE_PROC_PIDPATH

/* Define if we have the getopt function */
//FIXME: #cmakedefine HAVE_GETOPT

/* Define if we have SSE CPU extensions */
#define HAVE_SSE
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

/* Define if we have the lrintf function */
//FIXME: #cmakedefine HAVE_LRINTF

/* Define if we have the modff function */
//FIXME: #cmakedefine HAVE_MODFF

/* Define if we have the log2f function */
//FIXME: #cmakedefine HAVE_LOG2F

/* Define if we have the cbrtf function */
//FIXME: #cmakedefine HAVE_CBRTF

/* Define if we have the copysignf function */
//FIXME: #cmakedefine HAVE_COPYSIGNF

/* Define if we have the strtof function */
//FIXME: #cmakedefine HAVE_STRTOF

/* Define if we have the strnlen function */
#define HAVE_STRNLEN

/* Define if we have the __int64 type */
//FIXME: #cmakedefine HAVE___INT64

/* Define to the size of a long int type */
#define SIZEOF_LONG sizeof(long)

/* Define to the size of a long long int type */
#define SIZEOF_LONG_LONG sizeof(long long)

/* Define if we have C99 _Bool support */
#define HAVE_C99_BOOL

/* Define if we have C11 _Static_assert support */
#define HAVE_C11_STATIC_ASSERT

/* Define if we have C11 _Alignas support */
//FIXME: #cmakedefine HAVE_C11_ALIGNAS

/* Define if we have C11 _Atomic support */
//FIXME: #cmakedefine HAVE_C11_ATOMIC

/* Define if we have GCC's destructor attribute */
#define HAVE_GCC_DESTRUCTOR

/* Define if we have GCC's format attribute */
//FIXME: #cmakedefine HAVE_GCC_FORMAT

/* Define if we have stdint.h */
#define HAVE_STDINT_H

/* Define if we have stdbool.h */
#define HAVE_STDBOOL_H

/* Define if we have stdalign.h */
//FIXME: #cmakedefine HAVE_STDALIGN_H

/* Define if we have windows.h */
#define HAVE_WINDOWS_H

/* Define if we have dlfcn.h */
//FIXME: #cmakedefine HAVE_DLFCN_H

/* Define if we have pthread_np.h */
//FIXME: #cmakedefine HAVE_PTHREAD_NP_H

/* Define if we have malloc.h */
//FIXME: #cmakedefine HAVE_MALLOC_H

/* Define if we have dirent.h */
#undef HAVE_DIRENT_H

/* Define if we have strings.h */
#define HAVE_STRINGS_H

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

/* Define if we have ieeefp.h */
//FIXME: #cmakedefine HAVE_IEEEFP_H

/* Define if we have float.h */
#define HAVE_FLOAT_H

/* Define if we have fenv.h */
//FIXME: #cmakedefine HAVE_FENV_H

/* Define if we have GCC's __get_cpuid() */
//FIXME: #cmakedefine HAVE_GCC_GET_CPUID

/* Define if we have the __cpuid() intrinsic */
//FIXME: #cmakedefine HAVE_CPUID_INTRINSIC

/* Define if we have the _BitScanForward64() intrinsic */
//FIXME: #cmakedefine HAVE_BITSCANFORWARD64_INTRINSIC

/* Define if we have the _BitScanForward() intrinsic */
//FIXME: #cmakedefine HAVE_BITSCANFORWARD_INTRINSIC

/* Define if we have _controlfp() */
//FIXME: #cmakedefine HAVE__CONTROLFP

/* Define if we have __control87_2() */
//FIXME: #cmakedefine HAVE___CONTROL87_2

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

/* Define if we have pthread_mutexattr_setkind_np() */
//FIXME: #cmakedefine HAVE_PTHREAD_MUTEXATTR_SETKIND_NP

/* Define if we have pthread_mutex_timedlock() */
//FIXME: #cmakedefine HAVE_PTHREAD_MUTEX_TIMEDLOCK


// Some includes (required because some files don't include the right headers?)
#define AL_ALEXT_PROTOTYPES
#include <AL/efx.h>
#include <stdio.h>
