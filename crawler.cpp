#include <argparse/argparse.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <fstream>
#include <mutex>
#include <unordered_map>
#include <vector>
#include "includes/headers/crawler_class.hpp"
#include "includes/headers/client.hpp"
#include "includes/headers/protocol.h"
#include "includes/headers/color_print.hpp"
#include "includes/headers/toml.hpp"

std::string SERVER_PORT{};
std::string SERVER_IP{};
std::mutex vector_mutex;
unsigned int file_count = 0;
std::unordered_map<std::string, int> extension_count;
std::vector<std::string> path_vector;
const size_t MAX_SIZE = 4096;

void send_data(const std::vector<std::string> &paths)
{
    // TODO: get timing right, wait for answers before sending more data as to not overwhelm the server
    std::string response = "";
    unsigned short try_count = 0;
    Client *client = Client::getClientInstance(SERVER_IP, SERVER_PORT);
    std::string message = "";
    for (const std::string path : paths)
    {
        std::string next_path = std::to_string(CREATE) + " " + path + "\n";
        if ((message + next_path).length() > MAX_SIZE)
        {
            response = client->sendMessage(message);
            if (response != "ERROR sending failed")
            {
                std::cout << Color::info_message() << response << std::endl;
            }
            message = "";
        }
        message += next_path;
    }
    if (message != "")
    {
        response = client->sendMessage(message);
    }
    if (response == "ERROR")
    {
        std::cerr << "Error in response. Exiting." << std::endl;
        exit(1);
    }
}

void print_map(std::ostream &os)
{
    for (const auto &[key, value] : extension_count)
    {
        os << key << "\t" << value << "\n";
    }
}

void write_to_file()
{
    if (!extension_count.empty())
    {
        std::ofstream outFile("results/output_file.txt");

        // TODO: Fehler loggen, statt nur cerr?
        if (!outFile.is_open())
        {
            std::cerr << "ERROR: Datei konnte nicht erstellt werden!\n";
            print_map(std::cout);
        }
        else
        {
            print_map(outFile);
        }
        outFile.close();
    }
}

int main(int argc, char *argv[])
{

    auto my_processor = [&](const std::string &path)
    {
        std::lock_guard<std::mutex> lock(vector_mutex);
        path_vector.push_back(path);
        file_count++;
        // FIXME: Too simple. What happens when less than BATCH_SIZE files? How to know when last path
        // if (path_vector.size() >= BATCH_SIZE){
        //     // std::cout << "Sending batch of data." << std::endl;
        //     send_data(path_vector);
        //     path_vector.clear();
        // }
    };

    argparse::ArgumentParser program("Crawler");
    program.add_argument("-t", "--target")
        .help("Folder to start crawling");
    program.add_argument("-s", "--settings")
        .help("Path to settings")
        .default_value("../settings/settings.toml");

    size_t n_proc_task;
    size_t n_proc_crawl;
    std::string target_folder;
    try
    {
        program.parse_args(argc, argv);
        std::filesystem::path path_to_settings = program.get("-s");
        target_folder = program.get("-t");
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
        SERVER_PORT = std::to_string(server_port);
        SERVER_IP = toml::find<std::string>(SERVER_SETTINGS, "IP");

        const auto CRAWLER_SETTINGS = toml::find(settings, "crawler");
        n_proc_task = toml::find<int>(CRAWLER_SETTINGS, "threads_for_task");
        n_proc_crawl = toml::find<int>(CRAWLER_SETTINGS, "threads_for_crawling");

        clock_t start_time, end_time;
        double time_taken;
        auto t1 = std::chrono::high_resolution_clock::now();

        CrawlerBase<decltype(my_processor)> cb(target_folder, my_processor, n_proc_crawl, n_proc_task);
        cb.start_crawl_pool();
        cb.wait_all();
        std::cout << Color::warning_message(std::to_string(path_vector.size())) << std::endl;
        // for (const auto &s : path_vector)
        // {
        //     std::cout << Color::warning_message(s) << std::endl;
        // }
        // write_to_file();
        if (!path_vector.empty())
        {
            std::cout << "Vector not empty yet, sending data." << std::endl;
            send_data(path_vector);
            path_vector.clear();
        }
        Client *client = Client::getClientInstance(SERVER_IP, SERVER_PORT);
        const std::string done_message = std::to_string(DONE) + " Done\n";
        std::cout << "Res: " << client->sendMessage(done_message) << std::endl;
        std::cout << "Amount of files: " << file_count << std::endl;
        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<long double, std::milli> ms_double = t2 - t1;
        std::cout << ms_double.count() << " ms\n";
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
    catch (const std::exception &err)
    {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        return 1;
    }

    return 0;
}