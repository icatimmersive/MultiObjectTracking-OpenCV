//
// Created by Eric on 10/11/2015.
//

#ifndef MULTIOBJECTTRACKING_C_BLOB_H
#define MULTIOBJECTTRACKING_C_BLOB_H
#include <stdlib.h>
struct Blob {
    size_t cameraID;
    size_t id;
    int bounding_x;
    int bounding_y;
    int bounding_width;
    int bounding_height;

    //these fields were not being set by matlab
    double origin_x = 0.0;
    double origin_y = 0.0;
    double origin_z = 0.0;
    int orientation_x = 0;
    int orientation_y = 0;
    int orientation_z = 0;
    int orientation_theta = 0;
    char const *age; //can be added by correct wrapper call
    //updateTime
    //creationTime

    // quick fix for camera image size
    int image_width;
    int image_height;
};
typedef struct Blob Blob;
#endif //MULTIOBJECTTRACKING_C_BLOB_H
