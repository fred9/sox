/*
 * File format: raw IMA ADPCM           (c) 2007-8 SoX contributors
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
#include "vox.h"

SOX_FORMAT_HANDLER(ima)
{
  static char const * const names[] = {"ima", NULL};
  static unsigned const write_encodings[] = {SOX_ENCODING_IMA_ADPCM, 4, 0, 0};
  static sox_format_handler_t handler = {
    SOX_LIB_VERSION_CODE,
    "Raw IMA ADPCM",
    names, SOX_FILE_MONO,
    sox_ima_start, sox_vox_read, sox_vox_stopread,
    sox_ima_start, sox_vox_write, sox_vox_stopwrite,
    lsx_rawseek, write_encodings, NULL
  };
  return &handler;
}
