#ifndef BOOST_NETWORK_PROTOCOL_HTTP_CLIENT_ASYNC_IMPL_HPP_20100623
#define BOOST_NETWORK_PROTOCOL_HTTP_CLIENT_ASYNC_IMPL_HPP_20100623

// Copyright Dean Michael Berris 2010.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

namespace boost { namespace network { namespace http {

    template <class Tag, unsigned version_major, unsigned version_minor>
    struct basic_client_impl;

    namespace impl {
        template <class Tag, unsigned version_major, unsigned version_minor>
        struct async_client : 
            connection_policy<Tag,version_major,version_minor>::type
        {
            typedef
                typename connection_policy<Tag,version_major,version_minor>::type
                connection_base;
            typedef
                typename resolver<Tag>::type
                resolver_type;
            typedef
                typename string<Tag>::type
                string_type;

            async_client(bool cache_resolved, bool follow_redirect)
                : connection_base(cache_resolved, follow_redirect),
                service_ptr(new boost::asio::io_service),
                service_(*service_ptr),
                resolver_(service_),
                sentinel_(new boost::asio::io_service::work(service_))
            {
                connection_base::resolver_strand_.reset(new
                    boost::asio::io_service::strand(service_));
                lifetime_thread_.reset(new boost::thread(
                    boost::bind(
                        &boost::asio::io_service::run,
                        &service_
                        )));
            }

            async_client(bool cache_resolved, bool follow_redirect, boost::asio::io_service & service)
                : connection_base(cache_resolved, follow_redirect),
                service_ptr(),
                service_(service),
                resolver_(service_),
                sentinel_(new boost::asio::io_service::work(service_))
            {
            }

            ~async_client() throw ()
            {
                sentinel_.reset();
                if (lifetime_thread_.get()) {
                    lifetime_thread_->join();
                    lifetime_thread_.reset();
                }
            }

            basic_response<Tag> const request_skeleton(
                basic_request<Tag> const & request_, 
                string_type const & method, 
                bool get_body
                ) 
            {
                typename connection_base::connection_ptr connection_;
                connection_ = connection_base::get_connection(resolver_, request_);
                return connection_->send_request(method, request_, get_body);
            }

            std::auto_ptr<boost::asio::io_service> service_ptr;
            boost::asio::io_service & service_;
            resolver_type resolver_;
            boost::shared_ptr<boost::asio::io_service::work> sentinel_;
            boost::shared_ptr<boost::thread> lifetime_thread_;
        };
    } // namespace impl

} // namespace http

} // namespace network

} // namespace boost

#endif // BOOST_NETWORK_PROTOCOL_HTTP_CLIENT_ASYNC_IMPL_HPP_20100623
