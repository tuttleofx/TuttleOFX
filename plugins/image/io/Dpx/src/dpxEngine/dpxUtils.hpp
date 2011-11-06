#ifndef DPX_UTILS_HPP
#define DPX_UTILS_HPP

#include <stdint.h>

namespace tuttle {
namespace utils {

inline void endian_swap(unsigned short& x)
{
    x = (x>>8) |
	(x<<8);
}

inline void endian_swap(unsigned int& x)
{
    x = (x>>24) |
	((x<<8) & 0x00FF0000) |
	((x>>8) & 0x0000FF00) |
	(x<<24);
}
/*
inline void endian_swap( uint64_t& x)
{
    x = (x>>56) |
	((x<<40) & 0x00FF000000000000) |
	((x<<24) & 0x0000FF0000000000) |
	((x<<8)  & 0x000000FF00000000) |
	((x>>8)  & 0x00000000FF000000) |
	((x>>24) & 0x0000000000FF0000) |
	((x>>40) & 0x000000000000FF00) |
	(x<<56);
}*/

// Swap Endianness
template <typename T>
T swapEndian( T _var )
{
	T* addr = &_var;
	unsigned char swapped[16];

	switch( sizeof( T ) )
	{
		case 2:
			swapped[0] = *( (unsigned char*)addr + 1 );
			swapped[1] = *( (unsigned char*)addr  );
			//endian_swap(unsigned short& x);
			break;
		case 4:
			swapped[0] = *( (unsigned char*)addr + 3 );
			swapped[1] = *( (unsigned char*)addr + 2 );
			swapped[2] = *( (unsigned char*)addr + 1 );
			swapped[3] = *( (unsigned char*)addr  );
			break;
		case 8:
			swapped[0] = *( (unsigned char*)addr + 7 );
			swapped[1] = *( (unsigned char*)addr + 6 );
			swapped[2] = *( (unsigned char*)addr + 5 );
			swapped[3] = *( (unsigned char*)addr + 4 );
			swapped[4] = *( (unsigned char*)addr + 3 );
			swapped[5] = *( (unsigned char*)addr + 2 );
			swapped[6] = *( (unsigned char*)addr + 1 );
			swapped[7] = *( (unsigned char*)addr  );
			break;
		case 16:
			swapped[0]  = *( (unsigned char*)addr + 15 );
			swapped[1]  = *( (unsigned char*)addr + 14 );
			swapped[2]  = *( (unsigned char*)addr + 13 );
			swapped[3]  = *( (unsigned char*)addr + 12 );
			swapped[4]  = *( (unsigned char*)addr + 11 );
			swapped[5]  = *( (unsigned char*)addr + 10 );
			swapped[6]  = *( (unsigned char*)addr + 9 );
			swapped[7]  = *( (unsigned char*)addr + 8 );
			swapped[8]  = *( (unsigned char*)addr + 7 );
			swapped[9]  = *( (unsigned char*)addr + 6 );
			swapped[10] = *( (unsigned char*)addr + 5 );
			swapped[11] = *( (unsigned char*)addr + 4 );
			swapped[12] = *( (unsigned char*)addr + 3 );
			swapped[13] = *( (unsigned char*)addr + 2 );
			swapped[14] = *( (unsigned char*)addr + 1 );
			swapped[15] = *( (unsigned char*)addr  );
			break;
	}
	return *(T*)swapped;
}

}  // namespace utils
}  // namespace tuttle

#endif // DPX_UTILS_HPP
