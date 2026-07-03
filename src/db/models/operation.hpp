#pragma once
#ifndef OPERATION_H
#define OPERATION_H

#include <string>

struct Operation {
    // DB variables
    int32_t         id;
    std::string     name;

    // CLI variables
    bool            is_selected{false};
};

#endif