#include "stdafx.h"
#include "rest_client.h"
#include "winhttp.h"
#include <vector>

#pragma comment(lib, "winhttp.lib")

using namespace restful;
using namespace std;

static const wchar_t PUT[] = L"PUT";
static const wchar_t GET[] = L"GET";
static const wchar_t POST[] = L"POST";
static const wchar_t DEL[] = L"DELETE";


rest_client::rest_client(const wstring& name,
                         const wstring& host, 
                         const int port, 
                         const int timeout, 
                         const bool is_secure) :
    _name(name),
    _host(host),
    _port(port),
    _timeout(timeout),
    _is_secure(is_secure)
{
}

rest_client::~rest_client()
{
}

bool rest_client::initialize()
{
	// currently not thread safe initialization
    if (_connection != nullptr)
        return true;
        
    // initialize session
    if (_session == nullptr)
	{
        HINTERNET handle = WinHttpOpen(
                                _name.c_str(),  
			                    WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
			                    WINHTTP_NO_PROXY_NAME, 
			                    WINHTTP_NO_PROXY_BYPASS, 
                                0);

        if (handle == nullptr)
            return false;

        _session = move(make_unique_handle(handle));
	}

    // initialize connection
	if (_session != nullptr)
	{
		HINTERNET handle = WinHttpConnect(
                                _session.get(), 
                                _host.c_str(), 
                                _port, 
                                0);
        
        if (handle == nullptr)
            return false;

        _connection = move(make_unique_handle(handle));
	}

	return _connection != nullptr;
}

DWORD rest_client::get(stringstream& data, 
                       const wstring& absolute_path,
                       const wstring& headers)
{
	data.clear();
	
	if (!initialize())
		return GetLastError();

    auto request = open_request(GET, absolute_path);

	if (request == nullptr)
	    return GetLastError();

    BOOL is_success = 
        set_headers(request.get(), headers) &&
		send_request(request.get(), 0) &&
		receive_response(request.get()) &&
        read_data(request.get(), data);

	if(is_success)
        return ERROR_SUCCESS;
		
    return GetLastError();
}

DWORD rest_client::put(const string& data, 
                       const wstring& absolute_path,
                       const wstring& headers)
{
    return put_or_post(PUT, data, absolute_path, headers);
}

DWORD rest_client::post(const string& data, 
                        const wstring& absolute_path,
                        const wstring& headers)
{
    return put_or_post(POST, data, absolute_path, headers);
}

DWORD rest_client::put_or_post(const wchar_t* http_verb,
                               const string& data, 
                               const wstring& absolute_path,
                               const wstring& headers)
{
	if (!initialize())
		return GetLastError();

    auto request = open_request(http_verb, absolute_path);

	if (request == nullptr)
	    return GetLastError();

    BOOL is_success = 
        set_headers(request.get(), headers) &&
		send_request(request.get(), data.size()+1) &&
        write_data(request.get(), data) &&
        receive_response(request.get());

	if(is_success)
        return ERROR_SUCCESS;
		
    return GetLastError();
}

DWORD rest_client::del(const wstring& absolute_path,
                       const wstring& headers)
{
	if (!initialize())
		return GetLastError();

    auto request = open_request(DEL, absolute_path);

	if (request == nullptr)
	    return GetLastError();

	BOOL is_success = 
        set_headers(request.get(), headers) &&
		send_request(request.get(), 0) &&
        receive_response(request.get());

	if(is_success)
        return ERROR_SUCCESS;
		
    return GetLastError();
}

unique_handle rest_client::open_request(const wchar_t* http_verb,
                                        const wstring& absolute_path)
{
    return make_unique_handle( WinHttpOpenRequest( 
							        _connection.get(), 
							        http_verb, 
                                    absolute_path.c_str(), 
                                    nullptr, 
							        WINHTTP_NO_REFERER, 
                                    WINHTTP_DEFAULT_ACCEPT_TYPES,
                                    _is_secure ? WINHTTP_FLAG_SECURE : 0));
}

BOOL rest_client::set_headers(LPVOID request, const wstring& headers)
{
    return WinHttpAddRequestHeaders( 
				request, 
                headers.c_str(),
				(ULONG)-1L,
				WINHTTP_ADDREQ_FLAG_ADD );
}

BOOL rest_client::send_request(LPVOID request, 
                               const size_t data_length)
{
    return WinHttpSendRequest( 
                request,
			    WINHTTP_NO_ADDITIONAL_HEADERS, 
                0,
			    WINHTTP_NO_REQUEST_DATA, 
                0, 
			    data_length, 
                0 );
}

BOOL rest_client::receive_response(LPVOID request)
{
    return WinHttpReceiveResponse(
            request, 
            nullptr);
}

BOOL rest_client::read_data(LPVOID request, stringstream& data)
{
    DWORD size = 0;

	do 
	{
		size = 0;
		if(!WinHttpQueryDataAvailable( request, &size ) )
			return FALSE;

		if (size == 0)
			break;
			
		vector<char> buffer(size+1, 0);
			
		DWORD byte_count = 0;
		if(WinHttpReadData( request, (LPVOID)&buffer[0], size, &byte_count ) )
			data << &buffer[0];
		else
			return FALSE;

	} while( size > 0 );

    return TRUE;
}

BOOL rest_client::write_data(LPVOID request, const string& data)
{
    DWORD bytes_written = 0;
	return WinHttpWriteData( 
				request, 
				data.c_str(), 
                data.size()+1, 
                &bytes_written);
}

unique_handle rest_client::make_unique_handle(LPVOID handle)
{
    return unique_handle(handle, [](LPVOID handle) 
    { 
        if (handle != nullptr)
            WinHttpCloseHandle(handle);
    });
}