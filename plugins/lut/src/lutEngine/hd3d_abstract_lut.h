#ifndef ABSTRACT_LUT_H
#define ABSTRACT_LUT_H

#include "lut_reader.h"
#include "hd3d_color.h"

namespace tuttle
{
class Interpolator;
class TrilinInterpolator;

class AbstractLut {
    protected:
        size_t        _dimSize;
        double       *_data;
        bool          _shared;
        Interpolator *_interpolator;

    public:
        AbstractLut();
        AbstractLut( Interpolator * interpolator, size_t dimSize, double *data = NULL );
        AbstractLut( Interpolator * interpolator, LutReader & reader );
        virtual ~AbstractLut();

        void reset( Interpolator *interpolator, LutReader & reader );
        void reset( LutReader & reader );
        void reset( size_t dimSize, double *data = NULL );

        inline  const size_t & dimSize() const { return _dimSize; }
        double *getData() { return _data; }
        virtual const size_t totalSize() const = 0;

        virtual Color getIndexedColor(int _x, int _y, int _z) const = 0;
        virtual Color getColor(Color const &color) const = 0;
        virtual Color getColor(double _r, double _g, double _b) const = 0;
        virtual void  setIndexedColor(int _x, int _y, int _z, Color _color ) = 0;
        virtual void  setIndexedValues(int _x, int _y, int _z, double _r, double _g, double _b) = 0;
};

};

#endif //ABSTRACT_LUT_H
