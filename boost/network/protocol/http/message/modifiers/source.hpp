#ifndef BOOST_NETWORK_PROTOCOL_HTTP_MESSAGE_MODIFIER_SOURCE_HPP_20100624
#define BOOST_NETWORK_PROTOCOL_HTTP_MESSAGE_MODIFIER_SOURCE_HPP_20100624

// Copyright 2010 (C) Dean Michael Berris
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/network/support/is_async.hpp>
#include <boost/thread/future.hpp>
#include <boost/concept/requires.hpp>
#include <boost/network/protocol/http/tags.hpp>
#include <boost/network/message/directives.hpp>

namespace boost { namespace network { namespace http {

    template <class Tag>
    struct basic_response;

    namespace impl {

        template <class Tag, class T>
        void source(basic_response<Tag> & response, T const & value, mpl::false_ const &) {
            response << ::boost::network::source(value);
        }

        template <class Tag, class T>
        void source(basic_response<Tag> & response, T const & future, mpl::true_ const &) {
            response.source(future);
        }

    }

    template <class R>
    struct Response;

    template <class Tag, class T>
    inline
    BOOST_CONCEPT_REQUIRES(((Response<basic_response<Tag> >)),
        (void))
    source(basic_response<Tag> & response, T const & value) {
        impl::source(response, value, is_async<Tag>());
    }

} // namespace http

    namespace impl {
        
        template <class Message, class ValueType, class Async>
        inline void source(Message const & message, ValueType const & source_, http::tags::http_server const &, Async const &) {
            message.source = source_;
        }

    } /* impl */

} // namespace network

} // namespace boost

#include <boost/network/message/modifiers/source.hpp>
#endif // BOOST_NETWORK_PROTOCOL_HTTP_MESSAGE_MODIFIER_SOURCE_HPP_20100624
