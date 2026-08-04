#pragma once
#include <array>
#include <boost/asio.hpp>
#include <iostream>
using boost::asio::ip::tcp;

// https://anubhav-gupta62.medium.com/registry-design-pattern-ad4b4c3350e6 05.Mai.2026
// https://refactoring.guru/design-patterns/singleton/cpp/example 05.Mai.2026
class Client
{
private:
    boost::asio::io_context io_context;
    tcp::resolver resolver;
    tcp::socket socket;
    static Client *instance_;
    Client(const std::string &host, const std::string &port)
        : io_context(), resolver(io_context), socket(io_context)
    {
        try
        {
            tcp::resolver::results_type endpoints = resolver.resolve(host, port);
            boost::asio::connect(socket, endpoints);
        }
        catch (const std::exception &e)
        {
            std::cerr << "ERROR in Client::Client: " << e.what() << std::endl;
            throw;
        }
    }

public:
    static Client *getClientInstance(const std::string &host, const std::string &port)
    {
        if (instance_ == nullptr)
            instance_ = new Client(host, port);
        return instance_;
    }
    std::string sendMessage(const std::string &message)
    {
        try
        {
            boost::system::error_code ec;
            size_t status = boost::asio::write(socket, boost::asio::buffer(message), ec);
            if (ec || status == 0)
            {
                // std::cerr << "Sending failed: " << ec.message() << std::endl;
                return "ERROR sending failed";
            }
            return readResponse();
        }
        catch (const boost::system::system_error &e)
        {
            std::cerr << "Error while sending message: " << e.what() << std::endl;
            return "ERROR while sending";
        }
    }

    std::string readResponse()
    {
        try
        {
            boost::asio::streambuf buffer;
            boost::asio::read_until(socket, buffer, "\n");
            std::istream response_stream(&buffer);
            std::string response;
            std::getline(response_stream, response);
            return response;
        }
        catch (...)
        {
            return "ERROR in readResponse";
        }
    }

    std::string receiveMessage()
    {
        try
        {
            boost::asio::streambuf buf;
            boost::system::error_code error;
            boost::asio::read_until(socket, buf, "\n");

            if (error == boost::asio::error::eof)
            {
                return std::string(boost::asio::buffer_cast<const char *>(buf.data()), buf.size());
            }
            else if (error)
            {
                throw boost::system::system_error(error);
            }

            return std::string(boost::asio::buffer_cast<const char *>(buf.data()), buf.size());
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error while receiving message: " << e.what() << std::endl;
            throw;
        }
    }

    void communicate(std::string message)
    {
        try
        {

            for (;;)
            {
                sendMessage(message);
                std::string response = receiveMessage();
                std::cout << response << std::endl;
                if (response == "OK")
                {
                    break;
                }
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "Error while communicating: " << e.what() << std::endl;
        }
    }
};
Client *Client::instance_ = nullptr;