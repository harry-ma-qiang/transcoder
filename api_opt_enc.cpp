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

int api_parse_options_encode(picojson::value v, int* p_argc, char*** p_argv) {
    int r = -1;

    string input = getJSONLowerString(v, "input", "");
    if (input.empty()) {
        av_log(NULL, AV_LOG_ERROR, "input is required\n");
        return r;
    }
    
    if (input.find("udp://")==0) {
        input += "?reuse=1";
    }

    string output = getJSONLowerString(v, "output", "");
    if (output.empty()) {
        av_log(NULL, AV_LOG_ERROR, "output is required\n");
        return r;
    }

    string format = getJSONLowerString(v, "format", "");
    if (format.empty()) {
        //TODO: add more code to guess correct format.
        if (output.rfind("rtmp") == 0) {
            format = "flv";
        } else {
            format = "mpegts";
        }
    }

    // add_argv("-copyts", p_argc, p_argv);
    // add_argv("-start_at_zero", p_argc, p_argv);
    
    if (input.rfind("udp")==0 || input.rfind("rtmp")==0 || input.rfind("zmq")==0 || input.rfind("http")==0) {
        add_argv("-timeout", p_argc, p_argv);
        add_argv("5000000", p_argc, p_argv);
    }

    add_argv("-i", p_argc, p_argv);
    add_argv(input.c_str(), p_argc, p_argv);

    string videoBitrate = getJSONLowerString(v, "videoBitrate", "");
    if (videoBitrate.empty()) {
        videoBitrate = "6000";
    }
    
    string videoMaxrate = getJSONLowerString(v, "videoMaxrate", "");
    if (videoMaxrate.empty()) {
        videoMaxrate = videoBitrate;
    }
    
    string videoBuffer = getJSONLowerString(v, "videoBuffer", "");
    if (videoBuffer.empty()) {
        videoBuffer = videoBitrate;
    }
    
    string audioBitrate = getJSONLowerString(v, "audioBitrate", "");
    if (audioBitrate.empty()) {
        audioBitrate = "96k";
    }

    string videoKeyframeInterval = getJSONLowerString(v, "videoKeyframeInterval", "");
    if (videoKeyframeInterval.empty()) {
        videoKeyframeInterval = "25";
    }

    string videoCodec = getJSONLowerString(v, "videoCodec", "");
    if (videoCodec.empty()) {
        videoCodec = "h264";
    }
    
    string videoQP = getJSONLowerString(v, "videoQP", "");
    if (videoQP.empty()) {
        videoQP = "24";
    }

    stringstream xparams;
    xparams << "vbv-maxrate=" << videoMaxrate
            << ":vbv-bufsize=" << videoBuffer
            << ":bitrate=" << videoBitrate
            << ":keyint=" << videoKeyframeInterval;

    if (videoCodec == "h264") {
        add_argv("-c:v", p_argc, p_argv);
        add_argv("libx264", p_argc, p_argv);

        xparams << ":qcomp=0.5"
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
                << ":psy-rd=1";

        add_argv("-x264-params", p_argc, p_argv);
        add_argv(xparams.str().c_str(), p_argc, p_argv);

        // add_argv("-bsf:v", p_argc, p_argv);
        // add_argv("h264_metadata=del_user_data=1", p_argc, p_argv);
    } else if (videoCodec == "hevc") {
        add_argv("-c:v", p_argc, p_argv);
        add_argv("libx265", p_argc, p_argv);

        xparams << ":qcomp=0.5"
                << ":aq_mode=1"
                << ":cutree=1"
                << ":ref=2"
                << ":wpp=1"
                << ":frame-threads=8"
                << ":merange=16"
                << ":rc-lookahead=12"
                << ":ctu=16"
                << ":qg-size=16"
                << ":max-tu-size=16"
                << ":me=hex"
                << ":subme=1"
                << ":bframes=0"
                << ":no-allow-non-conformance=1"
                << ":aq_strength=1"
                << ":weightp=0"
                << ":weightb=0"
                << ":scenecut=40"
                << ":ipratio=0.71"
                << ":qpstep=7"
                << ":qpmax=69"
                << ":qpmin=0"
                << ":deblock=0,0"
                << ":b-pyramid=1"
                << ":no-open-gop=1"
                << ":b-adapt=0"
                << ":psy-rd=1"
                << ":rd=1"
                << ":early-skip=1"
                << ":rskip=1"
                << ":tskip-fast=1"
                << ":max-merge=1"
                << ":info=0";

        add_argv("-x265-params", p_argc, p_argv);
        add_argv(xparams.str().c_str(), p_argc, p_argv);
    } else if (videoCodec == "copy") {
        add_argv("-c:v", p_argc, p_argv);
        add_argv("copy", p_argc, p_argv);

        if (format == "mpegts") {
            add_argv("-bsf:v", p_argc, p_argv);
            add_argv("h264_mp4toannexb", p_argc, p_argv);
        }
    } else {
        av_log(NULL, AV_LOG_ERROR, "only h264, hevc video codec is supported.");
        return r;
    }

    if (videoCodec != "copy") {
        string filter = "format=yuv420p,pp=lb";
        string videoSize = getJSONLowerString(v, "videoSize", "");
        if (!videoSize.empty()) {
            filter += ",scale=" + videoSize;
        }
        string videoFPS = getJSONLowerString(v, "videoFPS", "");
        if (!videoFPS.empty()) {
            filter += ",fps=" + videoFPS;
        }

        add_argv("-vf", p_argc, p_argv);
        add_argv(filter.c_str(), p_argc, p_argv);
    }

    string audioCodec = getJSONLowerString(v, "audioCodec", "");
    if (audioCodec.empty() || audioCodec == "aac") {
        audioCodec = "aac";
    }

    add_argv("-c:a", p_argc, p_argv);
    add_argv(audioCodec.c_str(), p_argc, p_argv);

    if (audioCodec != "copy") {
        add_argv("-b:a", p_argc, p_argv);
        add_argv(audioBitrate.c_str(), p_argc, p_argv);
    } else {
        if (format == "mp4" || format == "flv") {
            add_argv("-bsf:a", p_argc, p_argv);
            add_argv("aac_adtstoasc", p_argc, p_argv);
        }
    }

    // add_argv("-copyts", p_argc, p_argv);
    // add_argv("-start_at_zero", p_argc, p_argv);

    add_argv("-hide_banner", p_argc, p_argv);
    add_argv("-xerror", p_argc, p_argv);
    add_argv("-y", p_argc, p_argv);

    // add_argv("-fflags", p_argc, p_argv);
    // add_argv("bitexact", p_argc, p_argv);
    
    add_argv("-f", p_argc, p_argv);
    add_argv(format.c_str(), p_argc, p_argv);

    if (format == "hls") {
        add_argv("-hls_list_size", p_argc, p_argv);
        add_argv(getJSONLowerString(v, "hls_list_size", "5").c_str(), p_argc, p_argv);

        add_argv("-hls_time", p_argc, p_argv);
        add_argv(getJSONLowerString(v, "hls_segment_duration", "3").c_str(), p_argc, p_argv);
        
        // add_argv("-hls_callback", p_argc, p_argv);
        // add_argv(getJSONLowerString(v, "hls_callback", "cb.bash").c_str(), p_argc, p_argv);

//        add_argv("-hls_playlist_type", p_argc, p_argv);
//        add_argv("event", p_argc, p_argv);

        add_argv("-hls_flags", p_argc, p_argv);
        add_argv("second_level_segment_index+second_level_segment_size+second_level_segment_duration", p_argc, p_argv);

        add_argv("-strftime", p_argc, p_argv);
        add_argv("1", p_argc, p_argv);

        add_argv("-strftime_mkdir", p_argc, p_argv);
        add_argv("1", p_argc, p_argv);

        add_argv("-hls_segment_filename", p_argc, p_argv);
        add_argv("%Y%m%d%H%M%S_%%04d_%%08s_%%013t.ts", p_argc, p_argv);
    }

    if (output.find("udp://")==0) {
        output += "?reuse=1";
    }
    
    add_argv(output.c_str(), p_argc, p_argv);
	
	for (int i=0; i<*p_argc; i++) {
		cerr << (*p_argv)[i] << " ";
	}
	cerr << endl;

    r = 0;

    return r;
}
