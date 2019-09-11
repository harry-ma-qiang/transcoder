/*
 * parse JSON object and generate FFmpeg argument
 */

#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include "api.h"
#include "api.hpp"

using namespace std;

int api_parse_options_capture(picojson::value v, int* p_argc, char*** p_argv) {
    int r = -1;

    string input = getJSONLowerString(v, "input", "");
    if (input.empty()) {
        input = "DeckLink SDI Micro";
    }

    add_argv("-f", p_argc, p_argv);
    add_argv("decklink", p_argc, p_argv);

    add_argv("-raw_format", p_argc, p_argv);
    add_argv("uyvy422", p_argc, p_argv);

    add_argv("-audio_input", p_argc, p_argv);
    add_argv("embedded", p_argc, p_argv);

    add_argv("-channels", p_argc, p_argv);
    add_argv("2", p_argc, p_argv);

    add_argv("-channel_layout", p_argc, p_argv);
    add_argv("stereo", p_argc, p_argv);

    add_argv("-i", p_argc, p_argv);
    add_argv(input.c_str(), p_argc, p_argv);

    string filter = "format=yuv420p,pp=lb";
    string videoSize = getJSONLowerString(v, "videoSize", "");
    if (!videoSize.empty()) {
        filter += ",scale=" + videoSize;
    }

    add_argv("-vf", p_argc, p_argv);
    add_argv(filter.c_str(), p_argc, p_argv);

    add_argv("-c:v", p_argc, p_argv);
    add_argv("api", p_argc, p_argv);

    string videoBitrate = getJSONLowerString(v, "videoBitrate", "");
    if (videoBitrate.empty()) {
        videoBitrate = "4000";
    }

    string videoBuffer = getJSONLowerString(v, "videoBuffer", "");
    if (videoBuffer.empty()) {
        videoBuffer = videoBitrate;
    }

    string videoKeyframeInterval = getJSONLowerString(v, "videoKeyframeInterval", "");
    if (videoKeyframeInterval.empty()) {
        videoKeyframeInterval = "75";
    }

    stringstream x264params;
    x264params << "vbv-maxrate=" << videoBitrate
            << ":vbv-bufsize=" << videoBuffer
            << ":bitrate=" << videoBitrate
            << ":keyint=" << videoKeyframeInterval
            << ":qcomp=0.5"
            << ":aq_mode=1"
            << ":mbtree=1"
            << ":ref=2"
            << ":sliced-threads=0"
            << ":threads=8"
            << ":merange=16"
            << ":rc-lookahead=25"
            << ":me=hex"
            << ":subme=6"
            << ":bframes=0"
            << ":aq_strength=1"
            << ":weightp=0"
            << ":scenecut=40"
            << ":ipratio=0.71"
            << ":qpstep=7"
            << ":qpmax=69"
            << ":qpmin=0"
            << ":deblock=0,0"
            << ":weightb=0"
            << ":b-pyramid=1"
            << ":no-open-gop=1"
            << ":b-adapt=0"
            << ":psy-rd=1"
            << ":level=4.1";

    add_argv("-x264-params", p_argc, p_argv);
    add_argv(x264params.str().c_str(), p_argc, p_argv);

    add_argv("-profile:v", p_argc, p_argv);
    add_argv("high", p_argc, p_argv);

    add_argv("-level:v", p_argc, p_argv);
    add_argv("4.2", p_argc, p_argv);

    add_argv("-c:a", p_argc, p_argv);
    add_argv("libfdk_aac", p_argc, p_argv);

    add_argv("-b:a", p_argc, p_argv);
    add_argv("196k", p_argc, p_argv);

    add_argv("-hide_banner", p_argc, p_argv);
    add_argv("-xerror", p_argc, p_argv);
    add_argv("-y", p_argc, p_argv);

    add_argv("-fflags", p_argc, p_argv);
    add_argv("bitexact", p_argc, p_argv);

    add_argv("-bsf:v", p_argc, p_argv);
    add_argv("h264_metadata=del_user_data=1", p_argc, p_argv);

    string output = getJSONLowerString(v, "output", "");
    if (output.empty()) {
        av_log(NULL, AV_LOG_ERROR, "output is required\n");
        return r;
    }

    string format = getJSONLowerString(v, "format", "");
    if (format.empty()) {
        if (output.rfind("rtmp") == 0) {
            format = "flv";
        } else {
            format = "mpegts";
        }
    }

    add_argv("-f", p_argc, p_argv);
    add_argv(format.c_str(), p_argc, p_argv);

    if (output.find("udp://")==0) {
        output += "?reuse=1";
    }
    
    add_argv(output.c_str(), p_argc, p_argv);

    r = 0;

    return r;
}
