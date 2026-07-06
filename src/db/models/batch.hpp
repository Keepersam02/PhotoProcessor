#pragma once
#ifndef BATCH_H
#define BATCH_H

#include <string>
#include <iostream>


/*
TODO:

- figure out enum 

*/

enum class BatchStatus : int8_t {
    Pending    = 0,
    Processing = 1,
    Complete   = 2,
    Failed     = 3,
    Deleted    = -1,
};

struct Batch {
    // DB variables
    int32_t         id;
    std::string     name;
    int64_t         dateCreated;
    int64_t         dateModified;
    BatchStatus     status;

    // CLI variables
    bool            is_selected{false};
    std::string     dateCreatedFormatted = "";
    std::string     dateModifiedFormatted = "";
};



#endif