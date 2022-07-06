#include <string>
#include <cstring>
#include <unistd.h>
#include <mutex>

#include <sys/types.h>  // for AF_INET SOCK_STREAM
#include <sys/socket.h> // for socket
#include <netinet/in.h> // for sockaddr_in
#include <arpa/inet.h>  // for inet_pton

#include "base.h"
#include "log.h"
#include "udp-piece.h"
#include "udp.h"

namespace {
    int sockfd;
    struct sockaddr_in toAddr;
    std::mutex dataLock;
    udp_piece_t* udpPiece = nullptr;
}

void InitUdp() {
    LOGI("init udp");
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    toAddr.sin_family = AF_INET;
    toAddr.sin_port = htons(targetPort);
    if (inet_pton(AF_INET, targetIP, &toAddr.sin_addr) <= 0) {
        PLOGE("inet_pton error");
        exit(-1);
    }
    LOGD("init udp successful");
    udpPiece = udp_piece_init(PIC_BUFFER_SIZE);
}

bool SendUdp(void* data, int dataLen) {
    std::lock_guard g(dataLock);

    int pieces = udp_piece_cut(udpPiece, data, dataLen);
    LOGD("pieces = %d", pieces);
    for (int i = 0; i < pieces; i++) {
        int size;
        uint8_t* buf = udp_piece_get(udpPiece, i, &size);
        ssize_t ret = sendto(sockfd, buf, size, 0, (struct sockaddr*) &toAddr, sizeof(toAddr));
        if (ret < 0) {
            PLOGE("sendto error");
            return false;
        }
    }
    return true;
}

void CloseUdp() {
    close(sockfd);
    udp_piece_deinit(udpPiece);
    delete udpPiece;
    udpPiece = nullptr;
    LOGD("closed udp");
}
