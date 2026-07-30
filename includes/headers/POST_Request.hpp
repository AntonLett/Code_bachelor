#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <iostream>

namespace beast = boost::beast;
namespace http  = beast::http;
namespace net   = boost::asio;
using     tcp   = boost::asio::ip::tcp;

class POST_Request{
private:
    std::string port_;
    std::string domain_;
    std::string path_;
    tcp::resolver resolver_;
    tcp::socket socket_;
    net::io_context& io_;

public:
    POST_Request(const std::string domain, const std::string port, const std::string path, net::io_context& io): 
        domain_{domain}, 
        port_{port}, 
        path_{path},
        io_(io),
        resolver_{tcp::resolver(io)},
        socket_{tcp::socket(io)}
        {}
    ~POST_Request(){
        beast::error_code ec;
        socket_.shutdown(tcp::socket::shutdown_both, ec);
    }

    void send_post(const std::string& body){
        const auto results = resolver_.resolve(domain_, port_);
        net::connect(socket_, results.begin(), results.end());
        http::request<http::string_body> req{http::verb::post, path_, 11};

        req.set(http::field::host, domain_);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::content_type, "application/json");
        req.content_length(body.size());
        req.body() = body;

        http::write(socket_,req);
    }
    std::string receive_answer(){
        beast::flat_buffer buffer;
        http::response<http::string_body> res;
        http::read(socket_, buffer, res);

        return res.body().data();
    }
};