
#include "memoryInfo.hpp"
#include <tuttle/common/utils/global.hpp>

#ifdef WINDOWS
#include <windows.h>
#else
#include <sys/sysinfo.h>
#endif

MemoryInfo getMemoryInfo()
{
	MemoryInfo infos;
#ifdef WINDOWS
	MEMORYSTATUS memory;
	GlobalMemoryStatus( &memory );
	
	// memory.dwMemoryLoad;
	infos._totalRam = memory.dwTotalPhys;
	infos._freeRam = memory.dwAvailPhys;
	//memory.dwTotalPageFile;
	//memory.dwAvailPageFile;
	infos._totalSwap = memory.dwTotalVirtual;
	infos._freeSwap = memory.dwAvailVirtual;
#else
	struct sysinfo sys_info;
	sysinfo( &sys_info );
	
	infos._totalRam = sys_info.totalram * sys_info.mem_unit;
	infos._freeRam = sys_info.freeram * sys_info.mem_unit;
	//infos._sharedRam = sys_info.sharedram * sys_info.mem_unit;
	//infos._bufferRam = sys_info.bufferram * sys_info.mem_unit;
	infos._totalSwap = sys_info.totalswap * sys_info.mem_unit;
	infos._freeSwap = sys_info.freeswap * sys_info.mem_unit;

	COUT_X( 80, "-=-" );
	COUT_VAR( infos._totalRam );
	COUT_VAR( infos._freeRam );
	COUT_VAR( infos._totalSwap );
	COUT_VAR( infos._freeSwap );
	COUT( "" );
	COUT_VAR( sys_info.sharedram * sys_info.mem_unit );
	COUT_VAR( sys_info.bufferram * sys_info.mem_unit );
	COUT( "plop" );
	
#endif
	return infos;
}


