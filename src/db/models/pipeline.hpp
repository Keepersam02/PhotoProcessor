#pragma once
#ifndef PIPELINE_H
#define PIPELINE_H

#include <string>
#include <iostream>


struct Pipeline {
    // DB variables
    int32_t         id;
    std::string     name;
    int64_t         dateCreated;
    int64_t         dateModified;
    int8_t          status;

    // CLI variables
    bool            is_selected;
    std::string     dateCreatedFormatted;
    std::string     dateModifiedFormatted;
};

#endif