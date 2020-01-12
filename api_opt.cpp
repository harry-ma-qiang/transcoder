/*
 * Read Job file
 */

#include <iostream>
#include <string>
#include <sstream>
#include <unistd.h>
#include "api.h"
#include "api.hpp"

using namespace std;

string getJSONLowerString(picojson::value v, string key, string defaultValue) {
    string value = v.get(key).to_str();

    // transform(value.begin(), value.end(), value.begin(), ::tolower);
    if (value.empty() || value == "null") {
        value = defaultValue;
    }

    return value;
}

int getJSONPositiveInt(picojson::value v, string key, int defaultValue) {
    string value = v.get(key).to_str();
    int r = atoi(value.c_str());

    if (value != "0" && r <= 0) {
        r = defaultValue;
    }

    return r;
}

bool getJSONBool(picojson::value v, string key, bool defaultValue) {
    bool r = defaultValue;
    string value = v.get(key).to_str();

    transform(value.begin(), value.end(), value.begin(), ::tolower);
    if (value == "true" || value == "1") {
        r = true;
    } else if (value == "false" || value == "0") {
        r = false;
    }

    return r;
}

picojson::value loadJob(string url) {
    picojson::value v;
    AVDictionary *options = NULL;
    AVIOContext *io_ctx = NULL;
    
    av_dict_set(&options, "timeout", "10000000", 0); //10 seconds
    av_dict_set(&options, "reconnect_delay_max", "10", 0);
    av_dict_set(&options, "reconnect", "1", 0);

    int r = avio_open2(&io_ctx, url.c_str(), AVIO_FLAG_READ, NULL, &options);

    if (r >= 0) {
        const int bufsize = 1024 * 1024;
        unsigned char buf[bufsize];
        avio_read(io_ctx, buf, bufsize);
        avio_close(io_ctx);

        string err = picojson::parse(v, string((const char*) buf));
        if (err.empty()) {
            string last_version = api.version;

            api.enable = getJSONBool(v, "enable", true);
            api.config = getJSONLowerString(v, "config", "");
            api.status = getJSONLowerString(v, "status", "");
            api.update = getJSONPositiveInt(v, "updateTimeInterval", 1);
            api.logLvl = getJSONLowerString(v, "log_level", "");
            api.logfmt = getJSONLowerString(v, "log_format", "");
            api.logfile = getJSONLowerString(v, "log_file", "");
            api.service = getJSONLowerString(v, "service", "");
            api.version = getJSONLowerString(v, "version", "");
            api.jobId = getJSONLowerString(v, "jobId", "");
			
            if (!api.enable || (!last_version.empty() && last_version != api.version)) {
				av_log(NULL, AV_LOG_ERROR, "Job is canceled\n");
                api.isGood = false;
            }
        } else {
            av_log(NULL, AV_LOG_ERROR, "Parse job error [%s]\n", buf);
			api.isGood = false;
        }
    } else {
        av_log(NULL, AV_LOG_ERROR, "Fetch job error [%s]", url.c_str());
		api.isGood = false;
    }

    return v;
}

int updateStatus(string url) {
    int ret = 0;
	
	if (api.jobId == "") {
		return ret;
	}
	
    string info = get_job_info();
    stringstream sstr;
    sstr << "Content-Length: " << info.length() << "\r\n";

    AVDictionary *options = NULL;
    AVIOContext *io_ctx = NULL;
    av_dict_set(&options, "timeout", "5000000", 0);
    av_dict_set(&options, "method", "POST", 0);
    av_dict_set(&options, "headers", sstr.str().c_str(), 0);

    av_log(NULL, AV_LOG_DEBUG, "Update status to [%s] [%s]", url.c_str(), info.c_str());
	cerr << "updateStatus: " << url << ":  " << info << endl;
    ret = avio_open2(&io_ctx, url.c_str(), AVIO_FLAG_WRITE, NULL, &options);
    if (ret >= 0) {
        avio_write(io_ctx, (const unsigned char*) info.c_str(), info.length());
        avio_close(io_ctx);
    } else {
        av_log(NULL, AV_LOG_ERROR, "Update status error [%s][%d]", url.c_str(), ret);
        ret = -1;
    }

    return ret;
}

int add_argv(const char* val, int* p_argc, char*** p_argv) {
    const int MAX_NUM_ARG = 1024;

    if ((*p_argc) >= MAX_NUM_ARG - 1) {
        return -1;
    }

    if ((*p_argc) == 0) {
        (*p_argv) = (char**) malloc(sizeof (char*) * MAX_NUM_ARG);
    }

    (*p_argv)[(*p_argc)] = (char*) malloc(sizeof (char) * MAX_NUM_ARG);
    strncpy((*p_argv)[(*p_argc)], val, MAX_NUM_ARG);
    (*p_argv)[(*p_argc)][strlen(val)] = '\0';
    (*p_argc)++;

    (*p_argv)[(*p_argc)] = NULL;

    return 0;
}

int api_start(int* p_argc, char*** p_argv) {
	api.enable = api.isGood = 1;
	
    if (getenv("DEBUG_FF_MODE")) return 0;
    int r = -1;
	
	if (getenv("DEBUG")) {
		api.isDebug = 1;
	}

    // pthread_mutex_init(&api.k, NULL);
    av_log_set_callback(api_log);

    if (p_argv && (*p_argv)[1]) {
        picojson::value v = loadJob(string((*p_argv)[1]));
        if (api.isGood) {
            (*p_argc) = 0;
            add_argv("api", p_argc, p_argv);

            if (api.service == "capture") {
                r = api_parse_options_capture(v, p_argc, p_argv);
            } else if (api.service == "encode") {
                r = api_parse_options_encode(v, p_argc, p_argv);
            }
            // api.jobStatus = "start";
            // updateStatus(api.config + api.jobId);
            // uploadLog();
        }
    } else {
        av_log(NULL, AV_LOG_ERROR, "Usage: api-1.5 job_url");
    }

    return r;
}
