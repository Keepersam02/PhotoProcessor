#pragma once
#ifndef PIPELINE_OPERATION_MAP_H
#define PIPELINE_OPERATION_MAP_H

#include <iostream>

struct PipelineOperationMap {
    // DB variables
    int32_t         id;
    int32_t         pipeline_id;
    int32_t         operation_id;
    int32_t         position;

};

#endif