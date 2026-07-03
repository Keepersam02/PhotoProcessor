#pragma once
#ifndef FILE_H
#define FILE_H

#include <string>
#include <iostream>


struct File {
    // DB variables
    int32_t         id;
    int32_t         source_id;
    int32_t         source_id;
    std::string     name;
    std::string     path;
    int64_t         dateCreated;
    int64_t         dateModified;

    // // CLI variables
    // bool            is_selected{false};
    // std::string     dateCreatedFormatted = "";
    // std::string     dateModifiedFormatted = "";
};

#endif