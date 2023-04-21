#pragma once

#include <vector>
#include <string>

namespace perf {

    struct CpuCore {
        int32_t id;
        std::string vendor;
        std::string type;
        std::vector<int32_t> frequencies;
    };

    struct CpuInfo {
        std::string hardware;
        std::vector<CpuCore> cores;
    };

    struct Device {
        std::string id;
        std::string name;

        CpuInfo cpu;
    };

    class ADB {
    public:
        ADB() = default;
        ~ADB() = default;

        bool Init();
        std::vector<std::string> SearchDevices();
        std::vector<std::string> SearchPackages(const std::string &id) const;
        std::vector<std::string> Execute(const std::string &cmd) const;

        std::string GetDeviceName(const std::string &id) const;

    private:
        void ProcessCPUInfo(Device &dev);

        std::string adb;
        std::vector<Device> devices;
    };

}
