//
// Created by branko at kokanovic dot org on 2018/7/16.
//

#ifndef PHP_DLIB_CNN_FACE_DETECTION_H
#define PHP_DLIB_CNN_FACE_DETECTION_H

#include <dlib/dnn.h>

using namespace dlib;

template <long num_filters, typename SUBNET> using con5d = con<num_filters,5,5,2,2,SUBNET>;
template <long num_filters, typename SUBNET> using con5  = con<num_filters,5,5,1,1,SUBNET>;

template <typename SUBNET> using downsampler  = relu<affine<con5d<32, relu<affine<con5d<32, relu<affine<con5d<16,SUBNET>>>>>>>>>;
template <typename SUBNET> using rcon5  = relu<affine<con5<45,SUBNET>>>;

using net_type = loss_mmod<con<1,9,9,1,1,rcon5<rcon5<rcon5<downsampler<input_rgb_image_pyramid<pyramid_down<6>>>>>>>>;

typedef struct _cnn_face_detection {
	net_type *net;
	zend_object std;
} cnn_face_detection;

ZEND_BEGIN_ARG_INFO_EX(cnn_face_detection_ctor_arginfo, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, cnn_face_detection_model_path, IS_STRING, 0)
ZEND_END_ARG_INFO()
PHP_METHOD(CnnFaceDetection, __construct);

ZEND_BEGIN_ARG_INFO_EX(cnn_face_detection_detect_arginfo, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, img_path, IS_STRING, 0)
#if PHP_VERSION_ID < 80000
    ZEND_ARG_TYPE_INFO(0, upsample_num, IS_LONG, 0)
#else
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, upsample_num, IS_LONG, 0, "0")
#endif
ZEND_END_ARG_INFO()
PHP_METHOD(CnnFaceDetection, detect);

#endif //PHP_DLIB_CNN_FACE_DETECTION_H
