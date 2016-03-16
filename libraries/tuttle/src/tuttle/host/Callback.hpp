#ifndef _CALLBACK_HPP_
#define _CALLBACK_HPP_

namespace tuttle
{
namespace host
{

class INode;

/** Used to store a callback object, atm, this object is triggered
 * before the render process **/
class Callback
{
public:
    Callback(){};
    virtual ~Callback(){};

    // Propagate the call to inherited classes
    virtual void call(INode&, double ofxtime) = 0;
};
};
};

#endif //_CALLBACK_HPP_
