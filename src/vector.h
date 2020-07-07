#ifndef PHP_DLIB_VECTOR_H
#define PHP_DLIB_VECTOR_H

ZEND_BEGIN_ARG_INFO_EX(dlib_vector_length_arginfo, 0, 0, 2)
    ZEND_ARG_ARRAY_INFO(0, x_arg, 0)
    ZEND_ARG_ARRAY_INFO(0, y_arg, 0)
ZEND_END_ARG_INFO()
PHP_FUNCTION(dlib_vector_length);

#endif //PHP_DLIB_VECTOR_H
