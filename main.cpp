#include <cstring>
#include <iostream>
#include <unistd.h>

#include "base.h"
#include "log.h"
#include "mycamera.h"
#include "udp.h"

void InitMain() {
    FILE* fp = fopen(kConfigFile, "r");
    if (fp) {
        fscanf(fp, "targetIP=%s\n", targetIP);
        fscanf(fp, "targetPort=%d\n", &targetPort);
        fclose(fp);
    }
    LOGI("targetIP = %s", targetIP);
    LOGI("targetPort = %d", targetPort);
    if (int child = fork()) {
        LOGD("fork, child = %d", child);
        sleep(10);
    } else {
        int r = execlp("/bin/wpa_supplicant", "/bin/wpa_supplicant", "-i", "wlan0", "-c", "/etc/wpa_supplicant.conf", nullptr);
        if (r) PLOGE("execlp failed");
        exit(-1);
    }
}

[[noreturn]]
void EventLoop() {
    while (true) {
        Capture();
        static const struct timespec t {
            .tv_sec = 0,
            .tv_nsec = 500 * 1000
        };
        nanosleep(&t, nullptr);
    }
}

int main() {
    InitMain();
    InitUdp();
    InitCamera();
    EventLoop();
}
