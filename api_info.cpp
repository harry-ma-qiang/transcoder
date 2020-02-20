/*
 * Function to init program
 */

#include <iostream>
#include <unistd.h>
#include <iomanip>
#include <sstream>
#include "api.h"
#include "api.hpp"

using namespace std;

string dump_stream(AVStream* st);
string dump_format(AVFormatContext* ctx);

string dump_stream(AVStream* st) {
    stringstream info;
    info << std::fixed << std::setprecision(2);
    info << "{\"index\":\"" << st->index << "\"";
    if (av_get_media_type_string(st->codecpar->codec_type))
        info << ", \"codec_type\":\"" << av_get_media_type_string(st->codecpar->codec_type) << "\"";
    if (avcodec_get_name(st->codecpar->codec_id))
        info << ", \"codec_name\":\"" << avcodec_get_name(st->codecpar->codec_id) << "\"";
    if (avcodec_profile_name(st->codecpar->codec_id, st->codecpar->profile))
        info << ", \"codec_profile\":\"" << avcodec_profile_name(st->codecpar->codec_id, st->codecpar->profile) << "\"";
    if (st->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
        if (st->codecpar->width > 0) info << ", \"width\":\"" << st->codecpar->width << "\"";
        if (st->codecpar->height > 0) info << ", \"height\":\"" << st->codecpar->height << "\"";
        if (st->sample_aspect_ratio.num && av_cmp_q(st->sample_aspect_ratio, st->codecpar->sample_aspect_ratio)) {
            AVRational display_aspect_ratio;
            av_reduce(&display_aspect_ratio.num, &display_aspect_ratio.den
                    , st->codecpar->width * (int64_t) st->sample_aspect_ratio.num
                    , st->codecpar->height * (int64_t) st->sample_aspect_ratio.den, 1024 * 1024);
            info << ", \"sar\":\"" << st->sample_aspect_ratio.num << ":" << st->sample_aspect_ratio.den << "\"";
            info << ", \"dar\":\"" << display_aspect_ratio.num << ":" << display_aspect_ratio.den << "\"";
        }
        if (st->avg_frame_rate.den > 0 && st->avg_frame_rate.num > 0)
            info << ", \"fps\":\"" << av_q2d(st->avg_frame_rate) << "\"";
        if (av_get_pix_fmt_name((AVPixelFormat) st->codecpar->format))
            info << ", \"color_space\":\"" << av_get_pix_fmt_name((AVPixelFormat) st->codecpar->format) << "\"";
        if (av_get_colorspace_name(st->codecpar->color_space))
            info << ", \"color_range\":\"" << av_get_colorspace_name(st->codecpar->color_space) << "\"";
        if (st->codecpar->field_order != AV_FIELD_UNKNOWN) {
            if (st->codecpar->field_order == AV_FIELD_PROGRESSIVE)
                info << ", \"field-order\":\"Progressive\"";
            else
                info << ", \"field-order\":\"Interlacing\"";
        }

    } else if (st->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
        if (st->codecpar->sample_rate)
            info << ", \"sample_rate\":\"" << st->codecpar->sample_rate << "\"";
        if (st->codecpar->format != AV_SAMPLE_FMT_NONE)
            info << ", \"sample_format\":\"" << av_get_sample_fmt_name((AVSampleFormat) st->codecpar->format) << "\"";

        if (st->codecpar->channels) {
            char buf[128];
            av_get_channel_layout_string(buf, strlen(buf), st->codecpar->channels, st->codecpar->channel_layout);
            info << ", \"channel_number\":\"" << st->codecpar->channels << "\"";
            if (strlen(buf)) info << ", \"channel_layout\":\"" << buf << "\"";
        }
    }

    if (st->codecpar->bit_rate > 0)
        info << ", \"bitrate\":\"" << st->codecpar->bit_rate / 1000 << "kbs\"";
    info << "}";

    return info.str();
}

string dump_format(AVFormatContext* ctx) {
    stringstream info;

    info << std::fixed << std::setprecision(3);
    info << "{\"format\":\"" << (ctx->oformat ? ctx->oformat->name : ctx->iformat->name)
            << "\", \"url\":\"" << ctx->url << "\"";
    if (ctx->duration != AV_NOPTS_VALUE) info << ", \"duration\":\"" << (double) ctx->duration / AV_TIME_BASE << "\"";
    if (ctx->start_time != AV_NOPTS_VALUE) info << ", \"start\":\"" << (double) ctx->start_time / AV_TIME_BASE << "\"";
    if (ctx->bit_rate) info << ", \"bitrate\":\"" << (double) ctx->bit_rate / 1000 << "kbs\"";
    info << ", \"stream\":[";
    for (unsigned int i = 0; i < ctx->nb_streams; i++) {
        info << (i == 0 ? "" : ",") << dump_stream(ctx->streams[i]);
    }
    info << "]}";

    return info.str();
}

int api_init(AVFormatContext* ictx, AVFormatContext* octx) {
    // api.mediaInfo = "{\"input\":" + dump_format(ictx);
    // api.mediaInfo += ", \"output\":" + dump_format(octx) + "}";
	api.mediaInfo = "{}";

    api.jobStatus = "doing";
    updateStatus(api.status);
    pthread_create(&api.th_sync, NULL, sync, NULL);

    return 0;
}

int last_frame_number = 0;
int64_t last_total_size = 0;
int64_t last_status_timestamp = 0;

int api_status(char* info, int64_t pts, int frame_number, int64_t total_size, int64_t i_duratoin, int64_t o_duration) {
    int secs = (FFABS(pts) / AV_TIME_BASE);
    float progress = -1;
    float in_duration = (float) i_duratoin / AV_TIME_BASE;
    float out_duration = (float) o_duration / AV_TIME_BASE;
    if ((out_duration <= 0 || out_duration > in_duration) && in_duration > 0)
        progress = ((float) pts / AV_TIME_BASE) / in_duration;
    else if ((in_duration <= 0 || in_duration > out_duration) && out_duration > 0)
        progress = secs / out_duration;
    if (progress > 1) progress = 1;

    float ifps = 0.0;
    float ibitrate = 0.0;
    int64_t status_timestamp = av_gettime();
    if (last_status_timestamp > 0) {
        ifps = (double) (frame_number - last_frame_number)*1000000.0 / (status_timestamp - last_status_timestamp);
        ibitrate = (double) (total_size - last_total_size)*8.0 * 1000000.0 / 1024 / (status_timestamp - last_status_timestamp);
    }

    last_status_timestamp = status_timestamp;
    last_frame_number = frame_number;
    last_total_size = total_size;

    stringstream sstr;
    sstr.precision(2);
    sstr << "progress=" << std::fixed << progress << endl;
    sstr << "ifps=" << std::fixed << ifps << endl;
    sstr << "ibitrate=" << std::fixed << ibitrate << endl;
    sstr << info << endl;

    char* tmp = strdup(sstr.str().c_str());
    sstr.str("");
    // sstr << "{ ";
    int one_end = 0;
    char * pch = strtok(tmp, "=\n");
    while (pch != NULL) {
        sstr << "\"" << pch << "\"";
        pch = strtok(NULL, "=\n");
        if (pch) {
            if (one_end) sstr << ",";
            else sstr << ":";
            one_end = !one_end;
        }
    }
    // sstr << " }";
    free(tmp);

    api.printStat = sstr.str();

    return 0;
}

time_t last_frame_time = 0;
int api_frame(AVCodecContext* ctx, const AVFrame* frame) {
    return 0;
}

string get_job_info() {
    string info = "\"status\":\"" + api.jobStatus + "\"";
    info += ",\"id\":\"" + api.jobId + "\"";

    // if (api.mediaInfo.length() > 0) {
    //     info += ",\"info\":" + api.mediaInfo;
    // }
    if (api.printStat.length() > 0) {
        // info += ",\"report\":" + api.printStat;
        info += "," + api.printStat;
    }

    return "{" + info + "}";
}
