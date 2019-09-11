/* 
 * File:   api.h
 * Author: harry
 *
 * Created on December 12, 2018, 10:50 AM
 */

#ifndef api_H
#define api_H

#ifdef __cplusplus
extern "C" {
#endif

#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavcodec/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavutil/log.h"
#include "libavutil/pixdesc.h"
#include "libavutil/time.h"

    int api_start(int*, char***);
    int api_exit(int, int, int, int);
    int api_decode_interrupt_cb(void);
    int api_status(char*, int64_t, int, int64_t, int64_t, int64_t);
    int api_frame(AVCodecContext*, const AVFrame*);
    int api_init(AVFormatContext*, AVFormatContext*);
    void api_log(void*, int, const char*, va_list);

#ifdef __cplusplus
}
#endif

#endif /* api_H */

