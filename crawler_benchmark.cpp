#include <argparse/argparse.hpp>
#include <benchmark/benchmark.h>
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

void send_data(const std::vector<std::string> &paths)
{
    // TODO: get timing right, wait for answers before sending more data as to not overwhelm the server
    std::string response = "";
    unsigned short try_count = 0;
    Client *client = Client::getClientInstance("127.0.0.1", "9200");
    std::string message = "";
    for (const std::string path : paths)
    {
        std::string next_path = std::to_string(CREATE) + " " + path + "\n";
        if ((message + next_path).length() > 4096)
        {
            response = client->sendMessage(message);
            /*if (response != "ERROR sending failed")
            {
                std::cout << Color::info_message() << response << std::endl;
            }*/
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

void send_files_to_server(const std::vector<std::string> &path_vector)
{
    if (!path_vector.empty())
    {
        std::cout << "Vector not empty yet, sending data." << std::endl;
        send_data(path_vector);
    }
    Client *client = Client::getClientInstance("127.0.0.1", "9200");
    const std::string done_message = std::to_string(DONE) + " Done\n";
    std::cout << "Res: " << client->sendMessage(done_message) << std::endl;
    // std::cout << Color::info_message() << "sent all files.\n";
}

// Source - https://stackoverflow.com/a/6406411
// Posted by johnsyweb, modified by community. See post 'Timeline' for change history
// Retrieved 2026-08-07, License - CC BY-SA 4.0
void write_files_for_updater(const std::vector<std::string> &path_vector)
{
    if (!path_vector.empty())
    {
        std::ofstream output_file("./updater_files_new");

        std::ostream_iterator<std::string> output_iterator(output_file, "\n");
        std::copy(std::begin(path_vector), std::end(path_vector), output_iterator);
    }
}

class CrawlerFixture : public benchmark::Fixture
{
public:
    std::vector<std::string> path_vector{};
    std::mutex vector_mutex;
    int file_count = 0;
    std::unique_ptr<CrawlerBase> cb; // ← Kein Template-Parameter!

    void SetUp(const ::benchmark::State &state) override
    {
        int n_proc_crawl = state.range(0);
        int n_proc_task = state.range(1);
        std::string target_folder = "D:\\";

        path_vector.clear();
        file_count = 0;

        // Lambda in std::function verpacken
        std::function<void(const std::string &)> my_processor =
            [this](const std::string &path)
        {
            std::lock_guard<std::mutex> lock(vector_mutex);
            path_vector.push_back(path);
            file_count++;
        };

        // Crawler initialisieren (außerhalb des Timings)
        cb = std::make_unique<CrawlerBase>(target_folder, my_processor, n_proc_crawl, n_proc_task);
    }
    void TearDown(const ::benchmark::State &state) override
    {
        cb.reset();
        path_vector.clear();
    }
};

BENCHMARK_DEFINE_F(CrawlerFixture, BM_Crawl)(benchmark::State &state)
{
    for (auto _ : state)
    {
        // Reset vor jeder Iteration
        path_vector.clear();
        file_count = 0;

        // Crawl ausführen (z.B. via run() oder im Konstruktor)
        cb->start_crawl_pool(); // oder whatever Methode den Crawl startet
        cb->wait_all();

        // Verhindern, dass der Compiler optimiert
        benchmark::DoNotOptimize(path_vector);
        benchmark::DoNotOptimize(file_count);
        benchmark::ClobberMemory();
    }
}

BENCHMARK_REGISTER_F(CrawlerFixture, BM_Crawl)
    ->Args({4, 2})
    ->Args({8, 2})
    ->Args({16, 2});

BENCHMARK_F(CrawlerFixture, BM_WriteData)(benchmark::State &state)
{
    for (auto _ : state)
    {
        write_files_for_updater(path_vector);
        benchmark::DoNotOptimize(path_vector);
        benchmark::ClobberMemory();
    }
}

BENCHMARK_F(CrawlerFixture, BM_SendData)(benchmark::State &state)
{
    for (auto _ : state)
    {
        send_files_to_server(path_vector);
        benchmark::DoNotOptimize(path_vector);
        benchmark::ClobberMemory();
    }
}

BENCHMARK_MAIN();