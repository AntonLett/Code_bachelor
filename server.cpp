#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <filesystem>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <unordered_map>
#include <vector>
// #include "includes/exiftool/inc/ExifTool.h"
#include "includes/headers/POST_Request.hpp"
#include "includes/headers/check_if_quotes_needed.hpp"
#include "includes/headers/settings_reader.hpp"
#include "includes/headers/color_print.hpp"
#include "includes/headers/parse_json.hpp"
#include "includes/headers/protocol.h"
#include "includes/headers/BS_thread_pool.hpp"
#include "includes/headers/Extractor.hpp"
#include "includes/headers/toml.hpp"

using boost::asio::ip::tcp;
BS::thread_pool<> pool_handle;
BS::thread_pool<> *extraction_pool;

std::mutex map_mutex;

// OPENSEARCH settings
std::string OS_HOST;
std::string OS_PORT;
std::string OS_INDEX;
std::string URL_PATH;

void handle_create(const std::vector<std::string> &paths);
void handle_delete(const std::vector<std::string> &paths);
void handle_move(const std::vector<std::string> &paths);
void handle_modify(const std::vector<std::string> &paths);
void handle_done(const std::vector<std::string> &paths);

const std::unordered_map<int, std::function<void(const std::vector<std::string> &)>> action_map =
    {{CREATE, handle_create},
     {DELETE, handle_delete},
     {MOVED_FROM, handle_move},
     {MODIFY, handle_modify},
     {DONE, handle_done}};

void handle_create(const std::vector<std::string> &paths)
{
    Registry *reg = Registry::getRegistry();
    std::vector<Extractor *> vExtractorAll = reg->getExtractors("*");
    std::unordered_map<std::string, std::string> result_map{};
    std::unordered_map<std::string, std::vector<std::string>> extension_map{};

    // sort files by extension
    for (const std::filesystem::path &fp : paths)
    {
        std::string ext = fp.extension().string();
        std::string fp_string = fp.string();
        if (ext.empty())
        {
            extension_map["empty"].push_back(fp_string);
        }
        else
        {
            extension_map[ext].push_back(fp_string);
        }
    }

    // pass files to extractors, based on extension
    for (const auto &[extension, files_with_extension] : extension_map)
    {
        std::vector<Extractor *> vExtractor = reg->getExtractors(extension);
        vExtractor.insert(vExtractor.end(), vExtractorAll.begin(), vExtractorAll.end()); // add the "all" extractors to the extractors for the specific extension
        for (const auto &e : vExtractor)
        {
            // std::cout << Color::warning_message(e->getName()) << std::endl;
            extraction_pool->detach_task(
                [e, &files_with_extension, &result_map]
                {
                    auto results = e->extract(files_with_extension);
                    // std::cout << results[1] << std::endl;
                    std::lock_guard<std::mutex> lock(map_mutex);
                    for (const auto &[id, res] : results)
                    {
                        result_map[id] += res;
                    }
                });
        }
    }
    // wait for all extractors to finish, so the results can be combined
    extraction_pool->wait();
    if (result_map.empty())
    {
        std::cerr << Color::colorize("Empty metadata vector. Probably error in use_extractor.", Color::RED) << std::endl;
        throw std::runtime_error(Color::error_message() + " metadata vector empty after using extractor.");
    }
    boost::asio::io_context io;
    POST_Request pr(OS_HOST, OS_PORT, URL_PATH, io);
    try
    {
        std::string all_files = {};
        for (const auto &[id, md] : result_map)
        {
            all_files += "{\"create\": {\"_id\": \"" + id + "\"}}\n{" + md + "}\n";
        }
        pr.send_post(all_files);
        std::string answer = pr.receive_answer();
        std::string em = get_opensearch_error_message(answer);
        if (!em.empty())
        {
            std::cout << Color::warning_message(all_files);
            std::cout << Color::warning_message("OpenSearch Error: ") << em << std::endl
                      << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << Color::MAGENTA << "ERROR IN HANDLE_CREATE" << Color::RESET << std::endl;
        std::cerr << e.what() << std::endl;
    }
}
void handle_delete(const std::vector<std::string> &paths)
{
    // std::cout << Color::info_message("Deleting.\n");
    boost::asio::io_context io;
    POST_Request pr(OS_HOST, OS_PORT, URL_PATH, io);
    std::string delete_messages{};
    for (const std::string &fp : paths)
    {
        std::string id = getHash(fp);
        delete_messages = R"({"delete": {"_id":)" + id + " }}\n";
    }
    pr.send_post(delete_messages);
    std::string answer = pr.receive_answer();
    std::string em = get_opensearch_error_message(answer);
    if (em != "")
    {
        std::cout << Color::warning_message() << em << std::endl;
    }
}

void handle_move(const std::vector<std::string> &paths)
{
}

void handle_modify(const std::vector<std::string> &paths)
{
    boost::asio::io_context io;
    POST_Request pr(OS_HOST, OS_PORT, URL_PATH, io);
    std::string modify_messages{};
    for (const std::string &fp : paths)
    {
        std::string id = getHash(fp);
        modify_messages = R"({"update": {"_id":)" + id + " }}\n";
    }
    pr.send_post(modify_messages);
    std::string answer = pr.receive_answer();
    std::cout << answer << std::endl;
    std::string em = get_opensearch_error_message(answer);
    if (em != "")
    {
        std::cout << Color::warning_message() << em << std::endl;
    }
}

void handle_done(const std::vector<std::string> &paths)
{
    std::cout << "Done." << std::endl;
}

bool handle_message(std::string message)
{
    try
    {
        std::unordered_map<int, std::vector<std::string>> event_map;
        std::vector<std::string> files;
        std::vector<std::string> paths;
        std::stringstream ss(message);
        std::string file;
        if (!message.empty())
        {
            while (std::getline(ss, file, '\n'))
            {
                files.push_back(file);
            }
        }
        for (std::string file : files)
        {
            if (file == "")
            {
                std::cerr << Color::warning_message() << "Empty file name, skipping.\n";
                continue;
            }
            std::string delimiter = " ";
            size_t pos = file.find(delimiter);

            if (pos == std::string::npos)
            {
                std::cerr << Color::error_message() << " Message not in expected format!" << std::endl;
                std::cerr << "File: " << file << std::endl;
                return false;
            }

            int event = std::stoi(file.substr(0, pos)); // i.e. CREATE, MOVED_TO, ... (based on linux tool inotify)
            if (pos + 1 > file.length())
            {
                std::cerr << Color::error_message() << " Invalid position for path extraction" << std::endl;
                std::cerr << file << std::endl;
                return false;
            }
            std::string path = file.substr(pos + 1, file.length());

            std::unordered_map<int, std::function<void(const std::vector<std::string> &)>>::const_iterator got = action_map.find(event);
            if (got == action_map.end())
            {
                std::cerr << Color::error_message() << " EVENT not found." << std::endl;
                std::cerr << event << std::endl;
                return false;
            }
            event_map[event].push_back(path);
        }
        for (const auto &[e, v] : event_map)
        {
            // pool_handle.detach_task(
            //     [e,v]{
            action_map.at(e)(v);
            //     }
            // );
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }
    return true;
}

// Source: https://www.codingwiththomas.com/blog/boost-asio-server-client-example
// Last access: 27.Mai.2026
class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket socket) : socket_(std::move(socket)) {}

    void start()
    {
        do_read();
    }

private:
    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                [this, self](boost::system::error_code ec, std::size_t length)
                                {
                                    if (!ec)
                                    {
                                        std::string message(data_, length);
                                        // std::cout << Color::info_message("Message: " + message) << std::endl;
                                        if (length > max_length)
                                        {
                                            std::cerr << Color::error_message() << " Message too long." << std::endl;
                                            do_write("ERROR: Message too long");
                                            throw std::length_error(Color::error_message() + "Received data package too large to handle.");
                                        }
                                        if (handle_message(message))
                                            do_write("OK");
                                        else
                                            do_write("ERROR: handle message");
                                        do_read();
                                    }
                                    else if (ec != boost::asio::error::eof && ec != boost::asio::error::connection_reset)
                                    {
                                        do_write("ERROR: error in connection");
                                        std::cout << "Shutdown." << std::endl;
                                    }
                                });
    }

    void do_write(const std::string &response)
    {

        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(response + "\n"),
                                 [this, self, response](std::error_code ec, size_t length)
                                 {
                                     if (ec)
                                     {
                                         std::cerr << Color::error_message() << " Error while sending response. " << std::endl;
                                         std::cerr << ec.message() << std::endl;
                                     }
                                     //  else
                                     //  {
                                     //     //  std::cout << Color::colorize("INFO:", Color::BOLD, Color::CYAN) << " Response sent: " << response << std::endl;
                                     //  }
                                 });
    }

    tcp::socket socket_;
    enum
    {
        max_length = 4096
    };
    char data_[max_length];
};

class TCPServer
{
public:
    TCPServer(boost::asio::io_context &io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
        do_accept();
    }

private:
    void do_accept()
    {
        acceptor_.async_accept(
            [this](std::error_code ec, tcp::socket socket)
            {
                if (!ec)
                {
                    std::make_shared<Session>(std::move(socket))->start();
                }
                do_accept();
            });
    }

    tcp::acceptor acceptor_;
};

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <path/to/settings.toml>\n";
        return 1;
    }

    // source: https://toruniina.github.io/toml11/docs/features/
    // access date: 26. May. 2026 10:53
    try
    {
        std::filesystem::path path_to_settings(argv[1]);
        if (!std::filesystem::exists(path_to_settings))
        {
            std::cerr << "Invalid path to settings.\n";
            std::cerr << "Usage: " << argv[0] << " <path/to/settings.toml>\n";
            return 1;
        }

        // settings for server
        const auto settings = toml::parse(path_to_settings);
        const auto SERVER_SETTINGS = toml::find(settings, "server");
        int server_port = toml::find<int>(SERVER_SETTINGS, "PORT");
        int n_threads = toml::find<int>(SERVER_SETTINGS, "EXTRACTOR_THREADS");
        extraction_pool = new BS::thread_pool<>(n_threads);

        // settings for OpenSearch
        const auto OS_SETTINGS = toml::find(settings, "opensearch");
        OS_PORT = toml::find<std::string>(OS_SETTINGS, "PORT");
        OS_HOST = toml::find<std::string>(OS_SETTINGS, "HOST");
        OS_INDEX = toml::find<std::string>(OS_SETTINGS, "INDEX");
        URL_PATH = "/" + OS_INDEX + "/_bulk";

        // settings for extractors
        const auto EXTRACTOR_SETTINGS = toml::find(settings, "extractors");
        // std::vector<std::string> exiftool_types = toml::find<std::vector<std::string>>(EXTRACTOR_SETTINGS, "Exif");
        std::vector<std::string> czi_types = toml::find<std::vector<std::string>>(EXTRACTOR_SETTINGS, "CZI");
        std::vector<std::string> fie_types = toml::find<std::vector<std::string>>(EXTRACTOR_SETTINGS, "Fileinfo");
        std::vector<std::string> exiv2_types = toml::find<std::vector<std::string>>(EXTRACTOR_SETTINGS, "EXIV2");
        // Exiftool_Extractor exiftool_extractor(exiftool_types);
        CZI_Extractor czi(czi_types);
        Exiv2_Extractor exiv2_extractor(exiv2_types);
        FilesystemInfo_Extractor fie(fie_types);

        // starting server
        boost::asio::io_context io_context;
        TCPServer server(io_context, server_port);
        std::cout << Color::colorize("Running on port: ", Color::CYAN) << server_port << std::endl;
        io_context.run();
    }
    catch (const toml::syntax_error &err)
    {
        std::cerr << err.what() << std::endl;
        return 1;
    }
    catch (const toml::type_error &err)
    {
        std::cerr << err.what() << std::endl;
        return 1;
    }
    catch (const std::exception &e)
    {
        std::cerr << Color::colorize("ERROR: ", Color::BOLD, Color::RED) << e.what() << std::endl;
        return 1;
    }
    catch (...)
    {
        std::cerr << Color::colorize("Unexpected Error.", Color::BOLD, Color::RED) << std::endl;
        return 1;
    }

    return 0;
}