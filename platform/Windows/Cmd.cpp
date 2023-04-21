#include <Cmd.h>
#include <memory>
#include <Windows.h>

namespace perf {

    struct Pipe {
        ~Pipe();
        bool Init();
        void CloseRead();
        void CloseWrite();

        HANDLE read;
        HANDLE write;
    };

    bool Pipe::Init()
    {
        SECURITY_ATTRIBUTES security = {sizeof(SECURITY_ATTRIBUTES)};
        security.bInheritHandle = TRUE;
        security.lpSecurityDescriptor = nullptr;

        if (!CreatePipe(&read, &write, &security, 0)) {
            return false;
        }
        return true;
    }

    void Pipe::CloseRead()
    {
        if (read != nullptr) {
            CloseHandle(read);
            read = nullptr;
        }
    }

    void Pipe::CloseWrite()
    {
        if (write != nullptr) {
            CloseHandle(write);
            write = nullptr;
        }
    }

    Pipe::~Pipe()
    {
        CloseRead();
        CloseWrite();
    }

    bool RunCmd(const std::string &cmd, std::string &out)
    {
        auto pipe = std::make_unique<Pipe>();
        if (!pipe->Init()) {
            return false;
        }

        PROCESS_INFORMATION procInfo = {};
        STARTUPINFO startInfo = {sizeof(STARTUPINFO)};
        startInfo.hStdOutput = pipe->write;
        startInfo.hStdError = pipe->write;
        startInfo.dwFlags |= STARTF_USESTDHANDLES;

        auto success = CreateProcess(nullptr, (char*)cmd.c_str(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW, nullptr, nullptr, &startInfo, &procInfo);
        if (!success) {
            return false;
        }

        WaitForSingleObject(procInfo.hProcess, 5000);
        for (;;) {
            DWORD dwRead = 0;
            DWORD dwAvail = 0;

            if (!::PeekNamedPipe(pipe->read, nullptr, 0, nullptr, &dwAvail, nullptr)) {
                break;
            }
            if (dwAvail == 0) {
                break;
            }
            std::string tmp(dwAvail, 0);
            if (!::ReadFile(pipe->read, (char*)tmp.data(), dwAvail, &dwRead, nullptr) || dwRead == 0) {
                break;
            }
            out += tmp;
        }
        CloseHandle(procInfo.hProcess);
        CloseHandle(procInfo.hThread);
        return true;
    }

}
