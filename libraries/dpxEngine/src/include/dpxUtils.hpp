/*
 * dpxUtils.hpp
 *
 *  Created on: 27 janv. 2009
 *      Author: Nicolas Rondaud
 */

#ifndef DPX_UTILS_HPP
#define DPX_UTILS_HPP

namespace tuttle {
namespace utils {

// Swap Endianness
template <typename T>
T swapEndian(T _var)
{
	T * addr = &_var;
	char swapped[16];

	switch (sizeof(T)) {
		case 2:	swapped[0]=*((char*)addr+1);
				swapped[1]=*((char*)addr  );
				break;
		case 4:	swapped[0]=*((char*)addr+3);
				swapped[1]=*((char*)addr+2);
				swapped[2]=*((char*)addr+1);
				swapped[3]=*((char*)addr  );
				break;
		case 8:	swapped[0]=*((char*)addr+7);
				swapped[1]=*((char*)addr+6);
				swapped[2]=*((char*)addr+5);
				swapped[3]=*((char*)addr+4);
				swapped[4]=*((char*)addr+3);
				swapped[5]=*((char*)addr+2);
				swapped[6]=*((char*)addr+1);
				swapped[7]=*((char*)addr  );
				break;
		case 16:swapped[0]=*((char*)addr+15);
				swapped[1]=*((char*)addr+14);
				swapped[2]=*((char*)addr+13);
				swapped[3]=*((char*)addr+12);
				swapped[4]=*((char*)addr+11);
				swapped[5]=*((char*)addr+10);
				swapped[6]=*((char*)addr+9);
				swapped[7]=*((char*)addr+8);
				swapped[8]=*((char*)addr+7);
				swapped[9]=*((char*)addr+6);
				swapped[10]=*((char*)addr+5);
				swapped[11]=*((char*)addr+4);
				swapped[12]=*((char*)addr+3);
				swapped[13]=*((char*)addr+2);
				swapped[14]=*((char*)addr+1);
				swapped[15]=*((char*)addr  );
				break;
	}
	return *(T*)swapped;
}

}  // namespace utils
}  // namespace tuttle


#endif // DPX_UTILS_HPP
