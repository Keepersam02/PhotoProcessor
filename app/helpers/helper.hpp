#pragma once
#ifndef HELPER_H
#define HELPER_H


#include <format>
#include <string>
#include <ctime>



std::string epochToReadable(int64_t time){
    time_t t = static_cast<time_t>(time);

    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%D %I:%M %p", std::gmtime(&t));
    return buffer;
}



#endif