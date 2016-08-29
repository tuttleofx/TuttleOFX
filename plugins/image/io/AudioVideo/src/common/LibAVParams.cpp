#include "LibAVParams.hpp"
#include "LibAVFeaturesAvailable.hpp"

#include <writer/AVWriterDefinitions.hpp>

extern "C" {
#include <libavformat/avformat.h>
}

#include <boost/foreach.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/bind/bind.hpp>

#include <iostream>
#include <limits>

namespace tuttle
{
namespace plugin
{
namespace av
{
namespace common
{

LibAVParams::LibAVParams(const std::string& prefixScope, const int flags, const bool isDetailed)
    : _paramOFX()
    , _paramFlagOFXPerOption()
    , _childsPerChoice()
    , _avOptions()
    , _avContext(NULL)
{
    if(!isDetailed)
    {
        if(prefixScope == kPrefixFormat)
        {
            // Allocate an AVFormatContext
            _avContext = avformat_alloc_context();
        }
        else
        {
            // Allocate an AVCodecContext (NULL: the codec-specific defaults won't be initialized)
            _avContext = avcodec_alloc_context3(NULL);
        }

        avtranscoder::OptionArray commonOptions;
        avtranscoder::loadOptions(commonOptions, _avContext, flags);
        _avOptions.insert(std::make_pair("", commonOptions));
    }
    else
    {
        if(prefixScope == kPrefixFormat)
        {
            _avOptions = avtranscoder::getAvailableOptionsPerOutputFormat();
        }
        else if(prefixScope == kPrefixVideo)
        {
            _avOptions = avtranscoder::getAvailableOptionsPerVideoCodec();
        }
        else if(prefixScope == kPrefixAudio)
        {
            _avOptions = avtranscoder::getAvailableOptionsPerAudioCodec();
        }
    }
}

LibAVParams::~LibAVParams()
{
    av_free(_avContext);
}

void LibAVParams::fetchLibAVParams(OFX::ImageEffect& plugin, const std::string& prefix)
{
    // iterate on map keys
    BOOST_FOREACH(avtranscoder::OptionArrayMap::value_type& subGroupOption, _avOptions)
    {
        const std::string detailedName = subGroupOption.first;
        avtranscoder::OptionArray& options = subGroupOption.second;

        // iterate on options
        BOOST_FOREACH(avtranscoder::Option& option, options)
        {
            std::string name = prefix;
            if(!detailedName.empty())
            {
                name += detailedName;
                name += "_";
            }
            name += option.getName();

            switch(option.getType())
            {
                case avtranscoder::eOptionBaseTypeBool:
                {
                    _paramOFX.push_back(plugin.fetchBooleanParam(name));
                    break;
                }
                case avtranscoder::eOptionBaseTypeInt:
                {
                    _paramOFX.push_back(plugin.fetchIntParam(name));
                    break;
                }
                case avtranscoder::eOptionBaseTypeDouble:
                {
                    _paramOFX.push_back(plugin.fetchDoubleParam(name));
                    break;
                }
                case avtranscoder::eOptionBaseTypeString:
                {
                    _paramOFX.push_back(plugin.fetchStringParam(name));
                    break;
                }
                case avtranscoder::eOptionBaseTypeRatio:
                {
                    _paramOFX.push_back(plugin.fetchInt2DParam(name));
                    break;
                }
                case avtranscoder::eOptionBaseTypeChoice:
                {
                    // manage exception of video/audio threads parameter: we want to manipulate an OFX Int parameter
                    if(name == kPrefixVideo + kOptionThreads || name == kPrefixAudio + kOptionThreads)
                    {
                        _paramOFX.push_back(plugin.fetchIntParam(name));
                        break;
                    }

                    OFX::ChoiceParam* choice = plugin.fetchChoiceParam(name);
                    _paramOFX.push_back(choice);
                    _childsPerChoice.insert(std::make_pair(choice, std::vector<std::string>()));
                    BOOST_FOREACH(const avtranscoder::Option& child, option.getChilds())
                    {
                        _childsPerChoice.at(choice).push_back(child.getName());
                    }
                    break;
                }
                case avtranscoder::eOptionBaseTypeGroup:
                {
                    _paramFlagOFXPerOption.insert(std::make_pair(name, std::vector<OFX::BooleanParam*>()));
                    BOOST_FOREACH(const avtranscoder::Option& child, option.getChilds())
                    {
                        std::string childName = prefix;
                        if(!detailedName.empty())
                        {
                            childName += detailedName;
                            childName += "_";
                        }
                        childName += child.getUnit();
                        childName += common::kPrefixFlag;
                        childName += child.getName();

                        _paramFlagOFXPerOption.at(name).push_back(plugin.fetchBooleanParam(childName));
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
}

avtranscoder::ProfileLoader::Profile LibAVParams::getCorrespondingProfile(const std::string& detailedName)
{
    avtranscoder::ProfileLoader::Profile profileToReturn;

    LibAVOptions optionsNameAndValue;
    // Get all libav options and values corresponding to the OFX parameters
    BOOST_FOREACH(OFX::ValueParam* param, _paramOFX)
    {
        // skip detailed params which does not concern our current format/codec
        if(!detailedName.empty() && param->getName().find("_" + detailedName + "_") == std::string::npos)
            continue;

        const std::string libavOptionName(getOptionNameWithoutPrefix(param->getName(), detailedName));
        try
        {
            const avtranscoder::Option& libavOption = getLibAVOption(libavOptionName, detailedName);
            std::string libavOptionValue("");

            // Manage OFX Boolean
            OFX::BooleanParam* paramBoolean = dynamic_cast<OFX::BooleanParam*>(param);
            if(paramBoolean)
            {
                if(libavOption.getDefaultBool() == paramBoolean->getValue())
                    continue;

                libavOptionValue = boost::to_string(paramBoolean->getValue());
                optionsNameAndValue.insert(std::make_pair(libavOptionName, libavOptionValue));
                continue;
            }

            // Manage OFX Int
            OFX::IntParam* paramInt = dynamic_cast<OFX::IntParam*>(param);
            if(paramInt)
            {
                // exception for threads
                if(libavOption.getDefaultInt() == paramInt->getValue() && libavOption.getName() != kOptionThreads)
                    continue;

                // FFmpeg threads option has a default value of 1.
                // Our threads parameter has a default value of 0.
                // If 0, do not use it to set the decoding/encoding profile:
                // avTranscoder will automatically set the number of threads according to the codec.
                if(libavOption.getName() == kOptionThreads && paramInt->getValue() == kOptionThreadsValue)
                    continue;

                libavOptionValue = boost::to_string(paramInt->getValue());
                optionsNameAndValue.insert(std::make_pair(libavOptionName, libavOptionValue));
                continue;
            }

            // Manage OFX Double
            OFX::DoubleParam* paramDouble = dynamic_cast<OFX::DoubleParam*>(param);
            if(paramDouble)
            {
                if(boost::test_tools::check_is_close(libavOption.getDefaultDouble(), paramDouble->getValue(),
                                                     boost::test_tools::percent_tolerance(0.5)))
                    continue;

                libavOptionValue = boost::to_string(paramDouble->getValue());
                optionsNameAndValue.insert(std::make_pair(libavOptionName, libavOptionValue));
                continue;
            }

            // Manage OFX String
            OFX::StringParam* paramString = dynamic_cast<OFX::StringParam*>(param);
            if(paramString)
            {
                // skip empty string
                libavOptionValue = paramString->getValue();
                if(libavOptionValue.empty())
                    continue;

                if(libavOption.getDefaultString() == libavOptionValue)
                    continue;

                optionsNameAndValue.insert(std::make_pair(libavOptionName, libavOptionValue));
                continue;
            }

            // Manage OFX Int2D
            OFX::Int2DParam* paramRatio = dynamic_cast<OFX::Int2DParam*>(param);
            if(paramRatio)
            {
                std::pair<int, int> ofxParamValue = std::make_pair(paramRatio->getValue().x, paramRatio->getValue().y);
                if(libavOption.getDefaultRatio() == ofxParamValue)
                    continue;

                libavOptionValue =
                    boost::to_string(paramRatio->getValue().x) + ":" + boost::to_string(paramRatio->getValue().y);
                optionsNameAndValue.insert(std::make_pair(libavOptionName, libavOptionValue));
                continue;
            }

            // Manage OFX Choice
            OFX::ChoiceParam* paramChoice = dynamic_cast<OFX::ChoiceParam*>(param);
            if(paramChoice)
            {
                if(libavOption.getDefaultInt() == paramChoice->getValue())
                    continue;

                const size_t optionIndex = paramChoice->getValue();
                const std::vector<std::string> childs(_childsPerChoice.at(paramChoice));
                if(childs.size() > optionIndex)
                {
                    libavOptionValue = childs.at(optionIndex);
                    optionsNameAndValue.insert(std::make_pair(libavOptionName, libavOptionValue));
                }
                continue;
            }
        }
        catch(std::exception& e)
        {
            TUTTLE_LOG_INFO("Can't get option " << libavOptionName << " (" << detailedName << "): " << e.what());
            continue;
        }
    }

    // Get all libav flags and values corresponding to the OFX parameters
    BOOST_FOREACH(FlagOFXPerOption::value_type& flagsPerOption, _paramFlagOFXPerOption)
    {
        try
        {
            // skip flags which does not concern our current format/codec
            if(!detailedName.empty() && flagsPerOption.first.find("_" + detailedName + "_") == std::string::npos)
                continue;

            const std::string flagName(getOptionNameWithoutPrefix(flagsPerOption.first, detailedName));

            // iterate on option's flags
            BOOST_FOREACH(OFX::BooleanParam* param, flagsPerOption.second)
            {
                // skip flags not set
                if(param->getValue() == param->getDefault())
                    continue;

                // get flag value
                std::string libavOptionValue("");
                if(param->getValue())
                    libavOptionValue.append("+");
                else
                    libavOptionValue.append("-");
                libavOptionValue.append(getOptionNameWithoutPrefix(param->getName(), detailedName));

                // if first flag with this flagName
                if(optionsNameAndValue.find(flagName) == optionsNameAndValue.end())
                {
                    // create new option in profile
                    optionsNameAndValue.insert(std::make_pair(flagName, libavOptionValue));
                }
                else
                {
                    // append flag to the existing option
                    optionsNameAndValue.at(flagName) += libavOptionValue;
                }
            }
        }
        catch(std::exception& e)
        {
            TUTTLE_LOG_WARNING("Can't create flag option from name " << flagsPerOption.first << " (" << detailedName << ")");
        }
    }

    // Fill and return the profile
    BOOST_FOREACH(LibAVOptions::value_type& nameAndValue, optionsNameAndValue)
    {
        profileToReturn[nameAndValue.first] = nameAndValue.second;
    }
    return profileToReturn;
}

avtranscoder::Option& LibAVParams::getLibAVOption(const std::string& libAVOptionName, const std::string& detailedName)
{
    avtranscoder::OptionArray& optionsArray = _avOptions.at(detailedName);

    avtranscoder::OptionArray::iterator iterOption = std::find_if(
        optionsArray.begin(), optionsArray.end(), boost::bind(&avtranscoder::Option::getName, _1) == libAVOptionName);
    const size_t optionIndex = std::distance(optionsArray.begin(), iterOption);

    if(optionIndex >= optionsArray.size())
        BOOST_THROW_EXCEPTION(exception::Failed() << exception::user() + "Can't get libav option '" + libAVOptionName +
                                                         "' (with detailed  '" + detailedName + "')");

    return optionsArray.at(optionIndex);
}

bool LibAVParams::setOption(const std::string& libAVOptionName, const std::string& value, const std::string& detailedName)
{
    try
    {
        // Get libav option
        avtranscoder::Option& option = getLibAVOption(libAVOptionName, detailedName);

        // Set libav option's value
        option.setString(value);

        // Get corresponding OFX parameter
        OFX::ValueParam* param = getOFXParameter(libAVOptionName, detailedName);
        if(!param)
        {
            TUTTLE_LOG_WARNING("Can't get OFX parameter corresponding to option " << libAVOptionName << " of subgroup "
                                                                                  << detailedName);
            return false;
        }

        // Set OFX parameter's value
        OFX::BooleanParam* paramBoolean = dynamic_cast<OFX::BooleanParam*>(param);
        if(paramBoolean)
        {
            paramBoolean->setValue(option.getBool());
            return true;
        }
        OFX::IntParam* paramInt = dynamic_cast<OFX::IntParam*>(param);
        if(paramInt)
        {
            paramInt->setValue(option.getInt());
            return true;
        }
        OFX::DoubleParam* paramDouble = dynamic_cast<OFX::DoubleParam*>(param);
        if(paramDouble)
        {
            paramDouble->setValue(option.getDouble());
            return true;
        }
        OFX::StringParam* paramString = dynamic_cast<OFX::StringParam*>(param);
        if(paramString)
        {
            paramString->setValue(option.getString());
            return true;
        }
        OFX::Int2DParam* paramRatio = dynamic_cast<OFX::Int2DParam*>(param);
        if(paramRatio)
        {
            paramRatio->setValue(option.getRatio().first, option.getRatio().second);
            return true;
        }
        OFX::ChoiceParam* paramChoice = dynamic_cast<OFX::ChoiceParam*>(param);
        if(paramChoice)
        {
            paramChoice->setValue(option.getInt());
            return true;
        }
    }
    catch(std::exception& e)
    {
        TUTTLE_LOG_WARNING("Can't set option " << libAVOptionName << " to " << value << ": " << e.what());
        return false;
    }
}

OFX::ValueParam* LibAVParams::getOFXParameter(const std::string& libAVOptionName, const std::string& detailedName)
{
    BOOST_FOREACH(OFX::ValueParam* param, _paramOFX)
    {
        if(getOptionNameWithoutPrefix(param->getName(), detailedName) == libAVOptionName)
            return param;
    }
    return NULL;
}

void addOptionsToGroup(OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group,
                       const avtranscoder::OptionArray& optionsArray, const std::string& prefix,
                       const std::string& detailedName)
{
    OFX::ParamDescriptor* param = NULL;
    BOOST_FOREACH(const avtranscoder::Option& option, optionsArray)
    {
        std::string name = prefix;
        if(!detailedName.empty())
        {
            name += detailedName;
            name += "_";
        }
        name += option.getName();

        switch(option.getType())
        {
            case avtranscoder::eOptionBaseTypeBool:
            {
                OFX::BooleanParamDescriptor* boolParam = desc.defineBooleanParam(name);
                boolParam->setDefault(option.getDefaultBool());
                param = boolParam;
                break;
            }
            case avtranscoder::eOptionBaseTypeInt:
            {
                OFX::IntParamDescriptor* intParam = desc.defineIntParam(name);
                intParam->setDefault(option.getDefaultInt());
                const int min =
                    option.getMin() > std::numeric_limits<int>::min() ? option.getMin() : std::numeric_limits<int>::min();
                const int max =
                    option.getMax() < std::numeric_limits<int>::max() ? option.getMax() : std::numeric_limits<int>::max();
                intParam->setRange(min, max);
                intParam->setDisplayRange(min, max);
                param = intParam;
                break;
            }
            case avtranscoder::eOptionBaseTypeDouble:
            {
                OFX::DoubleParamDescriptor* doubleParam = desc.defineDoubleParam(name);
                doubleParam->setDefault(option.getDefaultDouble());
                doubleParam->setRange(option.getMin(), option.getMax());
                doubleParam->setDisplayRange(option.getMin(), option.getMax());
                param = doubleParam;
                break;
            }
            case avtranscoder::eOptionBaseTypeString:
            {
                OFX::StringParamDescriptor* strParam = desc.defineStringParam(name);
                strParam->setDefault(option.getDefaultString());
                param = strParam;
                break;
            }
            case avtranscoder::eOptionBaseTypeRatio:
            {
                OFX::Int2DParamDescriptor* ratioParam = desc.defineInt2DParam(name);
                // @todo: minX, minY, maxX, maxY could be different
                ratioParam->setDefault(option.getDefaultRatio().first, option.getDefaultRatio().second);
                const int min =
                    option.getMin() > std::numeric_limits<int>::min() ? option.getMin() : std::numeric_limits<int>::min();
                const int max =
                    option.getMax() < std::numeric_limits<int>::max() ? option.getMax() : std::numeric_limits<int>::max();
                ratioParam->setRange(min, min, max, max);
                ratioParam->setDisplayRange(min, min, max, max);
                param = ratioParam;
                break;
            }
            case avtranscoder::eOptionBaseTypeChoice:
            {
                // manage exception of threads parameters: we want to manipulate an OFX Int parameter
                if(name == kPrefixVideo + kOptionThreads || name == kPrefixAudio + kOptionThreads)
                {
                    OFX::IntParamDescriptor* intParam = desc.defineIntParam(name);
                    intParam->setDefault(kOptionThreadsValue);
                    intParam->setRange(0, std::numeric_limits<int>::max());
                    intParam->setDisplayRange(0, 64);
                    param = intParam;
                    break;
                }

                OFX::ChoiceParamDescriptor* choiceParam = desc.defineChoiceParam(name);

                // Translate the libav option default value to a value compliant with OpenFX
                int ofxDefaultValue = 0;
                const int libavDefaultValue = option.getDefaultInt();
                if(libavDefaultValue >= 0 && libavDefaultValue < option.getChilds().size())
                    ofxDefaultValue = libavDefaultValue;
                else if(option.getMin() > std::numeric_limits<int>::min())
                {
                    const int computedDefaultValue = libavDefaultValue - option.getMin();
                    if(computedDefaultValue < option.getChilds().size())
                        ofxDefaultValue = computedDefaultValue;
                }
                choiceParam->setDefault(ofxDefaultValue);

                // Added child options
                BOOST_FOREACH(const avtranscoder::Option& child, option.getChilds())
                {
                    choiceParam->appendOption(child.getName() + " " + child.getHelp());
                }

                // Fixed warning message in some host (ie. Nuke), about OFX Choice parameters without child options
                if(option.getChilds().empty())
                {
                    choiceParam->appendOption("unknown");
                }

                // Hide the OFX Choice parameter if it has only one option (not useful in the interface)
                if(choiceParam->getNOptions() < 2)
                    choiceParam->setIsSecret(true);

                param = choiceParam;
                break;
            }
            case avtranscoder::eOptionBaseTypeGroup:
            {
                std::string groupName = prefix;
                groupName += kPrefixGroup;
                if(!detailedName.empty())
                {
                    groupName += detailedName;
                    groupName += "_";
                }
                groupName += option.getName();

                OFX::GroupParamDescriptor* groupParam = desc.defineGroupParam(groupName);
                groupParam->setOpen(false);
                BOOST_FOREACH(const avtranscoder::Option& child, option.getChilds())
                {
                    std::string childName = prefix;
                    if(!detailedName.empty())
                    {
                        childName += detailedName;
                        childName += "_";
                    }
                    childName += child.getUnit();
                    childName += kPrefixFlag;
                    childName += child.getName();

                    OFX::BooleanParamDescriptor* param = desc.defineBooleanParam(childName);
                    param->setLabel(child.getName());
                    param->setDefault(child.getOffset());
                    param->setHint(child.getHelp());
                    param->setParent(groupParam);
                }
                param = groupParam;
                break;
            }
            default:
                break;
        }
        if(param)
        {
            param->setLabel(option.getName());
            param->setHint(option.getHelp());
            param->setParent(group);

            // add help for our custom threads parameters
            if(name == kPrefixVideo + kOptionThreads || name == kPrefixAudio + kOptionThreads)
            {
                param->setHint("set a number of threads for encoding (0 autodetect a suitable number of threads to use)");
            }
        }
    }
}

void addOptionsToGroup(OFX::ImageEffectDescriptor& desc, OFX::GroupParamDescriptor* group,
                       const avtranscoder::OptionArrayMap& optionArrayMap, const std::string& prefix)
{
    // iterate on map keys
    BOOST_FOREACH(const avtranscoder::OptionArrayMap::value_type& subGroupOption, optionArrayMap)
    {
        const std::string detailedName = subGroupOption.first;
        const avtranscoder::OptionArray& options = subGroupOption.second;

        addOptionsToGroup(desc, group, options, prefix, detailedName);
    }
}

std::string getOptionNameWithoutPrefix(const std::string& optionName, const std::string& detailedName)
{
    std::string nameWithoutPrefix(optionName);

    // prefix
    if(nameWithoutPrefix.find(kPrefixFormat) != std::string::npos)
        nameWithoutPrefix.erase(0, kPrefixFormat.size());
    else if(nameWithoutPrefix.find(kPrefixVideo) != std::string::npos)
        nameWithoutPrefix.erase(0, kPrefixVideo.size());
    else if(nameWithoutPrefix.find(kPrefixAudio) != std::string::npos)
        nameWithoutPrefix.erase(0, kPrefixAudio.size());

    // detailed group name
    if(!detailedName.empty() && nameWithoutPrefix.find(detailedName) != std::string::npos)
    {
        // detailedName.size() + 1: also remove the "_"
        nameWithoutPrefix.erase(0, detailedName.size() + 1);
    }

    // childs of groups (flag)
    size_t endedPosition;
    if((endedPosition = nameWithoutPrefix.find(kPrefixFlag)) != std::string::npos)
    {
        nameWithoutPrefix.erase(0, endedPosition + kPrefixFlag.size());
    }

    return nameWithoutPrefix;
}

std::string getOptionFlagName(const std::string& optionName, const std::string& detailedName)
{
    std::string flagName;

    if(optionName.find(kPrefixFlag) != std::string::npos)
    {
        size_t startedPosition;
        if(detailedName.empty())
            startedPosition = optionName.find("_");
        else
        {
            startedPosition = optionName.find("_", prefixSize);
        }
        ++startedPosition; // started after the "_"
        size_t endedPosition = optionName.find(kPrefixFlag, startedPosition);

        flagName = optionName.substr(startedPosition, endedPosition - startedPosition);
    }

    return flagName;
}

void disableOFXParamsForFormatOrCodec(OFX::ImageEffect& plugin, const avtranscoder::OptionArrayMap& optionArrayMap,
                                      const std::string& filter, const std::string& prefix)
{
    // iterate on map keys
    BOOST_FOREACH(const avtranscoder::OptionArrayMap::value_type& subGroupOption, optionArrayMap)
    {
        const std::string detailedName = subGroupOption.first;
        const avtranscoder::OptionArray& options = subGroupOption.second;

        // iterate on options
        BOOST_FOREACH(const avtranscoder::Option& option, options)
        {
            std::string name = prefix;
            name += detailedName;
            name += "_";
            name += option.getName();

            switch(option.getType())
            {
                case avtranscoder::eOptionBaseTypeBool:
                {
                    OFX::BooleanParam* curOpt = plugin.fetchBooleanParam(name);
                    curOpt->setIsSecretAndDisabled(!(detailedName == filter));
                    break;
                }
                case avtranscoder::eOptionBaseTypeInt:
                {
                    OFX::IntParam* curOpt = plugin.fetchIntParam(name);
                    curOpt->setIsSecretAndDisabled(!(detailedName == filter));
                    break;
                }
                case avtranscoder::eOptionBaseTypeDouble:
                {
                    OFX::DoubleParam* curOpt = plugin.fetchDoubleParam(name);
                    curOpt->setIsSecretAndDisabled(!(detailedName == filter));
                    break;
                }
                case avtranscoder::eOptionBaseTypeString:
                {
                    OFX::StringParam* curOpt = plugin.fetchStringParam(name);
                    curOpt->setIsSecretAndDisabled(!(detailedName == filter));
                    break;
                }
                case avtranscoder::eOptionBaseTypeRatio:
                {
                    OFX::Int2DParam* curOpt = plugin.fetchInt2DParam(name);
                    curOpt->setIsSecretAndDisabled(!(detailedName == filter));
                    break;
                }
                case avtranscoder::eOptionBaseTypeChoice:
                {
                    OFX::ChoiceParam* curOpt = plugin.fetchChoiceParam(name);
                    curOpt->setIsSecretAndDisabled(!(detailedName == filter));
                    break;
                }
                case avtranscoder::eOptionBaseTypeGroup:
                {
                    std::string groupName = prefix;
                    groupName += common::kPrefixGroup;
                    groupName += detailedName;
                    groupName += "_";
                    groupName += option.getName();

                    OFX::GroupParam* curOpt = plugin.fetchGroupParam(groupName);
                    curOpt->setIsSecretAndDisabled(!(detailedName == filter));

                    BOOST_FOREACH(const avtranscoder::Option& child, option.getChilds())
                    {
                        std::string childName = prefix;
                        if(!detailedName.empty())
                        {
                            childName += detailedName;
                            childName += "_";
                        }
                        childName += child.getUnit();
                        childName += common::kPrefixFlag;
                        childName += child.getName();

                        OFX::BooleanParam* curOpt = plugin.fetchBooleanParam(childName);
                        curOpt->setIsSecretAndDisabled(!(detailedName == filter));
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
}
}
}
}
}
