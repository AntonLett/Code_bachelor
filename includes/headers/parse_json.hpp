#pragma once
#include <iostream>
#include <stdexcept>
#include "simdjson.h"
#include "color_print.hpp"

std::string get_opensearch_error_message(const std::string& json_string){
    simdjson::dom::parser parser;
    auto result = parser.parse(json_string);
    if (result.error()) {
        std::cerr << Color::error_message() << " while parsing: " << simdjson::error_message(result.error()) << std::endl;
        throw std::runtime_error(Color::error_message() + " while parsing: " + simdjson::error_message(result.error()));
    }
    simdjson::dom::element doc = result.value();
    simdjson::dom::object obj = doc.get_object();

    if (obj["errors"]){
        if (!obj["items"].is_array()) {
            std::cerr << "'items' is not an Array or does not exist!" << std::endl;
            throw std::runtime_error(Color::error_message() + " unexpected json format");
        }
        simdjson::dom::array items = obj["items"].get_array();
        if (items.size() == 0) {
            std::cerr << "'items' is empty!" << std::endl;
            throw std::runtime_error(Color::error_message() + " unexpected json format");
        }

        for (simdjson::dom::element item : items)
        {
            simdjson::dom::object item_obj = item.get_object();

            for (auto field : item_obj)
            {
                simdjson::dom::object operation_obj = field.value.get_object();
                auto error_result = operation_obj["error"];

                if (error_result.error() != simdjson::SUCCESS)
                    continue;

                simdjson::dom::object error_obj = error_result.get_object();

                auto reason_result = error_obj["reason"];

                if (reason_result.error() != simdjson::SUCCESS)
                    continue;

                return std::string(reason_result);
            }
        }/*
        simdjson::dom::element first_item = items.at(0);
        if (!first_item.is_object()) {
            std::cerr << Color::error_message() << "First element in 'items' is not an object!" << std::endl;
            throw std::runtime_error(Color::error_message() + " unexpected json format");
        }
        simdjson::dom::object first_obj = first_item.get_object();
        std::string error_message = std::string(first_obj["create"]["error"]["reason"]);
        return error_message;*/
    } 
    else return "";
}