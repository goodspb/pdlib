
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

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &img_path, &img_path_len) == FAILURE) {
        RETURN_FALSE;
    }
    try {
        frontal_face_detector detector = get_frontal_face_detector();

        array2d<unsigned char> img;
        load_image(img, img_path);

        array_init(return_value);

        std::vector<rectangle> dets = detector(img);
        for (unsigned long i = 0; i < dets.size(); ++i) {
            zval rect_arr;
            array_init(&rect_arr);
            add_assoc_long(&rect_arr, "left", dets[i].left());
            add_assoc_long(&rect_arr, "top", dets[i].top());
            add_assoc_long(&rect_arr, "right", dets[i].right());
            add_assoc_long(&rect_arr, "bottom", dets[i].bottom());
            // Add this assoc array to returned array
            //
            add_next_index_zval(return_value, &rect_arr);
        }
    }
    catch (exception& e)
    {
        RETURN_FALSE;
    }
}

