/*
 * log function, save to file or stream.
 */

#include <unistd.h>
#include <iostream>
#include <sstream>
#include <string>
#include "api.h"
#include "api.hpp"

//python -c 'import sys, json; print( json.dumps( json.load(sys.stdin) , indent=4) )'

using namespace std;

string get_level_str(int level) {
    switch (level) {
        case AV_LOG_QUIET:
            return "quiet";
        case AV_LOG_DEBUG:
            return "debug";
        case AV_LOG_VERBOSE:
            return "verbose";
        case AV_LOG_INFO:
            return "info";
        case AV_LOG_WARNING:
            return "warning";
        case AV_LOG_ERROR:
            return "error";
        case AV_LOG_FATAL:
            return "fatal";
        case AV_LOG_PANIC:
            return "panic";
//
//        case api_LOG_ERROR:
//            return "error";
//        case api_LOG_WARN:
//            return "warning";
//        case api_LOG_INFO:
//            return "info";
//        case api_LOG_DEBUG:
//            return "debug";

        default:
            return "";
    }
}

string getLocalTimeNow(string fmt = "") {
    time_t timep;
    time(&timep);
    char local_time[64];
    strftime(local_time, sizeof (local_time), fmt.empty() ? "%Y%m%d%H%M%S" : fmt.c_str(), localtime(&timep));

    return string(local_time);
}

void api_log(void *ptr, int level, const char *fmt, va_list vl) {
    int log_level = AV_LOG_ERROR;
//    if (api.logLvl == "debug") {
//        log_level = api_LOG_DEBUG;
//    } else if (api.logLvl == "info") {
//        log_level = api_LOG_INFO;
//    } else if (api.logLvl == "warning") {
//        log_level = api_LOG_WARN;
//    } else if (api.logLvl == "error") {
//        log_level = api_LOG_ERROR;
//    }
//    level = AV_LOG_INFO;
    if (level <= log_level) {
        char str[4096];
        int n = vsnprintf(str, 4096, fmt, vl);
        str[n] = '\0';

        for (int i = 0; i < n; i++) {
            if (str[i] == '\"' || str[i] == '\'' || str[i] == '\r' || str[i] == '\n') {
                str[i] = ' ';
            }
        }

        stringstream sstrLog;
        sstrLog << "{ \"Time\":\"" << getLocalTimeNow() << "\""
                << ", \"Level\":\"" << get_level_str(level) << "\""
                << ", \"Text\":\"" << str << "\" }";

        pthread_mutex_lock(&api.k);
        api.log.push(sstrLog.str());
        pthread_mutex_unlock(&api.k);
    }
}

int uploadLog() {
    int ret = 0;

    if (api.log.size() > 0 && api.logfile.length() > 0) {
        stringstream ss_txt;
        pthread_mutex_lock(&api.k);
        while (api.log.size()) {
            ss_txt << api.log.front() << endl;
            api.log.pop();
        }
        pthread_mutex_unlock(&api.k);

        stringstream ss_header;
        ss_header << "Content-Length: " << ss_txt.str().length() << "\r\n";

        AVDictionary *options = NULL;
        AVIOContext *io_ctx = NULL;
        av_dict_set(&options, "timeout", "5000000", 0);
        av_dict_set(&options, "method", "PUT", 0);
        av_dict_set(&options, "headers", ss_header.str().c_str(), 0);

//        av_log(NULL, api_LOG_DEBUG, "Upload log to [%s]", api.logfile.c_str());

        ret = avio_open2(&io_ctx, api.logfile.c_str(), AVIO_FLAG_WRITE, NULL, &options);
        if (ret >= 0) {
            avio_write(io_ctx, (const unsigned char*) ss_txt.str().c_str(), ss_txt.str().length());
            avio_close(io_ctx);
        } else {
//            av_log(NULL, api_LOG_ERROR, "Upload log error [%s][%d]", api.logfile.c_str(), ret);
            ret = -1;
        }
    }

    return ret;
}
