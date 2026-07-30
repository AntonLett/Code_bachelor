#pragma once
#include "BS_thread_pool.hpp"



class FiletypeFinder{
private:
    std::unordered_map<std::string, int> ending_count;
    std::string folder_to_search_in;
    std::string output_file;
    BS::thread_pool<> pool_crawler;
    BS::thread_pool<> pool_mapper;
    std::mutex map_mutex;
public:
    FiletypeFinder(std::string folder, std::string output = "file_endings_folder.txt", int num_threads = 16);
    void crawl_directory(std::string dir_path);
    void write_results_to_file();
    void check_file_ending(const std::string& full_path);
    void wait_all();
    void start_crawl_pool();
    void print_map(std::ostream& os);
};