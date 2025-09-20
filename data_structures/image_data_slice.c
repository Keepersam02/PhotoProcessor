//
// Created by keepersam on 9/18/25.
//

#include "image_data_slice.h"

#include <stdio.h>
#include <stdlib.h>


int init_image_data_slice(int capacity, void** struct_pointer) {
    image_data_slice* data = malloc(sizeof(image_data_slice));
    if (data == NULL) {
        free(data);
        fprintf(stderr, "Buy more ram");
        return -1;
    }
    *struct_pointer = data;
    return 0;
}

int add_image_data_slice(image_data_slice* image_data, float value) {
    if (image_data == NULL) {
        fprintf(stderr, "Null pointer passed");
        return -1;
    }
    if (image_data->size == image_data->capacity - 1) {
        fprintf(stderr, "image data at max size");
        return -2;
    }
    image_data->data[image_data->size] = value;
    image_data->size++;
    return 0;
}