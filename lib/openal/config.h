#ifndef CONFIG_H
#define CONFIG_H

/* Define to the library version */
#define ALSOFT_VERSION "1.6.372"

/* Define if we have the ALSA backend */
//FIXME: #define HAVE_ALSA

/* Define if we have the OSS backend */
//FIXME: #define HAVE_OSS

/* Define if we have the Solaris backend */
/* #undef HAVE_SOLARIS */

/* Define if we have the DSound backend */
/* #undef HAVE_DSOUND */

/* Define if we have the Windows Multimedia backend */
/* #undef HAVE_WINMM */

/* Define if we have dlfcn.h */
//FIXME: #define HAVE_DLFCN_H

/* Define if we have the sqrtf function */
//FIXME: #define HAVE_SQRTF

/* Define if we have the acosf function */
//FIXME: #define HAVE_ACOSF

/* Define if we have the strtof function */
//FIXME: #define HAVE_STRTOF

/* Define if we have stdint.h */
#define HAVE_STDINT_H

/* Define if we have the __int64 type */
/* #undef HAVE___INT64 */

/* Define to the size of a long int type */
#define SIZEOF_LONG 4

/* Define to the size of a long long int type */
#define SIZEOF_LONG_LONG 8

/* Define to the size of an unsigned int type */
#define SIZEOF_UINT 4

/* Define to the size of a void pointer type */
#define SIZEOF_VOIDP 4

/* Define if we have GCC's destructor attribute */
#define HAVE_GCC_DESTRUCTOR

/* Define if we have pthread_np.h */
/* #undef HAVE_PTHREAD_NP_H */

/* Define if we have float.h */
//FIXME: #define HAVE_FLOAT_H

/* Define if we have fenv.h */
//FIXME: #define HAVE_FENV_H

/* Define if we have fesetround() */
//FIXME: #define HAVE_FESETROUND

/* Define if we have _controlfp() */
/* #undef HAVE__CONTROLFP */

//FIXME: We don't expose errno properly in nxdk
#include <errno.h>
//FIXME: Where do these come from? do they evaluate arguments twice?
#include <stdlib.h>
#define min __min
#define max __max

#endif
