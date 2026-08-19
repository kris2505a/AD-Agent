#include "DirectoryService.hpp"
#include <print>
#include "Log.hpp"

DirectoryService::DirectoryService() {
    Log::Info("Allocating Directory Service");
    if (s_Instance) {
        Log::Error("Tried re-allocating on s_Instance");
        throw std::runtime_error("Instance already exists!");
    }
    s_Instance = this;
    Log::Info("DirectoryService initialized on s_Instance");
}

auto DirectoryService::Init(const std::wstring& connectionString) -> void {
    if (!s_Instance) {
        throw std::runtime_error("Instance of DirectoryService not created yet");
    }
    Log::Info("Initializing Service");
    s_Instance->InitImpl(connectionString);
    Log::Info("Initialized Service");
}

auto DirectoryService::QueryUsers() -> std::vector<User> {
    if (!s_Instance) {
        throw std::runtime_error("Instance of DirectoryService not created yet");
    }
    return s_Instance->QueryUserImpl();
}

auto DirectoryService::Shutdown() -> void {
    Log::Info("Shtting down service");
    s_Instance->ShutdownImpl();
    Log::Info("Service shut down");
}

auto DirectoryService::ShutdownImpl() -> void {
    CoUninitialize();
}


auto DirectoryService::InitImpl(const std::wstring& connectionString) -> void {
    
    HRESULT hr;
    Log::Info("Initializing COINIT_APARTMENTTHREADED");
    hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    
    if (FAILED(hr)) {
        Log::Error("Failed to perform COINIT_APARTMENTTHREADED: ");
        throw std::runtime_error("");
    }
    
    Log::Info("Connecting to AD: {}", std::string(connectionString.begin(), connectionString.end()));

    hr = ADsOpenObject(
        connectionString.c_str(),
        nullptr,
        nullptr,
        ADS_SECURE_AUTHENTICATION,
        IID_IADsContainer,
        reinterpret_cast<void**>(m_ADContainer.GetAddressOf())
    );
    if (FAILED(hr)) {
        Log::Error("Failed to connect to AD: {}", std::string(connectionString.begin(), connectionString.end()));
        Log::Error("{}", Log::HRtoStr(hr));
        throw std::runtime_error("");
    }
    Microsoft::WRL::ComPtr<IADs> ads;

    Log::Info("Opening Directory");
    hr = ADsOpenObject(
        connectionString.c_str(),
        nullptr,
        nullptr,
        ADS_SECURE_AUTHENTICATION,
        IID_IADs,
        reinterpret_cast<void**>(ads.GetAddressOf())
    );
    
    if (FAILED(hr)) {
        Log::Error("Failed to open Diriectory");
        Log::Error("{}", Log::HRtoStr(hr));
        throw std::runtime_error("");
    }
}

auto DirectoryService::QueryUserImpl() -> std::vector<User> {
    HRESULT hr;
    
    Microsoft::WRL::ComPtr<IDirectorySearch> adSearch;
    hr = m_ADContainer.As(&adSearch);
    
    if (FAILED(hr)) {
        Log::Error("Failed to create IDirectorySearch");
        Log::Error("{}", Log::HRtoStr(hr));
        throw std::runtime_error("");
    }

    std::vector<ADS_SEARCHPREF_INFO> searchPref(2);

    Log::Info("Setting search preference");

    searchPref.at(0).dwSearchPref = ADS_SEARCHPREF_SEARCH_SCOPE;
    searchPref.at(0).vValue.dwType = ADSTYPE_INTEGER;
    searchPref.at(0).vValue.Integer = ADS_SCOPE_SUBTREE;

    searchPref.at(1).dwSearchPref = ADS_SEARCHPREF_PAGESIZE;
    searchPref.at(1).vValue.dwType = ADSTYPE_INTEGER;
    searchPref.at(1).vValue.Integer = 1000;
    

    hr = adSearch->SetSearchPreference(searchPref.data(), searchPref.size());

    if (FAILED(hr)) {
        Log::Error("Failed to set search preference");
        Log::Error("{}", Log::HRtoStr(hr));
        throw std::runtime_error("");
    }

    ADS_SEARCH_HANDLE hSearch;

    WCHAR searchFilter[] = L"(&"
        L"(objectCategory=person)"
        L"(objectClass=user)"
        L"(!(sAMAccountName=Administrator))"
        L"(!(sAMAccountName=Guest))"
        L"(!(sAMAccountName=krbtgt))"

    L")";
    
    std::wstring samAccountName = L"sAMAccountName";
    std::wstring displayName = L"displayName";
    std::wstring mail = L"mail";
    
    std::vector<LPWSTR> attributes = {
        samAccountName.data(),
        displayName.data(),
        mail.data(),
    };

    DWORD dwNumAttributes = static_cast<DWORD>(attributes.size());

    hr = adSearch->ExecuteSearch(searchFilter, attributes.data(), dwNumAttributes, &hSearch);
    
    if (FAILED(hr)) {
        Log::Error("Failed to execute search");
        Log::Error("{}", Log::HRtoStr(hr));
    }
        
    std::vector<User> users;

    std::println("querying users");

    while (S_OK == hr) {
        std::wstring userName, displayName, mail, GUID;
        ADS_SEARCH_COLUMN col;
        

        for (size_t i = 0; i < attributes.size(); i++) {
            hr = adSearch->GetColumn(hSearch, attributes[i], &col);
            
            if (FAILED(hr))
                continue;

            std::wstring value;
            switch (col.dwADsType) {
            case ADSTYPE_DN_STRING:
                value = std::wstring(col.pADsValues[0].DNString);
                break;
            case ADSTYPE_CASE_IGNORE_STRING:
                value = std::wstring(col.pADsValues[0].CaseIgnoreString);
                break;
            }
            
            if (wcscmp(attributes.at(i), L"sAMAccountName") == 0) {
                userName = value;
            }

            else if (wcscmp(attributes.at(i), L"displayName") == 0) {
                displayName = value;
            }

            else if (wcscmp(attributes.at(i), L"mail") == 0) {
                mail = value;
            }
            hr = adSearch->FreeColumn(&col);

        }
        

        UserInfo info {
            .UserName = userName,
            .DisplayName = displayName,
            .Email = mail,
            .Guid = L"None!"
        };

        users.push_back(User(info));
        hr = adSearch->GetNextRow(hSearch);
    }

    adSearch->CloseSearchHandle(hSearch);

    return users;
}

auto DirectoryService::CreateUser(const std::wstring& sAMAccountName) -> void {
    s_Instance->CreateUserImpl(sAMAccountName);
}

auto DirectoryService::CreateUserImpl(const std::wstring& samAccountName) -> void {
    
    std::vector<ADS_ATTR_INFO> attrInfo;
    ADSVALUE objectValue {};
    objectValue.dwType = ADSTYPE_CASE_IGNORE_STRING;
    objectValue.CaseIgnoreString = const_cast<PWSTR>(L"user");

    ADS_ATTR_INFO objectClassInfo {};
    objectClassInfo.pszAttrName = const_cast<PWSTR>(L"objectClass");
    objectClassInfo.dwControlCode = ADS_ATTR_UPDATE;
    objectClassInfo.dwADsType = ADSTYPE_CASE_IGNORE_STRING;
    objectClassInfo.pADsValues = &objectValue;
    objectClassInfo.dwNumValues = 1;

    attrInfo.push_back(objectClassInfo);
    
    ADSVALUE samValue {};
    samValue.dwType = ADSTYPE_CASE_IGNORE_STRING;
    samValue.CaseIgnoreString = const_cast<PWSTR>(samAccountName.c_str());

    ADS_ATTR_INFO accountNameInfo {};
    accountNameInfo.pszAttrName = const_cast<PWSTR>(L"sAMAccountName");
    accountNameInfo.dwControlCode = ADS_ATTR_UPDATE;
    accountNameInfo.dwADsType = ADSTYPE_CASE_IGNORE_STRING;
    accountNameInfo.pADsValues = &samValue;
    accountNameInfo.dwNumValues = 1;

    attrInfo.push_back(accountNameInfo);


    ADSVALUE uacValue {};
    uacValue.dwType = ADSTYPE_INTEGER;
    uacValue.Integer = ADS_UF_NORMAL_ACCOUNT | ADS_UF_ACCOUNTDISABLE;

    ADS_ATTR_INFO uacInfo{};
    uacInfo.pszAttrName = const_cast<PWSTR>(L"userAccountControl");
    uacInfo.dwControlCode = ADS_ATTR_UPDATE;
    uacInfo.dwADsType = ADSTYPE_INTEGER;
    uacInfo.pADsValues = &uacValue;
    uacInfo.dwNumValues = 1;

    attrInfo.push_back(uacInfo);

    ADSVALUE displayNameValue {};
    displayNameValue.dwType = ADSTYPE_CASE_IGNORE_STRING;
    displayNameValue.CaseIgnoreString = const_cast<PWSTR>(L"Krisp Test 02");

    ADS_ATTR_INFO displayNameInfo {};
    displayNameInfo.pszAttrName = const_cast<PWSTR>(L"displayName");
    displayNameInfo.dwControlCode = ADS_ATTR_UPDATE;
    displayNameInfo.dwADsType = ADSTYPE_CASE_IGNORE_STRING;
    displayNameInfo.pADsValues = &displayNameValue;
    displayNameInfo.dwNumValues = 1;

    attrInfo.push_back(displayNameInfo);

    std::wstring commonName = L"CN=" + samAccountName;
    Microsoft::WRL::ComPtr<IDispatch> dispatch;
    Log::Info("Creating IDispatch");
    HRESULT hr;
    
    Microsoft::WRL::ComPtr<IDirectoryObject> directoryObject;
    hr = m_ADContainer.As(&directoryObject);
    if (FAILED(hr)) {
        Log::Error("Failed to com cast IDirectoryObject");
        Log::Error("{}", Log::HRtoStr(hr));
        throw std::runtime_error("");
    }

    hr = directoryObject->CreateDSObject(
        commonName.data(),
        attrInfo.data(),
        static_cast<DWORD>(attrInfo.size()),
        dispatch.GetAddressOf()
    );

    if (FAILED(hr)) {
        Log::Error("Failed to create IDispatch");
        Log::Error("{}", Log::HRtoStr(hr));
        throw std::runtime_error("");
    }

    Log::Info("Creating IADsUser");
    Microsoft::WRL::ComPtr<IADsUser> user;
    hr = dispatch.As(&user);
    std::wstring password = L"ServLocal@123";
    
    if (FAILED(hr)) {
        Log::Error("Failed to create IADsUser");
        Log::Error("{}", Log::HRtoStr(hr));
        throw std::runtime_error("");
    }
    
    hr = user->SetPassword(password.data());
    if (FAILED(hr)) {
        Log::Error("Failed to set password");
        Log::Error("{}", Log::HRtoStr(hr));
    }

    Microsoft::WRL::ComPtr<IADs> ads;
    hr = user.As(&ads);

    if (FAILED(hr)) {
        Log::Error("Failed to com cast IADsUser as IADs");
        Log::Error("{}", Log::HRtoStr(hr));
        throw std::runtime_error("");
    }

    VARIANT value {};
    VariantInit(&value);

    value.vt = VT_I4;
    value.lVal = ADS_UF_NORMAL_ACCOUNT;

    hr = ads->Put(const_cast<PWSTR>(L"userAccountControl"), value);

    if (FAILED(hr)) {
        Log::Error("Failed to Put UAC");
        Log::Error("{}", Log::HRtoStr(hr));
        VariantClear(&value);
        throw std::runtime_error("");
    }

    hr = ads->SetInfo();
    if (FAILED(hr)) {
        Log::Error("Failed to SetInfo UAV");
        Log::Error("{}", Log::HRtoStr(hr));
        VariantClear(&value);
        throw std::runtime_error("");
    }
    VariantClear(&value);

}
