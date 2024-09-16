#if defined(__linux__) && !defined(_DEFAULT_SOURCE)
#define _DEFAULT_SOURCE
#endif

#include <stdlib.h> // getloadavg
#include <stdio.h>




double fs_cpu_loadavg() {
  // return the average CPU load for last one minute

#if defined(_WIN32)
  fprintf(stderr, "CPU load average not implemented on Windows\n");
  return -0.0; // Not implemented for Windows
#elif defined(HAVE_GETLOADAVG)
  double a[3] = { 0.0, 0.0, 0.0 };

  if (getloadavg(a, 3) < 0)
    return -0.0;

  return a[0];
#else
    fprintf(stderr, "CPU load average not implemented on this platform\n");
    return -0.0; // Not implemented for other platforms
#endif
}
