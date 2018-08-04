#warning longjmp only supports constant 1 as second argument

#define longjmp(x, y) __builtin_longjmp(x, 1)
#define setjmp(x) __builtin_setjmp(x)

typedef void** jmp_buf;
