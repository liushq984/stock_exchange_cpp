#pragma once

#include <cstring>
#include <iostream>

template< typename T >
size_t pack( void* buffer, size_t offset, const T* source, size_t length=1 )
{
    memcpy( (char*)buffer+offset, source, sizeof(T)*length );
    return offset+sizeof(T)*length;
}

template< typename T >
size_t unpack( const void* buffer, size_t offset, T* target, size_t length=1 )
{
    memcpy( target, (char*)buffer+offset, sizeof(T)*length );
    return offset+sizeof(T)*length;
}

template< typename T >
constexpr size_t buffer_size()
{
    // compile time error - T is an unknown type
}

template< typename T >
constexpr size_t payload_type()
{
    // compile time error - T is an unknown type
}

#include "types.hpp"
#include "structs.inl"
