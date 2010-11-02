#include "core.hpp"

#include <ofxImageEffect.h>

#include <boost/lexical_cast.hpp>

namespace tuttle {
namespace ofx {

std::string mapStatusToString( const OfxStatus stat )
{
	switch( stat )
	{
		case kOfxStatOK: return "kOfxStatOK";
		case kOfxStatFailed: return "kOfxStatFailed";
		case kOfxStatErrFatal: return "kOfxStatErrFatal";
		case kOfxStatErrUnknown: return "kOfxStatErrUnknown";
		case kOfxStatErrMissingHostFeature: return "kOfxStatErrMissingHostFeature";
		case kOfxStatErrUnsupported: return "kOfxStatErrUnsupported";
		case kOfxStatErrExists: return "kOfxStatErrExists";
		case kOfxStatErrFormat: return "kOfxStatErrFormat";
		case kOfxStatErrMemory: return "kOfxStatErrMemory";
		case kOfxStatErrBadHandle: return "kOfxStatErrBadHandle";
		case kOfxStatErrBadIndex: return "kOfxStatErrBadIndex";
		case kOfxStatErrValue: return "kOfxStatErrValue";
		case kOfxStatReplyYes: return "kOfxStatReplyYes";
		case kOfxStatReplyNo: return "kOfxStatReplyNo";
		case kOfxStatReplyDefault: return "kOfxStatReplyDefault";
		case kOfxStatErrImageFormat: return "kOfxStatErrImageFormat";
	}
	return "UNKNOWN STATUS CODE: " + boost::lexical_cast<std::string>( stat );
}

}
}

std::ostream& operator<<( std::ostream& os, const OfxPlugin& v )
{
	os << "OfxPlugin {" << std::endl;
	os << "  pluginApi" << v.pluginApi << std::endl;
	os << "  apiVersion" << v.apiVersion << std::endl;
	os << "  pluginIdentifier" << v.pluginIdentifier << std::endl;
	os << "  pluginVersionMajor" << v.pluginVersionMajor << std::endl;
	os << "  pluginVersionMinor" << v.pluginVersionMinor << std::endl;
	os << "}" << std::endl;
	return os;
}

/*
   typedef struct OfxRangeI
   {
    int min, max;
   } OfxRangeI;

   typedef struct OfxRangeD
   {
    double min, max;
   } OfxRangeD;

   typedef struct OfxPointI
   {
    int x, y;
   } OfxPointI;

   typedef struct OfxPointD
   {
    double x, y;
   } OfxPointD;

   typedef struct OfxRectI
   {
    int x1, y1, x2, y2;
   } OfxRectI;

   typedef struct OfxRectD
   {
    double x1, y1, x2, y2;
   } OfxRectD;

   typedef struct OfxRGBAColourB
   {
    unsigned char r, g, b, a;
   }OfxRGBAColourB;

   typedef struct OfxRGBAColourS
   {
    unsigned short r, g, b, a;
   }OfxRGBAColourS;

   typedef struct OfxRGBAColourF
   {
    float r, g, b, a;
   }OfxRGBAColourF;

   typedef struct OfxRGBAColourD
   {
    double r, g, b, a;
   }OfxRGBAColourD;

   struct OfxRGBColourB
   {
    unsigned char r, g, b;
   };

   struct OfxRGBColourS
   {
    unsigned short r, g, b;
   };

   struct OfxRGBColourF
   {
    float r, g, b;
   };

   struct OfxRGBColourD
   {
    double r, g, b;
   };

   struct Ofx3DPointI
   {
    int x, y, z;
   };

   struct Ofx3DPointD
   {
    double x, y, z;
   };

   typedef struct OfxYUVAColourB
   {
    unsigned char y, u, v, a;
   }OfxYUVAColourB;

   typedef struct OfxYUVAColourS
   {
    unsigned short y, u, v, a;
   }OfxYUVAColourS;

   typedef struct OfxYUVAColourF
   {
    float y, u, v, a;
   }OfxYUVAColourF;
 */

