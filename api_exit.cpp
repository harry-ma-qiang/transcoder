/*
 * Exit callba
 */

#include <iostream>
#include <string>
#include <unistd.h>
#include "api.h"
#include "api.hpp"

using namespace std;

void *sync(void *ptr) {
    time_t now = time(NULL);
    while (api.isGood) {
        usleep(10000);
        if (time(NULL) - now >= api.update) {
            if (!api.config.empty()) {
                loadJob(api.config);
            }

            if (!api.status.empty()) {
                updateStatus();
            }

            if (api.log.size() > 0) {
                uploadLog();
            }

            now = time(NULL);
        }
    }
//    av_log(NULL, api_LOG_DEBUG, "Sync thread ended");

    return NULL;
}

int api_exit(int ret, int received_sigterm, int init_done, int main_ret) {
    api.isGood = false;

    if (received_sigterm) {
        api.jobStatus = "abort";
    } else if (ret && init_done) {
        api.jobStatus = "error";
    } else if (ret == 0 && main_ret == 0) {
        api.jobStatus = "ended";
    } else if (!init_done && api.enable) {
        api.jobStatus = "error";
    } else if (!init_done && !api.enable) {
        api.jobStatus = "pause";
    }

    pthread_join(api.th_sync, NULL);
    
//    av_log(NULL, api_LOG_DEBUG, "Job Done");
    updateStatus();
    uploadLog();
    
    pthread_mutex_destroy(&api.k);

    return ret;
}
