//----------------------------------------------------------------------------------------
//
// a single-threaded, multi client(using select), debug webserver - streaming out mjpg.
//
//----------------------------------------------------------------------------------------
#include <cstdio>
#include <vector>
#include <iostream>
#include <algorithm>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <ctime>
#include <opencv2/opencv.hpp>
//
// socket related abstractions:
//
#include "darkunistd.h"
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR   -1
#endif
#include "MJPG_sender.h"

using std::cerr;
using std::endl;

//----------------------------------------------------------------------------------------
struct _IGNORE_PIPE_SIGNAL
{
    struct sigaction new_actn, old_actn;
    _IGNORE_PIPE_SIGNAL() {
        new_actn.sa_handler = SIG_IGN;  // ignore the broken pipe signal
        sigemptyset(&new_actn.sa_mask);
        new_actn.sa_flags = 0;
        sigaction(SIGPIPE, &new_actn, &old_actn);
        // sigaction (SIGPIPE, &old_actn, NULL); // - to restore the previous signal handling
    }
} _init_once;
//----------------------------------------------------------------------------------------
static int close_socket(SOCKET s) {
    int close_output = ::shutdown(s, 1); // 0 close input, 1 close output, 2 close both
    char *buf = (char *)calloc(1024, sizeof(char));
    ::recv(s, buf, 1024, 0);
    free(buf);
    int close_input = ::shutdown(s, 0);
    int result = close(s);
    std::cerr << "Close socket: out = " << close_output << ", in = " << close_input << " \n";
    return result;
}
//----------------------------------------------------------------------------------------
// MJPG_sender class
//----------------------------------------------------------------------------------------
MJPG_sender::MJPG_sender(int port, int _timeout, int _quality)
        : sock(INVALID_SOCKET)
        , timeout(_timeout)
        , quality(_quality)
{
    close_all_sockets = 0;
    FD_ZERO(&master);
    if (port) open(port);
}
//----------------------------------------------------------------------------------------
MJPG_sender::~MJPG_sender(void)
{
    close_all();
    release();
}
//----------------------------------------------------------------------------------------
bool MJPG_sender::release(void)
{
    if (sock != INVALID_SOCKET)
        ::shutdown(sock, 2);
    sock = (INVALID_SOCKET);
    return false;
}
//----------------------------------------------------------------------------------------
void MJPG_sender::close_all(void)
{
    close_all_sockets = 1;
    cv::Mat tmp(cv::Size(10, 10), CV_8UC3);
    write(tmp);
}
//----------------------------------------------------------------------------------------
bool MJPG_sender::open(int port)
{
    sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN address;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);    // ::htons(port);
    int reuse = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        cerr << "setsockopt(SO_REUSEADDR) failed" << endl;

    // Non-blocking sockets
    int flags = fcntl(sock, F_GETFL, 0);
    fcntl(sock, F_SETFL, flags | O_NONBLOCK);

#ifdef SO_REUSEPORT
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0)
        cerr << "setsockopt(SO_REUSEPORT) failed" << endl;
#endif
    if (::bind(sock, (SOCKADDR*)&address, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
    {
        cerr << "error MJPG_sender: couldn't bind sock " << sock << " to port " << port << "!" << endl;
        return release();
    }
    if (::listen(sock, 10) == SOCKET_ERROR)
    {
        cerr << "error MJPG_sender: couldn't listen on sock " << sock << " on port " << port << " !" << endl;
        return release();
    }
    FD_ZERO(&master);
    FD_SET(sock, &master);
    maxfd = sock;
    return true;
}
//----------------------------------------------------------------------------------------
bool MJPG_sender::isOpened(void)
{
    return sock != INVALID_SOCKET;
}
//----------------------------------------------------------------------------------------
bool MJPG_sender::write(const cv::Mat& frame)
{
    fd_set rread = master;
    struct timeval select_timeout = { 0, 0 };
    struct timeval socket_timeout = { 0, timeout };
    if (::select(maxfd + 1, &rread, NULL, NULL, &select_timeout) <= 0)
        return true; // nothing broken, there's just noone listening

    std::vector<uchar> outbuf;
    std::vector<int> params;
    params.push_back(cv::IMWRITE_JPEG_QUALITY);
    params.push_back(quality);
    cv::imencode(".jpg", frame, outbuf, params);  //REMOVED FOR COMPATIBILITY
    // https://docs.opencv.org/3.4/d4/da8/group__imgcodecs.html#ga292d81be8d76901bff7988d18d2b42ac
    //std::cerr << "cv::imencode call disabled!" << std::endl;
    int outlen = static_cast<int>(outbuf.size());

    for(int s = 0; s <= maxfd; s++){
        socklen_t addrlen = sizeof(SOCKADDR);
        if (!FD_ISSET(s, &rread))      // ... on linux it's a bitmask ;)
            continue;

        if (s == sock) // request on master socket, accept and send main header.
        {
            SOCKADDR_IN address = { 0 };
            SOCKET      client = ::accept(sock, (SOCKADDR*)&address, &addrlen);
            if (client == SOCKET_ERROR)
            {
                cerr << "error MJPG_sender: couldn't accept connection on sock " << sock << " !" << endl;
                return false;
            }
            if (setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (char *)&socket_timeout, sizeof(socket_timeout)) < 0) {
                cerr << "error MJPG_sender: SO_RCVTIMEO setsockopt failed\n";
            }
            if (setsockopt(client, SOL_SOCKET, SO_SNDTIMEO, (char *)&socket_timeout, sizeof(socket_timeout)) < 0) {
                cerr << "error MJPG_sender: SO_SNDTIMEO setsockopt failed\n";
            }
            maxfd = (maxfd>client ? maxfd : client);
            FD_SET(client, &master);
            _write(client, "HTTP/1.0 200 OK\r\n", 0);
            _write(client,
                "Server: Mozarella/2.2\r\n"
                "Accept-Range: bytes\r\n"
                "Connection: close\r\n"
                "Max-Age: 0\r\n"
                "Expires: 0\r\n"
                "Cache-Control: no-cache, private\r\n"
                "Pragma: no-cache\r\n"
                "Content-Type: multipart/x-mixed-replace; boundary=mjpegstream\r\n"
                "\r\n", 0);
            cerr << "MJPG_sender: new client " << client << endl;
        }
        else // existing client, just stream pix
        {
            if (close_all_sockets) {
                int result = close_socket(s);
                cerr << "MJPG_sender: close clinet: " << result << " \n";
                continue;
            }

            char head[400];
            sprintf(head, "--mjpegstream\r\nContent-Type: image/jpeg\r\nContent-Length: %zu\r\n\r\n", (size_t)outlen);
            _write(s, head, 0);
            int n = _write(s, (char*)(&outbuf[0]), outlen);
            if (n < (int)outlen){
                cerr << "MJPG_sender: kill client " << s << endl;
                //::shutdown(s, 2);
                close_socket(s);
                FD_CLR(s, &master);
            }
        }
    }
    if (close_all_sockets) {
        int result = close_socket(sock);
        cerr << "MJPG_sender: close acceptor: " << result << " \n\n";
    }
    return true;
}
//----------------------------------------------------------------------------------------
