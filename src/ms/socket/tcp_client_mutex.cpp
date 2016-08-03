#include "stdafx.h"
#include <ms/socket/tcp_client_mutex.h>
#include <WS2tcpip.h>

namespace ms{
    namespace socket{
        bool tcp_client_mutex::InitSocket()
        {
            run_ = true;
            std::lock_guard<std::mutex> guard(socket_mutex_);
            
            server_addr_.sin_family = AF_INET;
            server_addr_.sin_port = htons(port_);

            if (INVALID_SOCKET != socket_)
            {
                closesocket(socket_);
            }
            socket_ = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (INVALID_SOCKET == socket_)
            {
                return false;
            }
            InetPton(AF_INET, ip_.c_str(), &server_addr_.sin_addr);
            return true;
        }

        bool tcp_client_mutex::Connect()
        {
            std::lock_guard<std::mutex> guard(socket_mutex_);

            int ret = ::connect(socket_, (sockaddr*)&server_addr_, sizeof(sockaddr_in));
            if (SOCKET_ERROR == ret)
            {
                closesocket(socket_);
                socket_ = INVALID_SOCKET;
                return false;
            }
            if (auto_reconnect_)
            {
                StartAutoConnect();
            }
            connected_ = true;
            return true;
        }
        void tcp_client_mutex::Stop()
        {
            run_ = false;
            Close();
        }
        void tcp_client_mutex::Close()
        {
            std::lock_guard<std::mutex> guard(socket_mutex_);
            connected_ = false;
            closesocket(socket_);
            socket_ = INVALID_SOCKET;
        }

        int tcp_client_mutex::Send(const char * buf , int size, int flags)
        {
            if (!connected_)
            {
                _cond_thread.notify_one();
                return -1;
            }
            std::lock_guard<std::mutex> guard(socket_mutex_);
            if (!connected_)
            {
                _cond_thread.notify_one();
                return -1;
            }
            long send_t = 0;
            do
            {
                long ret = ::send(socket_, buf+send_t, size-send_t, flags);
                DWORD dwErr = WSAGetLastError();
                if (ret > 0)
                {
                    send_t += ret;
                }
                else if (ret == 0)
                {
                    continue;
                }
                else if (dwErr == WSAEWOULDBLOCK)
                {
                    break;
                }
                else if (-1 == ret)
                {
                    break;
                }

            } while (send_t < size);
//             int ret = ::send(socket_, buf, size, flags);
            if (send_t < size)
            {
                connected_ = false;
            }
            return send_t;
        }
        bool tcp_client_mutex::StartAutoConnect()
        {
            if (!threadPtr_)
            {
                threadPtr_ = std::make_shared<std::thread>(std::bind(&tcp_client_mutex::AutoConnect, this));
            }
            return !!threadPtr_;
        }
        void tcp_client_mutex::AutoConnect()
        {
            while (run_)
            {
                if (!connected_)
                {
                    if (InitSocket())
                        Connect();
                }
                else
                {
                    std::unique_lock <std::mutex> lck(reconnect_mutex_);
                    _cond_thread.wait(lck);
                    continue;
                }
            }
        }
    }
}

       