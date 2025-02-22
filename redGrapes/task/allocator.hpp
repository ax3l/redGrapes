/* Copyright 2022 Michael Sippel
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

namespace redGrapes
{
namespace memory
{

struct Chunk
{
    Chunk( size_t capacity );
    Chunk( Chunk const & ) = delete;
    Chunk( Chunk & ) = delete;
    ~Chunk();
    
    bool empty() const;
    void reset();

    void * m_alloc( size_t n_bytes );
    unsigned m_free( void * );

    bool contains( void * );

private:
    size_t capacity;

    uintptr_t base;
    std::atomic_ptrdiff_t offset;

    std::atomic<unsigned> count;
};

struct Allocator
{
    size_t chunk_size;

    std::mutex m;
    std::vector< std::unique_ptr<Chunk> > blocked_chunks;
    std::unique_ptr<Chunk> active_chunk;

    Allocator()
        : chunk_size(0x1000)
        , active_chunk( std::make_unique<Chunk>(chunk_size) )
    {}

    template <typename T>
    T * m_alloc()
    {
        T * item = (T*) active_chunk->m_alloc( sizeof(T) );

        if( !item )
        {
            // create new chunk & try again
            {
                std::lock_guard<std::mutex> lock(m);               
                blocked_chunks.emplace_back(std::make_unique<Chunk>( chunk_size ));
                std::swap(active_chunk, blocked_chunks[blocked_chunks.size()-1]);
            }
            item = (T*) active_chunk->m_alloc( sizeof(T) );
        }

        return item;
    }

    template < typename T >
    void m_free( T * ptr )
    {
        if( active_chunk->contains((void*)ptr) )
            active_chunk->m_free((void*)ptr);

        else
        {
            std::lock_guard<std::mutex> lock(m);

            // find chunk containing ptr
            for( unsigned i = 0; i < blocked_chunks.size(); ++i )
            {
                Chunk & c = *blocked_chunks[i];

                if( c.contains((void*)ptr) )
                {
                    if( c.m_free( (void*) ptr ) == 0 )
                        blocked_chunks.erase(std::begin(blocked_chunks)+i);

                    break;
                }
            }
        }
    }
};

} // namespace memory
} // namespace redGrapes

