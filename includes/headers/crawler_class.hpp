#ifndef CRAWLER_CLASS
#define CRAWLER_CLASS
#include "BS_thread_pool.hpp"
#include <filesystem>


template <typename FileProcessor>
class CrawlerBase{
private:
    FileProcessor file_processor;
    std::string folder_to_search_in;
    BS::thread_pool<> pool_crawler;
    BS::thread_pool<> pool_task;
public:
    CrawlerBase(std::string folder, FileProcessor proc, size_t num_threads_crawl = 16, size_t num_threads_task = 16);
    void crawl_directory(std::string dir_path);
    void wait_all();
    void start_crawl_pool();
};

template <typename FileProcessor>
CrawlerBase<FileProcessor>::CrawlerBase(std::string folder, FileProcessor proc, size_t num_threads_crawl, size_t num_threads_task): 
    folder_to_search_in{folder}, file_processor{proc}, pool_crawler{num_threads_crawl}, pool_task{num_threads_task}{}
template <typename FileProcessor>
void CrawlerBase<FileProcessor>::crawl_directory(std::string dir_path) {

    for (const auto& entry : std::filesystem::directory_iterator(dir_path)){
        // If it's a directory, queue a task to crawl into it
        //follows symbolic links
        std::string entry_name = entry.path().string();
        if (entry.is_directory()) {
            pool_crawler.detach_task(
                [this,entry_name]
                {
                    this->crawl_directory(entry_name);
                }
            );
        }
        // If it's a regular file, collect metadata
        else if (entry.is_regular_file()) {
            pool_task.detach_task(
                [this,entry_name]
                {
                    file_processor(entry_name);
                }
            );
        } 
        // else std::cerr << "Error: Can't determine filetype for file: " << entry_name << ".\n";
    }
}

template <typename FileProcessor>
void CrawlerBase<FileProcessor>::wait_all(){
    this->pool_crawler.wait();
    this->pool_task.wait();
}

template <typename FileProcessor>
void CrawlerBase<FileProcessor>::start_crawl_pool(){
    pool_crawler.detach_task(
        [this]
        {
            crawl_directory(folder_to_search_in);
        }
    );
}

#endif