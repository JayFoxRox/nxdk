//
//	Debug output scrolling code submitted by Robin Mulloy
//
//
#ifndef HAL_DEBUG_H
#define HAL_DEBUG_H

#if defined(__cplusplus)
extern "C"
{
#endif

/**
 * Prints a message to whatever debug facilities might
 * be available.
 */
void debugPrint(const char *format, ...);
void debugPrintNum(int i);
void debugPrintBinary( int num );
void debugPrintHex(const char *buffer, int length);
void debugClearScreen( void );
void advanceScreen( void );


#ifdef __cplusplus
}
#endif

#endif
