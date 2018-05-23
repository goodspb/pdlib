//
// Created by goodspb on 2018/5/23.
//

#ifndef PDLIB_FACE_LANDMARK_DETECTION_H
#define PDLIB_FACE_LANDMARK_DETECTION_H

ZEND_BEGIN_ARG_INFO_EX(dlib_face_landmark_detection_arginfo, 0, 0, 1)
ZEND_ARG_INFO(0, shape_predictor_file_path)
ZEND_ARG_INFO(0, img_path)
ZEND_END_ARG_INFO()
PHP_FUNCTION(dlib_face_landmark_detection);

#endif //PDLIB_FACE_LANDMARK_DETECTION_H
