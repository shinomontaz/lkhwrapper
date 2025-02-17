# lkhwrapper
Set of overrides to provide a tsplib95 a convinient functionality for calls to avoid file exchange

## Usage
1. Place lkh code of two level tree into lkh folder
2. Update `GetTime.c` to fit the OS. Actually only for windows you need to uncomment `#undef HAVE_GETRUSAGE`
3. Run Makefile to create a static library. Use it with headers in lkh `INCLUDE` dir