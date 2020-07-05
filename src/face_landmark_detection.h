//
// Created by goodspb on 2018/5/23.
//

#ifndef PDLIB_FACE_LANDMARK_DETECTION_H
#define PDLIB_FACE_LANDMARK_DETECTION_H

#include <dlib/dnn.h>

using namespace dlib;

ZEND_BEGIN_ARG_INFO_EX(dlib_face_landmark_detection_arginfo, 0, 0, 2)
    ZEND_ARG_TYPE_INFO(0, shape_predictor_file_path, IS_STRING, 0)
    ZEND_ARG_TYPE_INFO(0, img_path, IS_STRING, 0)
ZEND_END_ARG_INFO()
PHP_FUNCTION(dlib_face_landmark_detection);

typedef struct _face_landmark_detection {
    shape_predictor *sp;
    zend_object std;
} face_landmark_detection;

ZEND_BEGIN_ARG_INFO_EX(face_landmark_detection_ctor_arginfo, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, shape_predictor_file_path, IS_STRING, 0)
ZEND_END_ARG_INFO()
PHP_METHOD(FaceLandmarkDetection, __construct);

ZEND_BEGIN_ARG_INFO_EX(face_landmark_detection_detect_arginfo, 0, 0, 2)
    ZEND_ARG_TYPE_INFO(0, img_path, IS_STRING, 0)
    ZEND_ARG_ARRAY_INFO(0, bounding_box, 0)
ZEND_END_ARG_INFO()
PHP_METHOD(FaceLandmarkDetection, detect);

#endif //PDLIB_FACE_LANDMARK_DETECTION_H
