
#include "stdafx.h"
#include <iostream>
#include "rest_client.h";

int _tmain(int argc, _TCHAR* argv[])
{
    restful::rest_client client(
        L"my client 1.0",
        L"api.github.com",
        443,
        5000,
        true);

    std::stringstream data;
    DWORD status = client.get(data, L"/users/eddiewillett", L"Pragma: no-cache; Accept: application/vnd.github.beta+json");
    if (status == ERROR_SUCCESS)
        std::cout << data.str();
    else
        std::cout << "Error retrieveing content. Error(" << status << ")" << std::endl;
    
    return 0;
}

