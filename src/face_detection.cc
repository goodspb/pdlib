
#include "../php_pdlib.h"
#include "face_detection.h"

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <iostream>

using namespace dlib;
using namespace std;

PHP_FUNCTION(dlib_face_detection)
{
    char  *img_path;
    size_t  img_path_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &img_path, &img_path_len) == FAILURE){
        RETURN_FALSE;
    }
    try {
        frontal_face_detector detector = get_frontal_face_detector();

        array2d<unsigned char> img;
        load_image(img, img_path);

        pyramid_up(img);
        std::vector<rectangle> dets = detector(img);
        RETURN_LONG(dets.size());
    }
    catch (exception& e)
    {
        RETURN_FALSE;
    }
}

