
#include <stdio.h>

#include <ADB.h>
#include <Cmd.h>

int main(int argc, char **argv)
{
    perf::ADB adb;
    adb.Init();
    auto devices = adb.SearchDevices();
    if (!devices.empty()) {
        auto packages = adb.SearchPackages(devices[0]);
        for (auto &package : packages) {
            printf("%s\n", package.c_str());
        }
    }

    return 0;
}
