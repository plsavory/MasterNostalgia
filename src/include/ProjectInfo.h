/*
Mastalgia - a (soon to be) Sega Master System emulator.
Lisenced under the GPLv3 license.
@author: Peter Savory
 */

#define PROJECT_NAME "Mastalgia"
#define PROJECT_VERSION "v0.0.1"


#ifdef __i386__
#define PROJECT_ARCH "-x86"
#elif __amd64__
#define PROJECT_ARCH "-x86-64"
#elif __arm__
#define PROJECT_ARCH "-arm"
#else
#define PROJECT_ARCH "unknown-arch"
#endif

#ifdef __WIN16
#define PROJECT_OS "-win16"
#elif __WIN32
#define PROJECT_OS "-win32"
#elif __WIN64
#define PROJECT_OS "-win64"
#elif __GNU__
#define PROJECT_OS "-gnu-hurd"
#elif __linux__
#define PROJECT_OS "-linux"
#elif __APPLE__ && __MACH__
#define PROJECT_OS "-osx"
#elif __minix
#define PROJECT_OS "-minix"
#elif __FreeBSD__
#define PROJECT_OS "-bsd"
#else
#define PROJECT_OS "-unknown-os"
#endif

#ifdef CURRENT_BRANCH
#define MAKE_STRING2(x) #x
#define MAKE_STRING(x) MAKE_STRING2(x)
#define BRANCH_STRING MAKE_STRING(CURRENT_BRANCH)
#else
#define BRANCH_STRING "unknown"
#endif

#ifdef CURRENT_COMMIT
#define CURRENT_COMMIT_STRING MAKE_STRING(CURRENT_COMMIT)
#else
#define CURRENT_COMMIT_STRING "none"
#endif
