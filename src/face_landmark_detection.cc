
#include "../php_pdlib.h"
#include "face_landmark_detection.h"

#include <zend_exceptions.h>

#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/image_io.h>
#include <iostream>

#define ARRAY_NAME_WITH_INDEX(name, index) name##index

using namespace dlib;
using namespace std;

static inline face_landmark_detection *php_face_landmark_detection_from_obj(zend_object *obj) {
    return (face_landmark_detection*)((char*)(obj) - XtOffsetOf(face_landmark_detection, std));
}

#define Z_FACE_LANDMARK_DETECTION_P(zv) php_face_landmark_detection_from_obj(Z_OBJ_P((zv)))

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

PHP_METHOD(FaceLandmarkDetection, __construct)
{
    char *sz_shape_predictor_file_path;
    size_t shape_predictor_file_path_len;

    face_landmark_detection *fld = Z_FACE_LANDMARK_DETECTION_P(getThis());

    if (nullptr == fld) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to find obj in FaceLandmarkDetection::__construct()");
        return;
    }

    // Parse predictor model's path
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
                &sz_shape_predictor_file_path, &shape_predictor_file_path_len) == FAILURE){
        zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, "Unable to parse shape_predictor_file_path");
        return;
    }

    // Load predictor model from given path
    try {
        string shape_predictor_file_path(sz_shape_predictor_file_path, shape_predictor_file_path_len);
        fld->sp = new shape_predictor;
        deserialize(shape_predictor_file_path) >> *(fld->sp);
    } catch (exception& e) {
        zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, e.what());
        return;
    }
}

// Helper macro to automatically have parsing of "top"/"bottom"/"left"/"right"
#define PARSE_BOUNDING_BOX_EDGE(side) \
    PARSE_LONG_FROM_ARRAY(bounding_box_hash, side, \
        "Bounding box (second argument) is missing " #side "key", "Value of bounding box's (second argument) " #side " key is not long type")

PHP_METHOD(FaceLandmarkDetection, detect)
{
    char *img_path;
    size_t img_path_len;
    zval *bounding_box;
    array2d<rgb_pixel> img;

    // Parse path to image and bounding box. Bounding box is associative array of 4 elements - "top", "bottom", "left" and "right".
    //
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "sa", &img_path, &img_path_len, &bounding_box) == FAILURE){
        zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, "Unable to parse detect arguments.");
        return;
    }

    // Check that bounding box have exactly 4 elements
    HashTable *bounding_box_hash = Z_ARRVAL_P(bounding_box);
    uint32_t bounding_box_num_elements = zend_hash_num_elements(bounding_box_hash);
    if (bounding_box_num_elements < 4) {
        zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, "Bounding box (second argument) needs to have at least 4 elements");
        return;
    }

    // Retrieve all 4 edges of bounding box
    //
    PARSE_BOUNDING_BOX_EDGE(top)
    PARSE_BOUNDING_BOX_EDGE(bottom)
    PARSE_BOUNDING_BOX_EDGE(left)
    PARSE_BOUNDING_BOX_EDGE(right)

    try {
        // Load image and execute shape predictor on it.
        //
        face_landmark_detection *fld = Z_FACE_LANDMARK_DETECTION_P(getThis());
        load_image(img, img_path);
        rectangle rct(left, top, right, bottom);
        full_object_detection shape = fld->sp->operator()(img, rct);

        // Return value is regular array with integer keys.
        // Each key is one part from shape. Value of each part is associative array of keys "x" and "y".
        //
        array_init(return_value);

        zval rect_arr, parts_arr;
        array_init(&rect_arr);
        array_init(&parts_arr);

        for (int i = 0; i < shape.num_parts(); i++) {
            zval part;
            array_init(&part);
            dlib::point p = shape.part(i);
            add_assoc_long(&part, "x", p.x());
            add_assoc_long(&part, "y", p.y());
            add_next_index_zval(&parts_arr, &part);
        }

        const rectangle& r = shape.get_rect();
        add_assoc_long(&rect_arr, "left", r.left());
        add_assoc_long(&rect_arr, "top", r.top());
        add_assoc_long(&rect_arr, "right", r.right());
        add_assoc_long(&rect_arr, "bottom", r.bottom());

        add_assoc_zval(return_value, "rect", &rect_arr);
        add_assoc_zval(return_value, "parts", &parts_arr);
    } catch (exception& e) {
        zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, e.what());
        return;
    }
}