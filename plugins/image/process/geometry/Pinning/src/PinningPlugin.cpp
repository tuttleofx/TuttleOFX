#include "PinningPlugin.hpp"
#include "PinningProcess.hpp"
#include "PinningDefinitions.hpp"

#include <tuttle/plugin/global.hpp>

#include <ofxsImageEffect.h>
#include <ofxsMultiThread.h>

#include <boost/gil/gil_all.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace tuttle
{
namespace plugin
{
namespace pinning
{

PinningPlugin::PinningPlugin(OfxImageEffectHandle handle)
    : SamplerPlugin(handle)
{
    _clipSrc = fetchClip(kOfxImageEffectSimpleSourceClipName);
    _clipDst = fetchClip(kOfxImageEffectOutputClipName);

    _paramMethod = fetchChoiceParam(kParamMethod);
    //	_paramManipulatorMode = fetchChoiceParam( kParamManipulatorMode );
    _paramSetToCornersIn = fetchPushButtonParam(kParamSetToCornersIn);
    _paramSetToCornersOut = fetchPushButtonParam(kParamSetToCornersOut);
    _paramOverlay = fetchBooleanParam(kParamOverlay);
    _paramInverse = fetchBooleanParam(kParamInverse);

    /*
    //TODO-vince //
    _paramGroupCentre       = fetchGroupParam( kParamGroupCentre );
    _paramPointCentre       = fetchDouble2DParam( kParamPointCentre);
    _paramOverlayCentre      = fetchBooleanParam( kParamOverlayCentre );
    _paramOverlayCentreColor = fetchRGBParam( kParamOverlayCentreColor );
    ///////////////////////
    */

    _paramGroupIn = fetchGroupParam(kParamGroupIn);
    _paramPointIn0 = fetchDouble2DParam(kParamPointIn + "0");
    _paramPointIn1 = fetchDouble2DParam(kParamPointIn + "1");
    _paramPointIn2 = fetchDouble2DParam(kParamPointIn + "2");
    _paramPointIn3 = fetchDouble2DParam(kParamPointIn + "3");
    _paramOverlayIn = fetchBooleanParam(kParamOverlayIn);
    _paramOverlayInColor = fetchRGBParam(kParamOverlayInColor);

    _paramGroupOut = fetchGroupParam(kParamGroupIn);
    _paramPointOut0 = fetchDouble2DParam(kParamPointOut + "0");
    _paramPointOut1 = fetchDouble2DParam(kParamPointOut + "1");
    _paramPointOut2 = fetchDouble2DParam(kParamPointOut + "2");
    _paramPointOut3 = fetchDouble2DParam(kParamPointOut + "3");
    _paramOverlayOut = fetchBooleanParam(kParamOverlayOut);
    _paramOverlayOutColor = fetchRGBParam(kParamOverlayOutColor);

    _paramGroupPerspMatrix = fetchGroupParam(kParamGroupPerspMatrix);
    _paramPerspMatrixRow0 = fetchDouble3DParam(kParamPerspMatrixRow + "0");
    _paramPerspMatrixRow1 = fetchDouble3DParam(kParamPerspMatrixRow + "1");
    _paramPerspMatrixRow2 = fetchDouble3DParam(kParamPerspMatrixRow + "2");

    _paramGroupBilMatrix = fetchGroupParam(kParamGroupBilinearMatrix);
    _paramBilMatrixRow0 = fetchDouble2DParam(kParamBilinearMatrixRow + "0");
    _paramBilMatrixRow1 = fetchDouble2DParam(kParamBilinearMatrixRow + "1");
    _paramBilMatrixRow2 = fetchDouble2DParam(kParamBilinearMatrixRow + "2");
    _paramBilMatrixRow3 = fetchDouble2DParam(kParamBilinearMatrixRow + "3");

    changedParam(OFX::InstanceChangedArgs(), kParamMethod);
    changedParam(OFX::InstanceChangedArgs(), kParamFilter);
}

PinningProcessParams<PinningPlugin::Scalar> PinningPlugin::getProcessParams(const OfxTime time,
                                                                            const OfxPointD& renderScale) const
{
    using namespace boost::numeric::ublas;
    PinningProcessParams<Scalar> params;

    const OfxRectD rod = _clipSrc->getCanonicalRod(time, renderScale);
    const double width = rod.x2 - rod.x1;
    const double height = rod.y2 - rod.y1;

    // persp matrix
    bounded_matrix<double, 3, 3> pm;
    _paramPerspMatrixRow0->getValue(pm(0, 0), pm(0, 1), pm(0, 2));
    _paramPerspMatrixRow1->getValue(pm(1, 0), pm(1, 1), pm(1, 2));
    _paramPerspMatrixRow2->getValue(pm(2, 0), pm(2, 1), pm(2, 2));
    params._perspective._matrix = pm;
    params._perspective._width = width;
    params._perspective._height = height;

    // bilinear matrix
    bounded_matrix<double, 2, 4> bm;
    _paramBilMatrixRow0->getValue(bm(0, 0), bm(1, 0));
    _paramBilMatrixRow1->getValue(bm(0, 1), bm(1, 1));
    _paramBilMatrixRow2->getValue(bm(0, 2), bm(1, 2));
    _paramBilMatrixRow3->getValue(bm(0, 3), bm(1, 3));
    params._bilinear._matrix = bm;
    params._bilinear._width = width;
    params._bilinear._height = height;

    params._method = static_cast<EParamMethod>(_paramMethod->getValue());

    SamplerPlugin::fillProcessParams(params._samplerProcessParams);

    return params;
}

void PinningPlugin::changedParam(const OFX::InstanceChangedArgs& args, const std::string& paramName)
{
    using namespace boost::numeric::ublas;

    bounded_vector<double, 2> pOut[4];
    bounded_vector<double, 2> pIn[4];

    SamplerPlugin::changedParam(args, paramName);

    if(_paramInverse->getValue())
    {
        _paramPointIn0->getValue(pOut[0][0], pOut[0][1]);
        _paramPointIn1->getValue(pOut[1][0], pOut[1][1]);
        _paramPointIn2->getValue(pOut[2][0], pOut[2][1]);
        _paramPointIn3->getValue(pOut[3][0], pOut[3][1]);

        _paramPointOut0->getValue(pIn[0][0], pIn[0][1]);
        _paramPointOut1->getValue(pIn[1][0], pIn[1][1]);
        _paramPointOut2->getValue(pIn[2][0], pIn[2][1]);
        _paramPointOut3->getValue(pIn[3][0], pIn[3][1]);
    }
    else
    {
        _paramPointOut0->getValue(pOut[0][0], pOut[0][1]);
        _paramPointOut1->getValue(pOut[1][0], pOut[1][1]);
        _paramPointOut2->getValue(pOut[2][0], pOut[2][1]);
        _paramPointOut3->getValue(pOut[3][0], pOut[3][1]);

        _paramPointIn0->getValue(pIn[0][0], pIn[0][1]);
        _paramPointIn1->getValue(pIn[1][0], pIn[1][1]);
        _paramPointIn2->getValue(pIn[2][0], pIn[2][1]);
        _paramPointIn3->getValue(pIn[3][0], pIn[3][1]);
    }

    if(paramName == kParamSetToCornersIn)
    {
        _paramPointIn0->setValue(-0.5, -0.5);
        _paramPointIn1->setValue(0.5, -0.5);
        _paramPointIn2->setValue(-0.5, 0.5);
        _paramPointIn3->setValue(0.5, 0.5);
    }
    else if(paramName == kParamSetToCornersOut)
    {
        _paramPointOut0->setValue(-0.5, -0.5);
        _paramPointOut1->setValue(0.5, -0.5);
        _paramPointOut2->setValue(-0.5, 0.5);
        _paramPointOut3->setValue(0.5, 0.5);
    }
    else if(paramName == kParamMethod)
    {
        bool bil = false;
        bool persp = false;
        bool fourPoints = false;

        switch(static_cast<EParamMethod>(_paramMethod->getValue()))
        {
            case eParamMethodAffine:
            {
                persp = true;
                break;
            }
            case eParamMethodPerspective:
            {
                fourPoints = true;
                persp = true;
                break;
            }
            case eParamMethodBilinear:
            {
                fourPoints = true;
                bil = true;
                break;
            }
        }

        _paramPointIn3->setIsSecretAndDisabled(!fourPoints);
        _paramPointOut3->setIsSecretAndDisabled(!fourPoints);

        _paramGroupPerspMatrix->setIsSecretAndDisabled(!persp);
        _paramPerspMatrixRow0->setIsSecretAndDisabled(!persp);
        _paramPerspMatrixRow1->setIsSecretAndDisabled(!persp);
        _paramPerspMatrixRow2->setIsSecretAndDisabled(!persp);

        _paramGroupBilMatrix->setIsSecretAndDisabled(!bil);
        _paramBilMatrixRow0->setIsSecretAndDisabled(!bil);
        _paramBilMatrixRow1->setIsSecretAndDisabled(!bil);
        _paramBilMatrixRow2->setIsSecretAndDisabled(!bil);
        _paramBilMatrixRow3->setIsSecretAndDisabled(!bil);

        // recompute the matrix
        changedParam(args, kParamPointIn);
    }
    else if(boost::starts_with(paramName, kParamPointIn) || boost::starts_with(paramName, kParamPointOut) ||
            paramName == kParamInverse)
    {
        switch(static_cast<EParamMethod>(_paramMethod->getValue()))
        {
            case eParamMethodAffine:
            {
                bounded_matrix<Scalar, 3, 3> perspMatrix;

                ////////////////////////////////////////
                // compute "perspMatrix" from input/output points -> "_paramPoint*"
                perspMatrix = identity_matrix<Scalar>(3);

                /* Calculates coefficients of affine transformation
                 * which maps (xi,yi) to (ui,vi), (i=1,2,3):
                 *
                 * ui = c00*xi + c01*yi + c02
                 *
                 * vi = c10*xi + c11*yi + c12
                 *
                 * Coefficients are calculated by solving linear system:
                 * / x0 y0  1  0  0  0 \ /c00\ /u0\
                 * | x1 y1  1  0  0  0 | |c01| |u1|
                 * | x2 y2  1  0  0  0 | |c02| |u2|
                 * |  0  0  0 x0 y0  1 | |c10| |v0|
                 * |  0  0  0 x1 y1  1 | |c11| |v1|
                 * \  0  0  0 x2 y2  1 / |c12| |v2|
                 *
                 * where:
                 *   cij - matrix coefficients
                 */
                static const int n = 6;
                permutation_matrix<double> P(n);
                matrix<double> A(n, n);
                vector<double> x(n);
                vector<double> b(n);

                /////////////////////
                // fill A and b... //
                for(int i = 0; i < 3; ++i)
                {
                    A(i, 0) = A(i + 3, 0 + 3) = pOut[i][0];
                    A(i, 1) = A(i + 3, 1 + 3) = pOut[i][1];
                    b(i) = pIn[i][0];
                    b(i + 3) = pIn[i][1];
                }
                subrange(A, 3, 6, 0, 3) = subrange(A, 0, 3, 3, 6) = zero_matrix<double>(n);
                subrange(A, 0, 3, 2, 3) = subrange(A, 3, 6, 5, 6) = scalar_matrix<double>(3, 1, 1);

                // TUTTLE_LOG_VAR( TUTTLE_INFO, A );

                lu_factorize(A, P);
                // Now A and P contain the LU factorization of A
                x = b;
                lu_substitute(A, P, x);
                // Now x contains the solution.

                _paramPerspMatrixRow0->setValue(x(0), x(1), x(2));
                _paramPerspMatrixRow1->setValue(x(3), x(4), x(5));
                _paramPerspMatrixRow2->setValue(0, 0, 1);
                break;
            }
            case eParamMethodPerspective:
            {
                bounded_matrix<double, 3, 3> perspMatrix;

                ////////////////////////////////////////
                // compute "perspMatrix" from input/output points -> "_paramPoint*"
                perspMatrix = identity_matrix<Scalar>(3);
                ////////////////////////////////////////
                /* Calculates coefficients of perspective transformation
                 * which maps (xi,yi) to (ui,vi), (i=1,2,3,4):
                 *
                 *      c00*xi + c01*yi + c02
                 * ui = ---------------------
                 *      c20*xi + c21*yi + c22
                 *
                 * 	    c10*xi + c11*yi + c12
                 * vi = ---------------------
                 * 	    c20*xi + c21*yi + c22
                 *
                 * Coefficients are calculated by solving linear system:
                 * / x0 y0 1 0 0 0 -x0*u0 -y0*u0 \ /c00\ /u0\
                 * | x1 y1 1 0 0 0 -x1*u1 -y1*u1 | |c01| |u1|
                 * | x2 y2 1 0 0 0 -x2*u2 -y2*u2 | |c02| |u2|
                 * | x3 y3 1 0 0 0 -x3*u3 -y3*u3 |.|c10|=|u3|,
                 * | 0 0 0 x0 y0 1 -x0*v0 -y0*v0 | |c11| |v0|
                 * | 0 0 0 x1 y1 1 -x1*v1 -y1*v1 | |c12| |v1|
                 * | 0 0 0 x2 y2 1 -x2*v2 -y2*v2 | |c20| |v2|
                 * \ 0 0 0 x3 y3 1 -x3*v3 -y3*v3 / \c21/ \v3/
                 *
                 * where:
                 * cij - matrix coefficients, c22 = 1
                 */

                static const int n = 8;
                permutation_matrix<double> P(n);
                matrix<double> A(n, n);
                vector<double> x(n);
                vector<double> b(n);

                /////////////////////
                // fill A and b... //
                for(int i = 0; i < 4; ++i)
                {
                    A(i, 0) = A(i + 4, 3) = pOut[i][0];
                    A(i, 1) = A(i + 4, 4) = pOut[i][1];
                    A(i, 2) = A(i + 4, 5) = 1.0;
                    A(i, 3) = A(i, 4) = A(i, 5) = A(i + 4, 0) = A(i + 4, 1) = A(i + 4, 2) = 0.0;

                    A(i, 6) = -(pOut[i][0]) * (pIn[i][0]);
                    A(i, 7) = -(pOut[i][1]) * (pIn[i][0]);
                    A(i + 4, 6) = -(pOut[i][0]) * (pIn[i][1]);
                    A(i + 4, 7) = -(pOut[i][1]) * (pIn[i][1]);

                    b(i) = pIn[i][0];
                    b(i + 4) = pIn[i][1];
                }
                // TUTTLE_LOG_VAR( TUTTLE_INFO, A );

                lu_factorize(A, P);
                // Now A and P contain the LU factorization of A
                x = b;
                lu_substitute(A, P, x);
                // Now x contains the solution.

                _paramPerspMatrixRow0->setValue(x(0), x(1), x(2));
                _paramPerspMatrixRow1->setValue(x(3), x(4), x(5));
                _paramPerspMatrixRow2->setValue(x(6), x(7), 1);

                break;
            }
            case eParamMethodBilinear:
            {
                bounded_matrix<Scalar, 2, 4> bilMatrix;
                ////////////////////////////////////////
                /// compute "bilMatrix" from input/output points -> "_paramPoint*"
                bilMatrix(0, 0) = 1.0;
                bilMatrix(0, 1) = 0.0;
                bilMatrix(0, 2) = 0.0;
                bilMatrix(0, 3) = 0.0;
                bilMatrix(1, 0) = 0.0;
                bilMatrix(1, 1) = 1.0;
                bilMatrix(1, 2) = 0.0;
                bilMatrix(1, 3) = 2.0;
                ////////////////////////////////////////
                /* Coefficients are calculated by solving linear system:
                 * / x0  y0   x0y0  1  0    0    0   0 \
                 * | 0   0  0  0    0  x0  y0  x0y0  1 |
                 * | x1  y1   x1y1  1  0    0    0   0 |
                 * | 0   0  0  0    0  x1  y1  x0y1  1 |
                 * | x2  y2   x2y2  1  0    0    0   0 |
                 * | 0   0  0  0    0  x2  y2  x2y2  1 |
                 * | x3  y3   x3y3  1  0    0    0   0 |
                 * \ 0   0  0  0    0  x3  y3  x3y3  1 /
                 */
                // Recuperation des points IN et OUT

                static const int n = 8;
                permutation_matrix<double> P(n);
                matrix<double> A(n, n);
                vector<double> c(n);
                vector<double> b(n);

                for(int i = 0; i < 4; ++i)
                {
                    A(2 * i, 0) = A(2 * i + 1, 0 + 4) = pOut[i][0];
                    A(2 * i, 1) = A(2 * i + 1, 1 + 4) = pOut[i][1];
                    A(2 * i, 2) = A(2 * i + 1, 2 + 4) = pOut[i][0] * pOut[i][1];
                    A(2 * i, 3) = A(2 * i + 1, 3 + 4) = 1;

                    A(2 * i, 4) = A(2 * i, 5) = A(2 * i, 6) = A(2 * i, 7) = 0;
                    A(2 * i + 1, 0) = A(2 * i + 1, 1) = A(2 * i + 1, 2) = A(2 * i + 1, 3) = 0;

                    b(i * 2) = pIn[i][0];
                    b(i * 2 + 1) = pIn[i][1];
                }

                lu_factorize(A, P);
                // Now A and P contain the LU factorization of A
                c = b;

                lu_substitute(A, P, c);
                // Now bilMatrix contains the solution.

                _paramBilMatrixRow0->setValue(c(0), c(4));
                _paramBilMatrixRow1->setValue(c(1), c(5));
                _paramBilMatrixRow2->setValue(c(2), c(6));
                _paramBilMatrixRow3->setValue(c(3), c(7));

                break;
            }
        }
    }
}

bool PinningPlugin::isIdentity(const OFX::RenderArguments& args, OFX::Clip*& identityClip, double& identityTime)
{
    return false; /// @todo remove this.
    using namespace boost::numeric::ublas;
    PinningProcessParams<Scalar> params = getProcessParams(args.time, args.renderScale);
    bool identity = false;

    // is the transformation matrix is an identity matrix the node is identity,
    // we perform no modification on the input image.
    switch(params._method)
    {
        case eParamMethodAffine:
        case eParamMethodPerspective:
        {
            if(norm_inf(params._perspective._matrix - identity_matrix<Scalar>(3)) == 0)
            {
                identity = true;
            }
            break;
        }
        case eParamMethodBilinear:
        {
            if(params._bilinear._matrix(0, 0) == 1.0 && params._bilinear._matrix(0, 1) == 0.0 &&
               params._bilinear._matrix(0, 2) == 0.0 && params._bilinear._matrix(0, 3) == 0.0 &&
               params._bilinear._matrix(1, 0) == 0.0 && params._bilinear._matrix(1, 1) == 1.0 &&
               params._bilinear._matrix(1, 2) == 0.0 && params._bilinear._matrix(1, 3) == 0.0)
            {
                identity = true;
            }
            break;
        }
    }
    identityClip = _clipSrc;
    identityTime = args.time;
    return identity;
}

/**
 * @brief The overridden render function
 * @param[in]   args     Rendering parameters
 */
void PinningPlugin::render(const OFX::RenderArguments& args)
{
    using namespace boost::gil;
    // instantiate the render code based on the pixel depth of the dst clip
    OFX::EBitDepth dstBitDepth = _clipDst->getPixelDepth();
    OFX::EPixelComponent dstComponents = _clipDst->getPixelComponents();

    // do the rendering
    if(dstComponents == OFX::ePixelComponentRGBA)
    {
        switch(dstBitDepth)
        {
            case OFX::eBitDepthUByte:
            {
                PinningProcess<rgba8_view_t> p(*this);
                p.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthUShort:
            {
                PinningProcess<rgba16_view_t> p(*this);
                p.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthFloat:
            {
                PinningProcess<rgba32f_view_t> p(*this);
                p.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthNone:
            case OFX::eBitDepthCustom:
            {
                TUTTLE_LOG_ERROR("Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin.");
                break;
            }
        }
    }
    else if(dstComponents == OFX::ePixelComponentAlpha)
    {
        switch(dstBitDepth)
        {
            case OFX::eBitDepthUByte:
            {
                PinningProcess<gray8_view_t> p(*this);
                p.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthUShort:
            {
                PinningProcess<gray16_view_t> p(*this);
                p.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthFloat:
            {
                PinningProcess<gray32f_view_t> p(*this);
                p.setupAndProcess(args);
                break;
            }
            case OFX::eBitDepthNone:
            case OFX::eBitDepthCustom:
            {
                TUTTLE_LOG_ERROR("Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin.");
                break;
            }
        }
    }
    else
    {
        TUTTLE_LOG_ERROR("Bit depth (" << mapBitDepthEnumToString(dstBitDepth) << ") not recognized by the plugin.");
    }
}
}
}
}
