#ifndef _TUTTLE_HOST_VERSION_HPP_
#define _TUTTLE_HOST_VERSION_HPP_

// This is more than the version of the host: this is the version of the 'tuttle bundle' (host + plugins + sam).
#define TUTTLE_HOST_VERSION_MAJOR 0
#define TUTTLE_HOST_VERSION_MINOR 13
#define TUTTLE_HOST_VERSION_MICRO 6

#define TUTTLE_HOST_VERSION_STR                                                                                             \
    BOOST_PP_STRINGIZE(TUTTLE_HOST_VERSION_MAJOR) "." BOOST_PP_STRINGIZE(TUTTLE_HOST_VERSION_MINOR) "." BOOST_PP_STRINGIZE(TUTTLE_HOST_VERSION_MICRO)

#endif
