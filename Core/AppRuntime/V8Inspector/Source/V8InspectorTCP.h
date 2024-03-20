// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// This code is based on the old node inspector implementation. See NOTICE.md for Node.js' project license details
#pragma once

// 'allocator<void>' is deprecated 
// Android with NDK25
// 23/10/04 : allocator still present in latest asio main branch
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#ifdef WIN32
#include <Winsock2.h>
#endif
#include <asio.hpp>

namespace Babylon
{
    class tcp_connection : public std::enable_shared_from_this<tcp_connection>
    {
    public:
        asio::ip::tcp::socket& socket();

        typedef void (*ReadCallback)(std::vector<char>&, bool iseof, void* data);
        inline void registerReadCallback(ReadCallback callback, void* data)
        {
            readcallback_ = callback;
            callbackData_ = data;
        }

        void read_loop_async();
        void write_async(std::vector<char>);
        void close();

        inline tcp_connection(asio::ip::tcp::socket socket)
            : socket_(std::move(socket))
        {
        }

    private:
        asio::ip::tcp::socket socket_;
        std::string message_;

        /// Buffer for incoming data.
        std::array<char, 8192> buffer_;

        void* callbackData_;
        ReadCallback readcallback_;
    };

    class tcp_server : public std::enable_shared_from_this<tcp_server>
    {
    public:
        typedef void (*ConnectionCallback)(std::shared_ptr<tcp_connection> connection, void* callbackData_);

        void run();
        void stop();

        tcp_server(unsigned short port, ConnectionCallback callback, void* data);
        void do_accept();

    private:
        asio::io_context io_context_;
        asio::ip::tcp::acceptor acceptor_;
        asio::ip::tcp::socket socket_;

        void* callbackData_;
        ConnectionCallback connectionCallback_;
    };
}