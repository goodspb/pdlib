//
// Created by goodspb on 2018/5/20.
//

#ifndef PHP_DLIB_FACE_DETECTION_H
#define PHP_DLIB_FACE_DETECTION_H

ZEND_BEGIN_ARG_INFO_EX(dlib_face_detection_arginfo, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, img_path, IS_STRING, 0)
#if PHP_VERSION_ID < 80000
    ZEND_ARG_TYPE_INFO(0, upsample_num, IS_LONG, 0)
#else
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, upsample_num, IS_LONG, 0, "0")
#endif
ZEND_END_ARG_INFO()
PHP_FUNCTION(dlib_face_detection);

#endif //PHP_DLIB_FACE_DETECTION_H
