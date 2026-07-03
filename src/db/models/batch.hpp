#pragma once
#ifndef BATCH_H
#define BATCH_H

#include <string>
#include <iostream>


/*
TODO:

- figure out enum 

*/

struct Batch {
    // DB variables
    int32_t         id;
    std::string     name;
    int64_t         dateCreated;
    int64_t         dateModified;
    int8_t          status;

    // CLI variables
    bool            is_selected{false};
    std::string     dateCreatedFormatted = "";
    std::string     dateModifiedFormatted = "";
};

enum class BatchStatus {
    Pending    = 0,
    Processing = 1,
    Complete   = 2,
    Failed     = 3,
    Deleted    = -1,
};


#endif