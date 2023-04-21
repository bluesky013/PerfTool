#include <ADB.h>
#include <sstream>
#include <unordered_map>
#include <Cmd.h>
#include <StringUtil.h>

namespace perf {

    std::unordered_map<int, std::string> VENDOR_MAP = {
            {0x41, "ARM"},
            {0x42, "Broadcom"},
            {0x43, "Cavium"},
            {0x44, "DigitalEquipment"},
            {0x48, "HiSilicon"},
            {0x49, "Infineon"},
            {0x4D, "Freescale"},
            {0x4E, "NVIDIA"},
            {0x50, "APM"},
            {0x51, "Qualcomm"},
            {0x56, "Marvell"},
            {0x69, "Intel"},
    };


    bool ADB::Init()
    {
        std::string out;
        perf::RunCmd("where adb", out);

        auto tokens = SplitString(out, "\r\n");
        if (!tokens.empty()) {
            adb = tokens[0] + " ";
            printf("%s", out.c_str());
            return true;
        }
        return false;
    }

    std::vector<std::string> ADB::SearchDevices()
    {
        std::vector<std::string> res;
        auto lines = Execute("devices");
        for (auto &line : lines) {
            auto tokens = SplitString(line, "\t");
            if (tokens.size() >= 2 && tokens[1] == "device") {
                res.emplace_back(tokens[0]);
            }
//                devices.emplace_back(Device{
//                    tokens[0],
//                    GetDeviceName(tokens[0])
//                });
//
//                auto &dev = devices.back();
//                ProcessCPUInfo(dev);
//
//                printf("id %s: name %s\n", dev.id.c_str(), dev.name.c_str());
        }
        return res;
    }

    std::vector<std::string> ADB::SearchPackages(const std::string &id) const
    {
        std::vector<std::string> res;

        std::stringstream ss;
        ss << "-s " << id << " shell pm list packages";
        auto lines = Execute(ss.str());
        for (auto &line : lines) {
            auto tokens = SplitString(line, ":");
            if (tokens.size() != 2) {
                continue;
            }
            res.emplace_back(tokens[1]);
        }
        return res;
    }

    std::vector<std::string> ADB::Execute(const std::string &cmd) const
    {
        std::stringstream ss;
        ss << adb << cmd;

        std::string out;
        RunCmd(ss.str(), out);
        return SplitString(out, "\r\n");
    }

    std::string ADB::GetDeviceName(const std::string &id) const
    {
        std::stringstream ss;
        ss << "-s " << id << " shell getprop ro.product.model";
        auto data = Execute(ss.str());
        return data.empty() ? "" : data[0];
    }

    void ProcessCpuCoreInfo(CpuCore &core, std::vector<std::string> &tokens)
    {
        auto &key = tokens[0];
        auto &value = tokens[1];

        if (key == "CPU implementer") {
            auto vendor = std::stoi(value);
            auto iter = VENDOR_MAP.find(vendor);
            core.vendor = iter != VENDOR_MAP.end() ? iter->second : "UNKNOWN";
        } else if (key == "CPU part") {
            auto part = std::stoi(value);
        }
    }

    void ADB::ProcessCPUInfo(Device &dev)
    {
        std::stringstream ss;
        ss << "-s " << dev.id << " shell cat /proc/cpuinfo";
        auto lines = Execute(ss.str());

        for (auto &line : lines) {
            auto tokens = SplitString(line, "\t:");
            if (tokens.size() != 2) {
                continue;
            }
            if (tokens[0] == "processor") {
                dev.cpu.cores.emplace_back();
                dev.cpu.cores.back().id = std::stoi(tokens[1]);
                // process frequency
            } else if (tokens[0] == "Hardware") {
                dev.cpu.hardware = tokens[1];
            }

            if (!dev.cpu.cores.empty()) {
                ProcessCpuCoreInfo(dev.cpu.cores.back(), tokens);
            }
        }

    }

}
