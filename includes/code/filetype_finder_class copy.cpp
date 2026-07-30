#include "../headers/filetype_finder_class.hpp"
#include <fstream>
#include <string.h>
#include <filesystem>

FiletypeFinder::FiletypeFinder(std::string folder, std::string output, int num_threads): 
    folder_to_search_in{folder}, output_file{output}, pool_crawler{num_threads/2+num_threads%2}, pool_mapper{num_threads/2}{}

void FiletypeFinder::write_results_to_file(){
    if (!this->ending_count.empty()){
        std::ofstream outFile(output_file);
        
        //TODO: Fehler loggen, statt nur cerr?
        if(!outFile.is_open()){
            std::cerr << "ERROR: Datei '" << output_file << "' konnte nicht erstellt werden!\n";
            print_map(std::cout);
        }
        else{   
            print_map(outFile);
        }
        outFile.close();
    }
}

void FiletypeFinder::crawl_directory(std::string dir_path) {

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
            pool_mapper.detach_task(
                [this,entry_name]
                {
                    check_file_ending(entry_name);
                }
            );
        } 
        // else std::cerr << "Error: Can't determine filetype for file: " << entry_name << ".\n";
    }
}

void FiletypeFinder::check_file_ending(const std::string& full_path) {
    size_t slash_pos = full_path.find_last_of("\\/");
    size_t dot_pos = full_path.find_last_of('.');

    // No dot -> no file ending
    if (dot_pos == std::string::npos)
        return;
    // dot before final slash -> no file ending
    if (slash_pos != std::string::npos && dot_pos < slash_pos)
        return;
    // otherwise, everything after the dot is the ending
    std::lock_guard<std::mutex> lock(map_mutex);
    ending_count[full_path.substr(dot_pos + 1)]++;
}

void FiletypeFinder::wait_all(){
    this->pool_crawler.wait();
    this->pool_mapper.wait();
}

void FiletypeFinder::start_crawl_pool(){
    pool_crawler.detach_task(
        [this]
        {
            crawl_directory(folder_to_search_in);
        }
    );
}

void FiletypeFinder::print_map(std::ostream& os){
    for (const auto& [key, value] : this->ending_count){
        os << key << "\t" << value << "\n";
    }
}