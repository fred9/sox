/* Implements the public API for libSoX general functions
 * All public functions & data are prefixed with sox_ .
 *
 * (c) 2006-8 Chris Bagwell and SoX contributors
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or (at
 * your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "sox_i.h"
#include <string.h>

const char *sox_version(void)
{
  static char versionstr[20];

  sprintf(versionstr, "%d.%d.%d",
          (SOX_LIB_VERSION_CODE & 0xff0000) >> 16,
          (SOX_LIB_VERSION_CODE & 0x00ff00) >> 8,
          (SOX_LIB_VERSION_CODE & 0x0000ff));
  return(versionstr);
}

sox_version_info_t const * sox_version_info(void)
{
#define STRINGIZE1(x) #x
#define STRINGIZE(x) STRINGIZE1(x)
    static char arch[30];
    static sox_version_info_t info = {
        /* size */
        sizeof(sox_version_info_t),
        /* flags */
        (sox_version_flags_t)(
#if HAVE_POPEN
        sox_version_have_popen +
#endif
#if  HAVE_MAGIC
        sox_version_have_magic +
#endif
#if HAVE_OPENMP
        sox_version_have_threads +
#endif
#ifdef HAVE_FMEMOPEN
        sox_version_have_memopen +
#endif
        sox_version_none),
        /* version_code */
        SOX_LIB_VERSION_CODE,
        /* version */
        NULL,
        /* sox_version_extra */
#ifdef PACKAGE_EXTRA
        PACKAGE_EXTRA,
#else
        NULL,
#endif
        /* sox_time */
        __DATE__ " " __TIME__,
        /* sox_distro */
#ifdef DISTRO
        DISTRO,
#else
        NULL,
#endif
        /* sox_compiler */
#if defined __GNUC__
        "gcc " __VERSION__,
#elif defined _MSC_VER
        "msvc " STRINGIZE(_MSC_FULL_VER),
#elif defined __SUNPRO_C
    fprintf(file, "sun c " STRINGIZE(__SUNPRO_C),
#else
        NULL,
#endif
        /* sox_arch */
        NULL
    };

    if (!info.version)
    {
        info.version = sox_version();
    }

    if (!info.arch)
    {
        snprintf(arch, sizeof(arch), "%u%u%u%u %u%u %u%u %c %s",
            (unsigned)sizeof(char), (unsigned)sizeof(short),
            (unsigned)sizeof(long), (unsigned)sizeof(off_t),
            (unsigned)sizeof(float), (unsigned)sizeof(double),
            (unsigned)sizeof(int *), (unsigned)sizeof(int (*)(void)),
            MACHINE_IS_BIGENDIAN ? 'B' : 'L',
            (info.flags & sox_version_have_threads) ? "OMP" : "");
        arch[sizeof(arch) - 1] = 0;
        info.arch = arch;
    }

    return &info;
}

/* Default routine to output messages; can be overridden */
static void output_message(
    unsigned level, const char *filename, const char *fmt, va_list ap)
{
  if (sox_globals.verbosity >= level) {
    sox_output_message(stderr, filename, fmt, ap);
    fprintf(stderr, "\n");
  }
}

sox_globals_t sox_globals = {
  2,               /* unsigned     verbosity */
  output_message,  /* sox_output_message_handler */
  sox_false,       /* sox_bool     repeatable */
  8192,            /* size_t       bufsiz */
  0,               /* size_t       input_bufsiz */
  0,               /* int32_t      ranqd1 */
  NULL,            /* char const * stdin_in_use_by */
  NULL,            /* char const * stdout_in_use_by */
  NULL,            /* char const * subsystem */
  NULL,            /* char       * tmp_path */
  sox_false,       /* sox_bool     use_magic */
  sox_false        /* sox_bool     use_threads */
};

char const * sox_strerror(int sox_errno)
{
  static char const * const errors[] = {
    "Invalid Audio Header",
    "Unsupported data format",
    "Can't allocate memory",
    "Operation not permitted",
    "Operation not supported",
    "Invalid argument",
  };
  if (sox_errno < SOX_EHDR)
    return strerror(sox_errno);
  sox_errno -= SOX_EHDR;
  if (sox_errno < 0 || (size_t)sox_errno >= array_length(errors))
    return "Unknown error";
  return errors[sox_errno];
}

void sox_output_message(FILE *file, const char *filename, const char *fmt, va_list ap)
{
  char const * slash_pos = LAST_SLASH(filename);
  char const * base_name = slash_pos? slash_pos + 1 : filename;
  char const * dot_pos   = strrchr(base_name, '.');
  fprintf(file, "%.*s: ", dot_pos? (int)(dot_pos - base_name) : -1, base_name);
  vfprintf(file, fmt, ap);
}

#undef lsx_fail
#undef lsx_warn
#undef lsx_report
#undef lsx_debug
#undef lsx_debug_more
#undef lsx_debug_most

#define SOX_MESSAGE_FUNCTION(name,level) \
void name(char const * fmt, ...) { \
  va_list ap; \
  va_start(ap, fmt); \
  if (sox_globals.output_message_handler) \
    (*sox_globals.output_message_handler)(level,sox_globals.subsystem,fmt,ap); \
  va_end(ap); \
}

SOX_MESSAGE_FUNCTION(lsx_fail  , 1)
SOX_MESSAGE_FUNCTION(lsx_warn  , 2)
SOX_MESSAGE_FUNCTION(lsx_report, 3)
SOX_MESSAGE_FUNCTION(lsx_debug , 4)
SOX_MESSAGE_FUNCTION(lsx_debug_more , 5)
SOX_MESSAGE_FUNCTION(lsx_debug_most , 6)

#undef SOX_MESSAGE_FUNCTION

int sox_init(void)
{
  return lsx_effects_init();
}

int sox_quit(void)
{
  sox_format_quit();
  return lsx_effects_quit();
}
