#ifndef _TUTTLE_HOST_LINKDATA_HPP_
#define _TUTTLE_HOST_LINKDATA_HPP_

#include "MemoryPool.hpp"

namespace tuttle
{
namespace host
{
namespace memory
{

/**
 * @brief A link to an external buffer which can't be managed by the MemoryPool.
 */
class LinkData : public IPoolData
{
    LinkData();
    LinkData(const LinkData&);

public:
    LinkData(char* dataLink)
        : _dataLink(dataLink)
    {
    }

    ~LinkData()
    {
        // we don't own _dataLink
    }

    char* data() { return _dataLink; }
    const char* data() const { return _dataLink; }

    const size_t size() const { return 0; }
    const size_t reservedSize() const { return 0; }

    void addRef() {}
    void release() {}

private:
    char* const _dataLink;
};
}
}
}

#endif
