
#include <mutex>
#include <thread>
#include <memory>
#include <ms/tstring.h>
#include <WinSock2.h>
#include <condition_variable>

namespace ms{
namespace socket{

    class tcp_client_mutex
    {
    public:
        tcp_client_mutex(const ms::tstring& ip, const short& port, bool auto_reconnect = false)
            : ip_(ip)
            , port_(port_)
            , auto_reconnect_(auto_reconnect)
        {

        }
        ~tcp_client_mutex()
        {
            ;
        }
        bool InitSocket();
        bool Connect();
        bool StartAutoConnect();
        void Stop();
        int Send(const char * buf, int size, int flags);
        void Close();
    private:
        bool run_{false};
        std::mutex socket_mutex_;
        ms::tstring ip_;
        short port_;
        SOCKET socket_{INVALID_SOCKET};
        bool connected_{ false };
        sockaddr_in server_addr_;
        struct state{
            enum value_t{readly, runing, stop};
        };

        std::shared_ptr<std::thread> threadPtr_;
    
    private: // 自动重连
        bool auto_reconnect_{ false };
        std::mutex reconnect_mutex_;
        std::condition_variable _cond_thread;

        void AutoConnect();
    };


}
}