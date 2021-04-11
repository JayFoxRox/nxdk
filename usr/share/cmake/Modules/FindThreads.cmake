# xboxkrnl and pdclib have C11 thread support out of the box
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(Threads_FOUND TRUE)
add_library(Threads::Threads INTERFACE IMPORTED)
