
#include "../php_pdlib.h"
#include "face_landmark_detection.h"

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/image_io.h>
#include <iostream>

#define ARRAY_NAME_WITH_INDEX(name, index) name##index

using namespace dlib;
using namespace std;

PHP_FUNCTION(dlib_face_landmark_detection)
{
    char  *shape_predictor_file_path;
    char  *img_path;
    size_t  shape_predictor_file_path_len, img_path_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS(), "ss", &shape_predictor_file_path, &shape_predictor_file_path_len,
            &img_path, &img_path_len) == FAILURE){
        RETURN_FALSE;
    }
    zval return_val;
    array_init(&return_val);

    try {
        frontal_face_detector detector = get_frontal_face_detector();
        shape_predictor sp;
        deserialize(shape_predictor_file_path) >> sp;

        array2d<rgb_pixel> img;
        load_image(img, img_path);
        pyramid_up(img);

        std::vector<rectangle> dets = detector(img);

        std::vector<full_object_detection> shapes;
        for (unsigned long j = 0; j < dets.size(); ++j)
        {
            full_object_detection shape = sp(img, dets[j]);

            zval ARRAY_NAME_WITH_INDEX(face, j);
            array_init(&ARRAY_NAME_WITH_INDEX(face, j));
            for (int k = 0; k < shape.num_parts(); k++) {
                zval ARRAY_NAME_WITH_INDEX(part, k);
                array_init(&ARRAY_NAME_WITH_INDEX(part, k));
                dlib::point p = shape.part(k);
                add_next_index_long(&ARRAY_NAME_WITH_INDEX(part, k),p.x());
                add_next_index_long(&ARRAY_NAME_WITH_INDEX(part, k),p.y());
                add_next_index_zval(&ARRAY_NAME_WITH_INDEX(face, j), &ARRAY_NAME_WITH_INDEX(part, k));
            }
            add_next_index_zval(&return_val, &ARRAY_NAME_WITH_INDEX(face, j));
        }
        RETURN_ZVAL(&return_val, 0, 0);
    }
    catch (exception& e)
    {
        RETURN_FALSE;
    }
}