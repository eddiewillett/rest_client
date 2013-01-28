#pragma once

#include <functional>
#include <string>
#include <sstream>
#include <memory>

namespace restful
{   
    typedef std::unique_ptr<VOID, std::function<void(LPVOID)>> unique_handle;
    
    class rest_client
    {
    public:
        rest_client(const std::wstring& name,
                    const std::wstring& host, 
                    const int port, 
                    const int timeout, 
                    const bool is_secure);
        virtual ~rest_client();

        DWORD get(std::stringstream& data, 
                  const std::wstring& absolute_path,
                  const std::wstring& headers);
        DWORD put(const std::string& data, 
                  const std::wstring& absolute_path,
                  const std::wstring& headers);
        DWORD post(const std::string& data, 
                   const std::wstring& absolute_path,
                   const std::wstring& headers);
        DWORD del(const std::wstring& absolute_path,
                  const std::wstring& headers);

    private:
        rest_client(const rest_client&);
        rest_client& operator=(const rest_client&);

        bool initialize();
        DWORD put_or_post(const wchar_t* http_verb,
                          const std::string& data, 
                          const std::wstring& absolute_path,
                          const std::wstring& headers);

        unique_handle open_request(const wchar_t* http_verb, 
                                   const std::wstring& absolute_path);
        BOOL set_headers(LPVOID request, 
                         const std::wstring& headers);
        BOOL send_request(LPVOID request, 
                          const size_t data_length);
        BOOL receive_response(LPVOID request);
        BOOL read_data(LPVOID request, std::stringstream& data);
        BOOL write_data(LPVOID request, const std::string& data);
        unique_handle make_unique_handle(LPVOID handle);

        // members
        unique_handle _session;
        unique_handle _connection;
        std::wstring _name;
        std::wstring _host;
        int _port;
        int _timeout;
        bool _is_secure;
    };
}

