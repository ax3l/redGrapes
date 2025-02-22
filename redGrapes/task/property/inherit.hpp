/* Copyright 2019-2020 Michael Sippel
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/**
 * @file redGrapes/property/inherit.hpp
 */

#pragma once

#include <type_traits>
#include <fmt/format.h>
#include <redGrapes/task/property/trait.hpp>

namespace redGrapes
{

struct Task;

template <
    typename T_Head,
    typename... T_Tail
>
struct TaskPropertiesInherit
    : T_Head
    , TaskPropertiesInherit< T_Tail ... >
{
    template < typename PropertiesBuilder >
    struct Builder
        : T_Head::template Builder< PropertiesBuilder >
        , TaskPropertiesInherit< T_Tail ... >::template Builder< PropertiesBuilder >
    {
        Builder( PropertiesBuilder & p )
            : T_Head::template Builder< PropertiesBuilder >{ p }
            , TaskPropertiesInherit< T_Tail ... >::template Builder< PropertiesBuilder >( p )
        {}
    };

    struct Patch
        : T_Head::Patch
        , TaskPropertiesInherit< T_Tail ... >::Patch
    {
        template < typename PatchBuilder >
        struct Builder
            : T_Head::Patch::template Builder< PatchBuilder >
            , TaskPropertiesInherit< T_Tail ... >::Patch::template Builder< PatchBuilder >
        {
            Builder( PatchBuilder & p )
                : T_Head::Patch::template Builder< PatchBuilder >{ p }
                , TaskPropertiesInherit< T_Tail ... >::Patch::template Builder< PatchBuilder >( p )
            {}
        };
    };
    
    void apply_patch( Patch const & patch )
    {
        T_Head::apply_patch( patch );
        TaskPropertiesInherit< T_Tail ... >::apply_patch( patch );
    }
};

struct PropEnd_t {};

template<>
struct TaskPropertiesInherit< PropEnd_t >
{
    template < typename PropertiesBuilder >
    struct Builder
    {
        Builder( PropertiesBuilder & ) {}
    };

    struct Patch
    {
        template < typename PatchBuilder >
        struct Builder
        {
            Builder( PatchBuilder & ) {}
        };        
    };

    void apply_patch( Patch const & ) {}
};

template < typename... Policies >
struct TaskProperties1
    : public TaskPropertiesInherit< Policies..., PropEnd_t >
{
    struct Builder
        : TaskPropertiesInherit< Policies..., PropEnd_t >::template Builder< Builder >
    {
        TaskProperties1 prop;

        Builder()
            : TaskPropertiesInherit< Policies..., PropEnd_t >::template Builder< Builder >( *this )
        {}

        Builder( TaskProperties1 const & prop )
            : TaskPropertiesInherit< Policies..., PropEnd_t >::template Builder< Builder >( *this )
            , prop(prop)
        {}

        Builder( Builder const & b )
            : prop( b.prop )
            , TaskPropertiesInherit< Policies..., PropEnd_t >::template Builder< Builder >( *this )
        {}

        template < typename T >
        void add( T const & obj )
        {
            trait::BuildProperties<T>::build( *this, obj );
        }

        operator TaskProperties1 () const
        {
            return prop;
        }
    };

    struct Patch
        : TaskPropertiesInherit< Policies..., PropEnd_t >::Patch
    {
        struct Builder
            : TaskPropertiesInherit< Policies..., PropEnd_t >::Patch::template Builder< Builder >
        {
            Patch patch;

            Builder()
                : TaskPropertiesInherit< Policies..., PropEnd_t >::Patch::template Builder< Builder >( *this )
            {}

            Builder( Builder const & b )
                : patch( b.patch )
                , TaskPropertiesInherit< Policies..., PropEnd_t >::Patch::template Builder< Builder >( *this )
            {}

            operator Patch () const
            {
                return patch;
            }
        };
    };
    
    void apply_patch( Patch const & patch )
    {
        TaskPropertiesInherit< Policies..., PropEnd_t >::apply_patch( patch );
    }
};

} // namespace redGrapes

template <>
struct fmt::formatter<
    redGrapes::TaskPropertiesInherit< redGrapes::PropEnd_t >
>
{
    constexpr auto parse( format_parse_context& ctx )
    {
        return ctx.begin();
    }
    
    template < typename FormatContext >
    auto format(
        redGrapes::TaskPropertiesInherit< redGrapes::PropEnd_t > const & prop,
        FormatContext & ctx
    )
    {
        return ctx.out();
    }
};

template <
    typename T_Head
>
struct fmt::formatter<
    redGrapes::TaskPropertiesInherit< T_Head, redGrapes::PropEnd_t >
>
{
    constexpr auto parse( format_parse_context& ctx )
    {
        return ctx.begin();
    }

    template < typename FormatContext >
    auto format(
        redGrapes::TaskPropertiesInherit< T_Head, redGrapes::PropEnd_t > const & prop,
        FormatContext & ctx
    )
    {
        return fmt::format_to(
                   ctx.out(),
                   "{}",
                   (T_Head const &) prop
               );
    }
};

template <
    typename T_Head,
    typename... T_Tail
>
struct fmt::formatter<
    redGrapes::TaskPropertiesInherit< T_Head, T_Tail... >
>
{
    constexpr auto parse( format_parse_context& ctx )
    {
        return ctx.begin();
    }

    template < typename FormatContext >
    auto format(
        redGrapes::TaskPropertiesInherit< T_Head, T_Tail... > const & prop,
        FormatContext & ctx
    )
    {
        return fmt::format_to(
                   ctx.out(),
                   "{}, {}",
                   (T_Head const &) prop,
                   (redGrapes::TaskPropertiesInherit< T_Tail... > const &) prop
               );
    }

};

template <
    typename... Policies
>
struct fmt::formatter<
    redGrapes::TaskProperties1< Policies... >
>
{
    constexpr auto parse( format_parse_context& ctx )
    {
        return ctx.begin();
    }

    template < typename FormatContext >
    auto format(
        redGrapes::TaskProperties1< Policies... > const & prop,
        FormatContext & ctx
    )
    {
        return fmt::format_to(
                   ctx.out(),
                   "{{ {} }}",
                   ( typename redGrapes::TaskPropertiesInherit< Policies..., redGrapes::PropEnd_t > const & ) prop
               );
    }
};

