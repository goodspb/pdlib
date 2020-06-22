//
// Created by branko at kokanovic dot org on 2018/8/26.
//

#ifndef PHP_DLIB_FACE_RECOGNITION_H
#define PHP_DLIB_FACE_RECOGNITION_H

#include <dlib/dnn.h>

using namespace dlib;

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual = add_prev1<block<N,BN,1,tag1<SUBNET>>>;

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_down = add_prev2<avg_pool<2,2,2,2,skip1<tag2<block<N,BN,2,tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET>
using block  = BN<con<N,3,3,1,1,relu<BN<con<N,3,3,stride,stride,SUBNET>>>>>;

template <int N, typename SUBNET> using ares      = relu<residual<block,N,affine,SUBNET>>;
template <int N, typename SUBNET> using ares_down = relu<residual_down<block,N,affine,SUBNET>>;

template <typename SUBNET> using alevel0 = ares_down<256,SUBNET>;
template <typename SUBNET> using alevel1 = ares<256,ares<256,ares_down<256,SUBNET>>>;
template <typename SUBNET> using alevel2 = ares<128,ares<128,ares_down<128,SUBNET>>>;
template <typename SUBNET> using alevel3 = ares<64,ares<64,ares<64,ares_down<64,SUBNET>>>>;
template <typename SUBNET> using alevel4 = ares<32,ares<32,ares<32,SUBNET>>>;

using anet_type = loss_metric<fc_no_bias<128,avg_pool_everything<
                            alevel0<
                            alevel1<
                            alevel2<
                            alevel3<
                            alevel4<
                            max_pool<3,3,2,2,relu<affine<con<32,7,7,2,2,
                            input_rgb_image_sized<150>
                            >>>>>>>>>>>>;

typedef struct _face_recognition {
    anet_type *net;
	zend_object std;
    dlib::rand rnd;
} face_recognition;

ZEND_BEGIN_ARG_INFO_EX(face_recognition_ctor_arginfo, 0, 0, 1)
    ZEND_ARG_TYPE_INFO(0, face_recognition_model_path, IS_STRING, 0)
ZEND_END_ARG_INFO()
PHP_METHOD(FaceRecognition, __construct);

ZEND_BEGIN_ARG_INFO_EX(face_recognition_compute_descriptor_arginfo, 0, 0, 2)
    ZEND_ARG_TYPE_INFO(0, img_path, IS_STRING, 0)
    ZEND_ARG_ARRAY_INFO(0, landmarks, 0)
#if PHP_VERSION_ID < 80000
    ZEND_ARG_TYPE_INFO(0, num_jitters, IS_LONG, 0)
#else
    ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, num_jitters, IS_LONG, 0, "1")
#endif
ZEND_END_ARG_INFO()
PHP_METHOD(FaceRecognition, computeDescriptor);

#endif //PHP_DLIB_FACE_RECOGNITION_H
