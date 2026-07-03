#pragma once
#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <string>
#include <iostream>


struct Directory {
    // DB variables
    int32_t         id;
    std::string     name;
    std::string     path;
    int32_t         number_of_files;
    int64_t         dateCreated;
    int64_t         dateModified;

    // CLI variables
    bool            is_selected{false};
    std::string     dateCreatedFormatted = "";
    std::string     dateModifiedFormatted = "";
};

#endif