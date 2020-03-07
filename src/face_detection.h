//
// Created by goodspb on 2018/5/20.
//

#ifndef PHP_DLIB_FACE_DETECTION_H
#define PHP_DLIB_FACE_DETECTION_H

ZEND_BEGIN_ARG_INFO_EX(dlib_face_detection_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, img_path)
    ZEND_ARG_INFO(0, upsample_num)
ZEND_END_ARG_INFO()
PHP_FUNCTION(dlib_face_detection);

#endif //PHP_DLIB_FACE_DETECTION_H
