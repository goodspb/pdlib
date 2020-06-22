#include "../php_pdlib.h"
#include "face_recognition.h"

#include <zend_exceptions.h>

#include <dlib/image_io.h>

using namespace std;
using namespace dlib;

static inline face_recognition *php_face_recognition_from_obj(zend_object *obj) {
    return (face_recognition*)((char*)(obj) - XtOffsetOf(face_recognition, std));
}

#define Z_FACE_RECOGNITION_P(zv) php_face_recognition_from_obj(Z_OBJ_P((zv)))

PHP_METHOD(FaceRecognition, __construct)
{
    char  *sz_face_recognition_model_path;
    size_t face_recognition_model_path_len;

    face_recognition *fr = Z_FACE_RECOGNITION_P(getThis());

    if (NULL == fr) {
        php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to find obj in FaceRecognition::__construct()");
        return;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
                &sz_face_recognition_model_path, &face_recognition_model_path_len) == FAILURE){
        zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, "Unable to parse face_recognition_model_path");
        return;
    }

    try {
        string face_recognition_model_path(sz_face_recognition_model_path, face_recognition_model_path_len);
        fr->net = new anet_type;
        deserialize(face_recognition_model_path) >> *(fr->net);
    } catch (exception& e) {
        zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, "%s", e.what());
        return;
    }
}

std::vector<matrix<rgb_pixel>> pdlib_jitter_image(
        const matrix<rgb_pixel>& img,
        const int num_jitters,
        dlib::rand& rnd) {
    std::vector<matrix<rgb_pixel>> crops;
    for (int i = 0; i < num_jitters; ++i)
        crops.push_back(dlib::jitter_image(img,rnd));
    return crops;
}


// Helper macro to automatically have parsing of "top"/"bottom"/"left"/"right"
//
#define PARSE_BOUNDING_BOX_EDGE(side) \
    PARSE_LONG_FROM_ARRAY(rect_hash, side, \
        "Shape's rect array is missing " #side "key", "Shape's rect array's " #side " key is not long type")

// Helper macro to parse "x"/"y"
//
#define PARSE_POINT(coord) \
    PARSE_LONG_FROM_ARRAY(part_hash, coord, \
        #coord " coordinate key is missing in parts array", #coord " coordinate key is not of long type")


PHP_METHOD(FaceRecognition, computeDescriptor)
{
    char *img_path;
    size_t img_path_len;
    zval *shape;
    long num_jitters = 1;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "sa|l", &img_path, &img_path_len, &shape, &num_jitters) == FAILURE){
        zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, "Unable to parse computeDescriptor arguments");
        return;
    }

    HashTable *shape_hash = Z_ARRVAL_P(shape);
    uint32_t shape_hash_num_elements = zend_hash_num_elements(shape_hash);
    if (shape_hash_num_elements != 2) {
        zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, "Shape (second argument) needs to have exactly 2 elements - keys \"rect\" and \"parts\"");
        return;
    }

    zval *rect_zval = zend_hash_str_find(shape_hash, "rect", sizeof("rect")-1);
    if (rect_zval == nullptr) {
        zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, "Shape (second argument) array needs to have \"rect\" key"); \
        return;
    }
    if (Z_TYPE_P(rect_zval) != IS_ARRAY) {
        zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, "Value of shape's key \"rect\" must be array");
        return;
    }
    HashTable *rect_hash = Z_ARRVAL_P(rect_zval);
    PARSE_BOUNDING_BOX_EDGE(top)
    PARSE_BOUNDING_BOX_EDGE(bottom)
    PARSE_BOUNDING_BOX_EDGE(left)
    PARSE_BOUNDING_BOX_EDGE(right)
    rectangle rect(left, top, right, bottom);


    zval *parts_zval = zend_hash_str_find(shape_hash, "parts", sizeof("parts")-1);
    if (parts_zval == nullptr) {
        zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, "Shape (second argument) array needs to have \"parts\" key"); \
        return;
    }
    if (Z_TYPE_P(parts_zval) != IS_ARRAY) {
        zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, "Value of shape's key \"parts\" must be array");
        return;
    }
    HashTable *parts_hash = Z_ARRVAL_P(parts_zval);
    HashPosition parts_pos;
    uint32_t parts_count = zend_hash_num_elements(parts_hash);
    point parts_points[parts_count];

    if ((parts_count != 5) && (parts_count != 68)) {
        zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC,
            "The full_object_detection must use the iBUG 300W 68 point face landmark style or dlib's 5 point style");
        return;
    }

    for (zend_hash_internal_pointer_reset_ex(parts_hash, &parts_pos);
        zend_hash_has_more_elements_ex(parts_hash, &parts_pos) == SUCCESS;
        zend_hash_move_forward_ex(parts_hash, &parts_pos)
    ) {
        zend_string* str_index = {0};
        zend_ulong num_index;
        zval *part_zval = zend_hash_get_current_data_ex(parts_hash, &parts_pos);
        switch (zend_hash_get_current_key_ex(parts_hash, &str_index, &num_index, &parts_pos)) {
            case HASH_KEY_IS_LONG:
                if (Z_TYPE_P(part_zval) == IS_ARRAY)
                {
                    HashTable *part_hash = Z_ARRVAL_P(part_zval);
                    PARSE_POINT(x)
                    PARSE_POINT(y)
                    if (num_index > parts_count) {
                        zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, "Internal error, bad parsing of parts array");
                        return;
                    }
                    parts_points[num_index] = point(x, y);
                } else {
                    zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, "Values from parts array must be arrays with \"x\" and \"y\" keys");
                    return;
                }
                break;
            case HASH_KEY_IS_STRING:
                zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, "Parts array must be indexed and it contains string keys");
                return;
                break;
        }
    }

    std::vector<point> parts;
    for (unsigned int i = 0; i < parts_count; i++) {
        parts.push_back(parts_points[i]);
    }

    try {
        face_recognition *fr = Z_FACE_RECOGNITION_P(getThis());
        full_object_detection fod(rect, parts);
        matrix<rgb_pixel> img;
        load_image(img, img_path);

        std::vector<chip_details> dets;
        dets.push_back(get_face_chip_details(fod, 150, 0.25));
        dlib::array<matrix<rgb_pixel>> face_chips;
        extract_image_chips(img, dets, face_chips);

        array_init(return_value);
        matrix<float,0,1> face_descriptor;
        if (num_jitters <= 1) {
            std::vector<matrix<float,0,1>> face_descriptors = fr->net->operator()(face_chips, 16);
            face_descriptor = face_descriptors[0];
        } else {
            matrix<rgb_pixel>& face_chip = face_chips[0];
            face_descriptor = mean(mat(fr->net->operator()(pdlib_jitter_image(face_chip, num_jitters, fr->rnd), 16)));
        }

        for (auto& d : face_descriptor) {
            add_next_index_double(return_value, d);
        }
    } catch (exception& e) {
        zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, "%s", e.what());
        return;
    }
}
