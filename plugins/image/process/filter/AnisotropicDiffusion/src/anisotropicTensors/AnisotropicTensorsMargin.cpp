#include "AnisotropicTensorsMargin.hpp"
#include "AnisotropicTensorsPlugin.hpp"

#include <tuttle/plugin/opengl/gl.h>

#include <ofxsImageEffect.h>
#include <ofxsInteract.h>
#include <boost/gil/gil_all.hpp>

namespace tuttle
{
namespace plugin
{
namespace anisotropicFilter
{
namespace tensors
{

bool TensorsMargin::draw(const OFX::DrawArgs& args)
{
    TensorsPlugin* plugin = dynamic_cast<TensorsPlugin*>(_effect);
    if(plugin && plugin->_paramDisplayMargin->getValue())
    {
        OfxRectD rect[2];
        rect[0] = plugin->_renderRect;
        rect[1] = plugin->_overSizedRect;
        // get the project size
        OfxPointD projSize, projOffset;
        projOffset = _effect->getProjectOffset();
        projSize = _effect->getProjectSize();

        glColor3f(1.0f, 1.0f, 1.0f);
        glPushMatrix();
        for(int i = 0; i < 2; i++)
        {
            double x1 = projOffset.x + rect[i].x1;
            double y1 = projOffset.y + rect[i].y1;
            double x2 = projOffset.x + rect[i].x2;
            double y2 = projOffset.y + rect[i].y2;
            if(i >= 1)
                glColor3f(1.0f, 1.00f, 0.0f);

            glBegin(GL_LINES);

            glVertex2f(x1, y1);
            glVertex2f(x2, y1);

            glVertex2f(x1, y2);
            glVertex2f(x2, y2);

            glVertex2f(x1, y1);
            glVertex2f(x1, y2);

            glVertex2f(x2, y1);
            glVertex2f(x2, y2);

            glEnd();
        }
        glPopMatrix();
        return true; ///! @todo: true or false cf. doc openfx
    }
    return false;
}
}
}
}
}
