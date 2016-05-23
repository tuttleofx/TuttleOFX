#include "OfxhClipImageSet.hpp"

#include <tuttle/host/ofx/OfxhImageEffectNodeDescriptor.hpp>

#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace tuttle
{
namespace host
{
namespace ofx
{
namespace attribute
{

OfxhClipImageSet::OfxhClipImageSet()
    : _clipPrefsDirty(true)
{
}

OfxhClipImageSet::OfxhClipImageSet(const OfxhClipImageSet& other)
    : _clipsByOrder(other._clipsByOrder.clone())
    , _clipPrefsDirty(true)
{
    initMapFromList();
}

void OfxhClipImageSet::initMapFromList()
{
    for(ClipImageVector::iterator it = _clipsByOrder.begin(), itEnd = _clipsByOrder.end(); it != itEnd; ++it)
    {
        _clipImages[it->getName()] = &(*it);
    }
}

OfxhClipImageSet::~OfxhClipImageSet()
{
}

bool OfxhClipImageSet::operator==(const This& other) const
{
    return _clipsByOrder == other._clipsByOrder;
}

void OfxhClipImageSet::copyClipsValues(const OfxhClipImageSet& other)
{
    if(_clipsByOrder.size() != other._clipsByOrder.size())
    {
        BOOST_THROW_EXCEPTION(exception::Bug()
                              << exception::dev("You try to copy clips values, but the two lists are not identical."));
    }

    ClipImageVector::const_iterator oit = other._clipsByOrder.begin(), oitEnd = other._clipsByOrder.end();
    for(ClipImageVector::iterator it = _clipsByOrder.begin(), itEnd = _clipsByOrder.end(); it != itEnd && oit != oitEnd;
        ++it, ++oit)
    {
        OfxhClipImage& c = *it;
        const OfxhClipImage& oc = *oit;
        if(c.getName() != oc.getName())
        {
            BOOST_THROW_EXCEPTION(exception::Bug() << exception::dev(
                                      "You try to copy clips values, but it is not the same clips in the two lists."));
        }
        c.copyValues(oc);
    }
}

void OfxhClipImageSet::populateClips(const imageEffect::OfxhImageEffectNodeDescriptor& descriptor) OFX_EXCEPTION_SPEC
{
    const imageEffect::OfxhImageEffectNodeDescriptor::ClipImageDescriptorVector& clips = descriptor.getClipsByOrder();

    _clipsByOrder.reserve(clips.size());
    /// @todo tuttle don't manipulate clip here, delegate to ClipInstanceSet
    for(imageEffect::OfxhImageEffectNodeDescriptor::ClipImageDescriptorVector::const_iterator it = clips.begin(),
                                                                                              itEnd = clips.end();
        it != itEnd; ++it)
    {
        const std::string& name = it->getName();
        // foreach clip descriptor make a ClipImageInstance
        OfxhClipImage* instance = newClipImage(*it); //( this, *it, counter );
        if(!instance)
            BOOST_THROW_EXCEPTION(exception::Bug() << exception::dev("Error on ClipImage creation."));

        _clipsByOrder.push_back(instance);
        _clipImages[name] = instance;
    }
}

OfxhClipImage& OfxhClipImageSet::getClip(const std::string& name, const bool acceptPartialName)
{
    ClipImageMap::const_iterator it = _clipImages.find(name);

    if(it != _clipImages.end())
        return *it->second;

    std::vector<std::string> matches;
    OfxhClipImage* res = NULL;
    if(acceptPartialName)
    {
        BOOST_FOREACH(ClipImageMap::value_type& p, _clipImages)
        {
            if(boost::algorithm::starts_with(p.first, name))
            {
                matches.push_back(p.first);
                res = p.second;
            }
        }
        if(matches.size() > 1)
        {
            BOOST_THROW_EXCEPTION(exception::Value() << exception::user() + "Ambiguous partial clip name \"" + name +
                                                            "\". Possible values are: " +
                                                            boost::algorithm::join(matches, ", ") + ".");
        }
    }

    if(matches.size() == 0)
    {
        std::ostringstream ss;
        ss << "Clip not found (" << name << ").\n";
        ss << "List of existing clips [";
        BOOST_FOREACH(const ClipImageMap::value_type& c, _clipImages)
        {
            ss << "(\"" << c.first << "\":\"" << c.second->getClipIdentifier() << "\"), ";
        }
        ss << "].\n";
        BOOST_THROW_EXCEPTION(OfxhException(ss.str()));
    }
    return *res;
}

OfxhClipImage* OfxhClipImageSet::getClipPtr(const std::string& name, const bool acceptPartialName)
{
    try
    {
        return &this->getClip(name, acceptPartialName);
    }
    catch(...)
    {
        return NULL;
    }
}

const OfxhClipImage* OfxhClipImageSet::getClipPtr(const std::string& name, const bool acceptPartialName) const
{
    try
    {
        return &this->getClip(name, acceptPartialName);
    }
    catch(...)
    {
        return NULL;
    }
}
}
}
}
}
