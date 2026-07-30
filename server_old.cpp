#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
#include <filesystem>
#include <iostream>
#include <magic.h>
#include <memory>
#include <stdexcept>    
#include <string>
#include <sys/stat.h>
#include <unordered_map>
#include <vector>
#include "includes/exiftool/inc/ExifTool.h"
#include "includes/headers/POST_Request.hpp"
#include "includes/headers/check_if_quotes_needed.hpp"
#include "includes/headers/settings_reader.hpp"
#include "includes/headers/color_print.hpp"
#include "includes/headers/parse_json.hpp"
#include "includes/headers/protocol.h"
#include "includes/headers/BS_thread_pool.hpp"

using boost::asio::ip::tcp;
BS::thread_pool<> pool_handle(4);


std::vector<std::string> magic_output;
// OPENSEARCH settings
std::string OS_HOST;
std::string OS_PORT;
std::string OS_INDEX;
std::string URL_PATH;

void handle_create(const std::vector<std::string>& paths);
void handle_delete(const std::vector<std::string>& paths);
void handle_move(const std::vector<std::string>& paths);
void handle_modify(const std::vector<std::string>& paths);
void handle_done(const std::vector<std::string>& paths);

const std::unordered_map<int, std::function<void(const std::vector<std::string>&)>> action_map = 
    {{CREATE, handle_create},
    {DELETE, handle_delete},
    {MOVED_FROM, handle_move},
    {MODIFY, handle_modify},
    {DONE, handle_done}};

const int get_inode_number(const std::string& filename){
    struct stat file_info;
    if (stat(filename.c_str(), &file_info) == -1) {
        perror("stat");
        throw std::runtime_error(Color::error_message() + "error using stat.\n");
    }
    return file_info.st_ino;
}

std::vector<std::string> use_exiftool(const std::vector<std::string>& paths){
    std::vector<std::string> results;
    ExifTool *et = new ExifTool();
    for (std::string fp : paths){
        unsigned int inode_num;
        inode_num = get_inode_number(fp);
        std::string metadata = "{\"index\": {\"_id\": \"" + std::to_string(inode_num) + "\"}}\n{";
        const char *file_path = fp.c_str();
        int cmdNum = et->ExtractInfo(file_path, "-File:all\n-s\n-a\n-FileGroupID\n");
        
        TagInfo *info = et->GetInfo(cmdNum, 1); 

        if (info) {
            // print returned information
            for (TagInfo *i=info; i; i=i->next) {
                std::string val = needsQuotes(std::string(i->value));
                metadata +=  "\"" + std::string(i->name) + "\": " + val;
                if(i->next) metadata += ",";
                else metadata += "}\n";
            }
            results.push_back(metadata);
            delete info;
        } else if (et->LastComplete() <= 0) {
            std::cout << fp << std::endl << std::endl;
            std::cerr << (Color::error_message("Error executing exiftool!\n"));
            std::cerr << "File might be too large, consider increasing timeout duration.\n";
        }
        // print exiftool stderr messages
        char *err = et->GetError();
        if (err) {
            throw std::runtime_error(Color::error_message() + std::string(err));
        }
    }
    delete et;      // delete our ExifTool object
    return results;
}

void use_magic(const std::vector<std::string> paths){
    magic_t h = magic_open(MAGIC_NONE); 
    magic_load(h, NULL);
    for (const std::string path : paths){
        const char* typed = magic_file(h, path.c_str());
        magic_output.push_back(typed ? typed : "UNKNOWN");
    }
    magic_close(h);
}

void handle_create(const std::vector<std::string>& paths){
    // std::cout << "CREATE: Path is: \'" << path << "\'" << std::endl;
    std::vector<std::string> metadata = use_exiftool(paths);
    if(metadata.empty()){
        std::cerr << Color::colorize("Empty metadata vector. Probably error in use_extractor.", Color::RED) << std::endl;
        throw std::runtime_error(Color::error_message() + " metadata vector empty after using extractor.");
    }
    boost::asio::io_context io;
    POST_Request pr(OS_HOST,OS_PORT,URL_PATH,io);
    for(std::string m : metadata){
        pr.send_post(m);
        std::string answer =  pr.receive_answer();
        std::string em = get_opensearch_error_message(answer);
        if (em != ""){
            std::cout << Color::warning_message() << em << std::endl;
        }
    }
}
void handle_delete(const std::vector<std::string>& paths){
    std::cout << Color::info_message("Deleting.\n");
    boost::asio::io_context io;
    POST_Request pr(OS_HOST,OS_PORT,URL_PATH,io);
    std::string delete_messages{}; 
    for (const std::string& fp : paths){
        unsigned int inode_num;
        inode_num = get_inode_number(fp);
        delete_messages = R"({"delete": {"_id":)" + std::to_string(inode_num) + " }}\n";
    }
    pr.send_post(delete_messages);
    std::string answer =  pr.receive_answer();
    std::cout << answer << std::endl;
    std::string em = get_opensearch_error_message(answer);
    if (em != ""){
        std::cout << Color::warning_message() << em << std::endl;
    }
}

void handle_move(const std::vector<std::string>& paths){

}

void handle_modify(const std::vector<std::string>& paths){

}

void handle_done(const std::vector<std::string>& paths){
    std::cout << "Done." << std::endl;
}

int handle_message(std::string message){
    std::unordered_map<int, std::vector<std::string>> event_map;
    std::vector<std::string> files;
    std::vector<std::string> paths;
    std::stringstream ss(message);
    std::string file;
    if (!message.empty()){
        while(std::getline(ss,file,'\n')){
            files.push_back(file);
        }
    }
    for (std::string file : files){
        if (file == "") {
            std::cerr << Color::warning_message() << "Empty file name, skipping.\n";
            continue;
        }
        std::string delimiter = " ";
        size_t pos = message.find(delimiter);
        
        if (pos == std::string::npos){
            std::cerr << Color::error_message()<< " Message not in expected format!" << std::endl;
            std::cerr << "File: " << file << std::endl;;
            return 1;
        }

        int event = std::stoi(file.substr(0, pos));    //i.e. CREATE, MOVED_TO, ... (based on linux tool inotify)
        if(pos+1 > file.length()) {
            std::cerr << Color::error_message() << " Invalid position for path extraction" << std::endl;
            std::cerr << file << std::endl;
            return 1;
        }
        std::string path = file.substr(pos+1, file.length());

        std::unordered_map<int,std::function<void(const std::vector<std::string>&)>>::const_iterator got = action_map.find(event);
        if ( got == action_map.end() ){
            std::cerr << Color::error_message() << " EVENT not found." << std::endl;
            std::cerr << event << std::endl;
            return 1;
        }
        event_map[event].push_back(path);
    }
    for(const auto& [e,v] : event_map){
        pool_handle.detach_task(
            [e,v]{
                action_map.at(e)(v);
            }
        );
    }
    return 0;
}

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket) : socket_(std::move(socket)) {}

    void start() {
        do_read();
    }

private:
    void do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::string message(data_, length);
                    std::cout << Color::info_message("Message: " + message) << std::endl;
                    if(length > max_length) {
                        std::cerr << Color::error_message() << " Message too long." << std::endl;
                        do_write("ERROR");
                        throw std::length_error(Color::error_message() + "Received data package too large to handle.");
                    }
                    if (!handle_message(message)) do_write("OK");
                    else do_write("ERROR");
                    do_read();  
                } else if (ec != boost::asio::error::eof && ec != boost::asio::error::connection_reset) {
                    do_write("ERROR");
                    std::cout << "Shutdown." << std::endl;
                }
            });
    }

    void do_write(const std::string& response) {

        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(response),
            [this, self, response](std::error_code ec, size_t length){
                if(!ec){
                    std::cout << Color::colorize("INFO:", Color::BOLD, Color::CYAN) << " Response sent: " << response << std::endl;
                }
                else {
                    std::cerr << Color::error_message() << " Error while sending response. " << std::endl;
                    std::cerr << ec.message() << std::endl;
                }
            });
    }

    tcp::socket socket_;
    enum { max_length = 4096 };
    char data_[max_length];
};

class TCPServer {
public:
    TCPServer(boost::asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(
            [this](std::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<Session>(std::move(socket))->start();
                }
                do_accept(); 
            });
    }

    tcp::acceptor acceptor_;
};

int main() {
    try {
        std::unordered_map<std::string, std::string> settings = parse_settings("settings/server.conf");
        OS_PORT  = settings.at("OS_PORT");
        OS_HOST  = settings.at("OS_HOST");
        OS_INDEX = settings.at("OS_INDEX");
        URL_PATH = "/" + OS_INDEX + "/_bulk";
        int server_port = std::stoi(settings.at("SERVER_PORT"));

        boost::asio::io_context io_context;
        TCPServer server(io_context, server_port); 
        std::cout << Color::colorize("Running ...", Color::CYAN) << std::endl; 
        io_context.run(); 
    } catch (const std::out_of_range& e){
        std::cerr << Color::colorize("ERROR: ", Color::BOLD, Color::RED) << "Key doesn't exist. Probably error in parsing settings.\n"
            << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << Color::colorize("ERROR: ", Color::BOLD, Color::RED) << e.what() << std::endl;
    }

    return 0;
}