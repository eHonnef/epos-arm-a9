// EPOS AES Mediator Common Package

#ifndef __aes_h
#define __aes_h

#include <system/config.h>

__BEGIN_SYS

class AES_Common
{
protected:
    AES_Common() {}
};

__END_SYS

#ifdef __AES_H
#include __AES_H
#else

#include <utility/aes.h>

__BEGIN_SYS

template<unsigned int KEY_SIZE>
class AES: private AES_Common, public _UTIL::AES<KEY_SIZE> {};

__END_SYS

#endif

#endif
