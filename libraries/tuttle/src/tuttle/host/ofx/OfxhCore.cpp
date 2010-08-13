#include "OfxhCore.hpp"

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

std::ostream& operator<<( std::ostream& out, const OfxPointI& p )
{
	return out << "x:" << p.x << " y:" << p.y;
}

std::ostream& operator<<( std::ostream& out, const OfxPointD& p )
{
	return out << "x:" << p.x << " y:" << p.y;
}

std::ostream& operator<<( std::ostream& out, const OfxRangeI& r )
{
	return out << "min:" << r.min << " max:" << r.max;
}

std::ostream& operator<<( std::ostream& out, const OfxRangeD& r )
{
	return out << "min:" << r.min << " max:" << r.max;
}

std::ostream& operator<<( std::ostream& out, const OfxRectI& r )
{
	return out << "x1:" << r.x1 << " y1:" << r.y1 << " x2:" << r.x2 << " y2:" << r.y2;
}

std::ostream& operator<<( std::ostream& out, const OfxRectD& r )
{
	return out << "x1:" << r.x1 << " y1:" << r.y1 << " x2:" << r.x2 << " y2:" << r.y2;
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

