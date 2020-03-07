
#include "../php_pdlib.h"
#include "face_detection.h"

#include <zend_exceptions.h>
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
    long upsample_num = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &img_path, &img_path_len, &upsample_num) == FAILURE) {
        zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, "Unable to parse dlib_face_detection arguments");
        RETURN_FALSE;
    }
    try {
        frontal_face_detector detector = get_frontal_face_detector();

        pyramid_down<2> pyr;
        array2d<unsigned char> img;
        load_image(img, img_path);

        unsigned int levels = upsample_num;
        while (levels > 0) {
            levels--;
            pyramid_up(img, pyr);
        }

        array_init(return_value);

        std::vector<rectangle> dets = detector(img);
        for (unsigned long i = 0; i < dets.size(); ++i) {
            rectangle rect = pyr.rect_down(dets[i], upsample_num);

            zval rect_arr;
            array_init(&rect_arr);
            add_assoc_long(&rect_arr, "left", rect.left());
            add_assoc_long(&rect_arr, "top", rect.top());
            add_assoc_long(&rect_arr, "right", rect.right());
            add_assoc_long(&rect_arr, "bottom", rect.bottom());
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

