//
// Created by branko at kokanovic dot org on 2018/7/19.
//

#ifndef PHP_DLIB_CHINESE_WHISPERS_H
#define PHP_DLIB_CHINESE_WHISPERS_H

ZEND_BEGIN_ARG_INFO_EX(dlib_chinese_whispers_arginfo, 0, 0, 1)
    ZEND_ARG_INFO(0, edges)
ZEND_END_ARG_INFO()
PHP_FUNCTION(dlib_chinese_whispers);

#endif //PHP_DLIB_CHINESE_WHISPERS_H
