/*
 *  Copyright (c) 2014 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <stdlib.h>
#include <string.h>

#include "./ivfdec.h"
#include "./video_stream_reader.h"

#include "vpx_ports/mem_ops.h"

static const char *const kIVFSignature = "DKIF";

struct VpxVideoStreamReaderStruct {
  VpxVideoInfo info;
  uint8_t *buffer;
  size_t buffer_size;
  size_t frame_size;
};

VpxVideoStreamReader *vpx_video_stream_reader_open(const uint8_t *header) {
  VpxVideoStreamReader *reader = NULL;

  if (memcmp(kIVFSignature, header, 4) != 0)
    return NULL;  // Wrong IVF signature

  if (mem_get_le16(header + 4) != 0)
    return NULL;  // Wrong IVF version

  reader = calloc(1, sizeof(*reader));
  if (!reader)
    return NULL;  // Can't allocate VpxVideoStreamReader

  reader->info.codec_fourcc = mem_get_le32(header + 8);
  reader->info.frame_width = mem_get_le16(header + 12);
  reader->info.frame_height = mem_get_le16(header + 14);
  reader->info.time_base.numerator = mem_get_le32(header + 16);
  reader->info.time_base.denominator = mem_get_le32(header + 20);

  return reader;
}

void vpx_video_stream_reader_close(VpxVideoStreamReader *reader) {
  if (reader) {
    free(reader->buffer);
    free(reader);
  }
}

int vpx_video_stream_reader_read_frame(const uint8_t *buf, uint8_t buf_size,
  VpxVideoStreamReader *reader) {
  return !ivf_read_stream_frame(buf, buf_size, &reader->buffer,
                                &reader->frame_size, &reader->buffer_size);
}

const uint8_t *vpx_video_stream_reader_get_frame(VpxVideoStreamReader *reader,
                                          size_t *size) {
  if (size)
    *size = reader->frame_size;

  return reader->buffer;
}

const VpxVideoInfo *vpx_video_stream_reader_get_info(VpxVideoStreamReader *reader) {
  return &reader->info;
}
