/* 
 * File:   api.cpp
 * Author: harry
 *
 * Created on December 12, 2018, 10:50 AM
 */

#include <signal.h>
#include "api.h"
#include "api.hpp"

using namespace std;

API api;

static int raise_term = 0;

int api_decode_interrupt_cb(void) {
    if (!api.isGood) {
        if (raise_term == 0) {
            raise(SIGTERM);
        }
        raise_term++;

        return 1;
    }

    return 0;
}
