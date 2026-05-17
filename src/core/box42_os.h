#ifndef BOX42_OS_H
#define BOX42_OS_H

/* OS detection */

#if defined(_WIN32) || defined(_WIN64)
  #define BOX42_OS_WINDOWS 1
#endif

#if defined(__linux__)
  #define BOX42_OS_LINUX 1
#endif

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
  #define BOX42_OS_BSD 1
#endif

#if defined(__APPLE__) && defined(__MACH__)
  #define BOX42_OS_MAC 1
#endif

#if defined(__amigaos__) || defined(AMIGA) || defined(__AMIGA__)
  #define BOX42_OS_AMIGA 1
#endif

/* Fallback: generic POSIX */
#if !defined(BOX42_OS_WINDOWS) && \
    !defined(BOX42_OS_LINUX)   && \
    !defined(BOX42_OS_BSD)     && \
    !defined(BOX42_OS_MAC)     && \
    !defined(BOX42_OS_AMIGA)
  #define BOX42_OS_POSIX 1
#endif

#endif /* BOX42_OS_H */
