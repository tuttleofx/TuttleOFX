#include "system.hpp"
#include "memoryInfo.hpp"
#include <tuttle/common/utils/global.hpp>

#if defined(__WINDOWS__)
#include <windows.h>
#elif defined(__LINUX__)
#include <sys/sysinfo.h>
#elif defined(__APPLE__)
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/vm_statistics.h>
#include <mach/mach_types.h>
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#else
#warning "System unrecognized. Can't found memory infos."
#include <limits>
#endif

MemoryInfo getMemoryInfo()
{
    MemoryInfo infos;

#if defined(__WINDOWS__)
    MEMORYSTATUS memory;
    GlobalMemoryStatus(&memory);

    // memory.dwMemoryLoad;
    infos._totalRam = memory.dwTotalPhys;
    infos._freeRam = memory.dwAvailPhys;
    // memory.dwTotalPageFile;
    // memory.dwAvailPageFile;
    infos._totalSwap = memory.dwTotalVirtual;
    infos._freeSwap = memory.dwAvailVirtual;
#elif defined(__LINUX__)
    struct sysinfo sys_info;
    sysinfo(&sys_info);

    infos._totalRam = sys_info.totalram * sys_info.mem_unit;
    infos._freeRam = sys_info.freeram * sys_info.mem_unit;
    // infos._sharedRam = sys_info.sharedram * sys_info.mem_unit;
    // infos._bufferRam = sys_info.bufferram * sys_info.mem_unit;
    infos._totalSwap = sys_info.totalswap * sys_info.mem_unit;
    infos._freeSwap = sys_info.freeswap * sys_info.mem_unit;
//	TUTTLE_LOG_VAR( TUTTLE_TRACE, sys_info.sharedram * sys_info.mem_unit );
//	TUTTLE_LOG_VAR( TUTTLE_TRACE, sys_info.bufferram * sys_info.mem_unit );
#elif defined(__APPLE__)
    uint64_t physmem;
    size_t len = sizeof physmem;
    int mib[2] = {CTL_HW, HW_MEMSIZE};
    size_t miblen = sizeof(mib) / sizeof(mib[0]);

    // Total physical memory.
    if(sysctl(mib, miblen, &physmem, &len, NULL, 0) == 0 && len == sizeof(physmem))
        infos._totalRam = physmem;

    // Virtual memory.
    mib[0] = CTL_VM;
    mib[1] = VM_SWAPUSAGE;
    struct xsw_usage swap;
    len = sizeof(struct xsw_usage);
    if(sysctl(mib, miblen, &swap, &len, NULL, 0) == 0)
    {
        infos._totalSwap = swap.xsu_total;
        infos._freeSwap = swap.xsu_avail;
    }

    // In use.
    mach_port_t stat_port = mach_host_self();
    vm_size_t page_size;
    vm_statistics_data_t vm_stat;
    mach_msg_type_number_t count = sizeof(vm_stat) / sizeof(natural_t);
    if(KERN_SUCCESS == host_page_size(stat_port, &page_size) &&
       KERN_SUCCESS == host_statistics(stat_port, HOST_VM_INFO, (host_info_t)&vm_stat, &count))
    {
        // uint64_t used = ((int64_t)vm_stat.active_count + (int64_t)vm_stat.inactive_count + (int64_t)vm_stat.wire_count) *
        // (int64_t)page_size;
        // infos._freeRam = infos._totalRam - used;
        infos._freeRam = (int64_t)vm_stat.free_count * (int64_t)page_size;
    }
#else
    // TODO: could be done on FreeBSD too
    // see https://github.com/xbmc/xbmc/blob/master/xbmc/linux/XMemUtils.cpp
    infos._totalRam = infos._freeRam = infos._totalSwap = infos._freeSwap = std::numeric_limits<std::size_t>::max();
#endif
    TUTTLE_LOG_DEBUG("[Memory infos] " << infos);

    return infos;
}

std::ostream& operator<<(std::ostream& os, const MemoryInfo& infos)
{
    os << "total ram:" << infos._totalRam << std::endl
       << "free ram:" << infos._freeRam << std::endl
       << "total swap:" << infos._totalSwap << std::endl
       << "free swap:" << infos._freeSwap << std::endl;
    return os;
}
