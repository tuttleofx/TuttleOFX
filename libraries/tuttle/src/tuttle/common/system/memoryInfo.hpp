#ifndef TUTTLE_SYSTEM_MEMORY_HPP_
#define TUTTLE_SYSTEM_MEMORY_HPP_

#include <cstddef>

struct MemoryInfo
{
	std::size_t _totalRam;
	std::size_t _freeRam;
//	std::size_t _sharedRam;
//	std::size_t _bufferRam;
	std::size_t _totalSwap;
	std::size_t _freeSwap;
};

MemoryInfo getMemoryInfo();

#endif

