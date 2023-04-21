
#include <stdio.h>

#include <ADB.h>
#include <Cmd.h>

int main(int argc, char **argv)
{
    perf::ADB adb;
    adb.Init();
    auto devices = adb.SearchDevices();
    if (!devices.empty()) {
        adb.EnableWireless(devices[0]);
    }

    return 0;
}
