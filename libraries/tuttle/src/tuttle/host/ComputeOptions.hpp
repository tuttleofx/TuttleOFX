#ifndef _TUTTLE_HOST_CORE_COMPUTEOPTIONS_HPP_
#define _TUTTLE_HOST_CORE_COMPUTEOPTIONS_HPP_

#include <tuttle/common/utils/global.hpp>

#include <ofxCore.h>

#include <tuttle/common/utils/Formatter.hpp>

#include <tuttle/common/atomic.hpp>
#include <boost/shared_ptr.hpp>

#include <limits>
#include <list>

namespace tuttle
{
namespace host
{

class IProgressHandle
{
public:
    virtual ~IProgressHandle() = 0;

    virtual void beginSequence() {}
    virtual void beginFrame() {}
    virtual void setupAtTime() {}
    virtual void processAtTime() {}
    virtual void endFrame() {}
    virtual void endSequence() {}
};

struct TimeRange
{
    TimeRange()
        : _begin(std::numeric_limits<int>::min())
        , _end(std::numeric_limits<int>::max())
        , _step(1)
    {
    }
    TimeRange(const int frame)
        : _begin(frame)
        , _end(frame)
        , _step(1)
    {
    }
    TimeRange(const int begin, const int end, const int step = 1)
        : _begin(begin)
        , _end(end)
        , _step(step)
    {
    }
    TimeRange(const OfxRangeD& range, const int step = 1);

    int _begin;
    int _end;
    int _step;
};

class ComputeOptions
{
public:
    typedef ComputeOptions This;

    ComputeOptions()
        : _begin(std::numeric_limits<int>::min())
        , _end(std::numeric_limits<int>::max())
        , _abort(false)
    {
        init();
    }

    explicit ComputeOptions(const int frame)
        : _begin(std::numeric_limits<int>::min())
        , _end(std::numeric_limits<int>::max())
        , _abort(false)
    {
        init();
        _timeRanges.push_back(TimeRange(frame, frame));
    }

    ComputeOptions(const int begin, const int end, const int step = 1)
        : _begin(std::numeric_limits<int>::min())
        , _end(std::numeric_limits<int>::max())
        , _abort(false)
    {
        init();
        _timeRanges.push_back(TimeRange(begin, end, step));
    }

    ComputeOptions(const ComputeOptions& options)
        : _begin(std::numeric_limits<int>::min())
        , _end(std::numeric_limits<int>::max())
        , _abort(false)
    {
        *this = options;
    }

    ComputeOptions& operator=(const ComputeOptions& other)
    {
        _timeRanges = other._timeRanges;

        _begin = other._begin;
        _end = other._end;
        _renderScale = other._renderScale;
        _continueOnError = other._continueOnError;
        _continueOnMissingFile = other._continueOnMissingFile;
        _forceIdentityNodesProcess = other._forceIdentityNodesProcess;
        _returnBuffers = other._returnBuffers;
        _isInteractive = other._isInteractive;

        // don't modify the abort status?
        //_abort.store( false, boost::memory_order_relaxed );

        return *this;
    }

private:
    void init()
    {
        setRenderScale(1.0, 1.0);
        setReturnBuffers(true);
        setContinueOnError(false);
        setContinueOnMissingFile(false);
        setColorEnable(false);
        setIsInteractive(false);
        setForceIdentityNodesProcess(false);
    }

public:
    const std::list<TimeRange>& getTimeRanges() const { return _timeRanges; }

    int getBegin() const { return _begin; }
    int getEnd() const { return _end; }

    This& setTimeRange(const int begin, const int end, const int step = 1)
    {
        _timeRanges.clear();
        addTimeRange(begin, end, step);
        return *this;
    }
    This& setTimeRange(const TimeRange& timeRange)
    {
        _timeRanges.clear();
        _timeRanges.push_back(timeRange);
        return *this;
    }
    This& addTimeRange(const int begin, const int end, const int step = 1)
    {
        addTimeRange(TimeRange(begin, end, step));
        return *this;
    }
    This& addTimeRange(const TimeRange& timeRange)
    {
        _timeRanges.push_back(timeRange);
        return *this;
    }

    This& setBegin(const int& beginTime)
    {
        _begin = beginTime;
        return *this;
    }

    This& setEnd(const int& endTime)
    {
        _end = endTime;
        return *this;
    }

    /**
     * @brief To get a preview of the result, you could set a renderscale.
     */
    This& setRenderScale(const double x, const double y)
    {
        _renderScale.x = x;
        _renderScale.y = y;
        return *this;
    }
    const OfxPointD& getRenderScale() const { return _renderScale; }

    /**
     * @brief Continue as much as possible after an error.
     * If an image file inside an image sequence failed to be loaded, we continue to process other images of the sequence.
     */
    This& setContinueOnError(const bool v = true)
    {
        _continueOnError = v;
        return *this;
    }
    bool getContinueOnError() const { return _continueOnError; }

    /**
     * @brief Continue as much as possible after an error.
     * If an image file inside an image sequence failed to be loaded, we continue to process other images of the sequence.
     */
    This& setContinueOnMissingFile(const bool v = true)
    {
        _continueOnMissingFile = v;
        return *this;
    }
    bool getContinueOnMissingFile() const { return _continueOnMissingFile; }

    /**
     * @brief To get output buffer of all output nodes.
     */
    This& setReturnBuffers(const bool v = true)
    {
        _returnBuffers = v;
        return *this;
    }
    bool getReturnBuffers() const { return _returnBuffers; }

    /**
     * @brief Set the output color enabled or not.
     */
    This& setColorEnable(const bool enable = true)
    {
        enable ? tuttle::common::Color::get()->enable() : tuttle::common::Color::get()->disable();
        return *this;
    }

    /**
     * @brief Inform plugins about the kind of process: batch or interactive.
     */
    This& setIsInteractive(const bool v = true)
    {
        _isInteractive = v;
        return *this;
    }
    bool getIsInteractive() const { return _isInteractive; }

    /**
     * @brief For debug puposes only, you could force to call the process on all identity nodes.
     * This case should never happens to the plugin, so it may fail to do it.
     */
    This& setForceIdentityNodesProcess(const bool v = true)
    {
        _forceIdentityNodesProcess = v;
        return *this;
    }
    bool getForceIdentityNodesProcess() const { return _forceIdentityNodesProcess; }

    /**
     * @brief The application would like to abort the process (from another thread).
     */
    void abort() { _abort.store(true, boost::memory_order_relaxed); }
    /**
     * @brief Has someone asked to abort the process?
     */
    bool getAbort() const { return _abort.load(boost::memory_order_relaxed); }

    /**
    * @brief A handle to follow the progress (start, end...) of the compute
    */
    void setProgressHandle(boost::shared_ptr<IProgressHandle> progressHandle) { _progressHandle = progressHandle; }
    bool isProgressHandleSet() const { return _progressHandle.get() != NULL; }
    void beginSequenceHandle() const
    {
        if(isProgressHandleSet())
            _progressHandle->beginSequence();
    }
    void beginFrameHandle() const
    {
        if(isProgressHandleSet())
            _progressHandle->beginFrame();
    }
    void setupAtTimeHandle() const
    {
        if(isProgressHandleSet())
            _progressHandle->setupAtTime();
    }
    void processAtTimeHandle() const
    {
        if(isProgressHandleSet())
            _progressHandle->processAtTime();
    }
    void endFrameHandle() const
    {
        if(isProgressHandleSet())
            _progressHandle->endFrame();
    }
    void endSequenceHandle() const
    {
        if(isProgressHandleSet())
            _progressHandle->endSequence();
    }

private:
    std::list<TimeRange> _timeRanges;

    OfxPointD _renderScale;
    // different to range
    int _begin;
    int _end;

    bool _continueOnError;
    bool _continueOnMissingFile;
    bool _forceIdentityNodesProcess;
    bool _returnBuffers;
    bool _isInteractive;

    boost::atomic_bool _abort;

    boost::shared_ptr<IProgressHandle> _progressHandle;
};
}
}

#endif
