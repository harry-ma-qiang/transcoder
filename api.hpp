/* 
 * File:   api.hpp
 * Author: harry
 *
 * Created on December 12, 2018, 9:06 PM
 */

#ifndef api_HPP
#define api_HPP

#include <string>
#include <queue>
#include <pthread.h>
#include "picojson.h"
#include "api.h"

using namespace std;

typedef struct api {
    bool isGood;
    bool isLoop;
    bool enable;
    int update;

    string config;
    string status;
    string notify;
    string logfmt;
    string logLvl;
    string logfile;
    string service;
    string version;

    string mediaInfo;
    string printStat;
    string jobStatus;

    pthread_t th_sync;
    pthread_mutex_t k;
    queue<string> log;

    api() : isGood(true), enable(true), config(""), status(""), service(""), version("") {
    }
} API;

extern API api;

int uploadLog();
int updateStatus();
int add_argv(const char*, int*, char***);
int api_parse_options_capture(picojson::value, int*, char***);
int api_parse_options_encode(picojson::value, int*, char***);
int getJSONPositiveInt(picojson::value, string, int);
void *sync(void *ptr);
bool getJSONBool(picojson::value, string, bool = false);
string get_job_info();
string getJSONLowerString(picojson::value, string, string);
picojson::value loadJob(string);

#endif /* api_HPP */
