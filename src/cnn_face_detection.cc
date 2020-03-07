#include "../php_pdlib.h"
#include "cnn_face_detection.h"

#include <zend_exceptions.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>
#include <dlib/dnn.h>
#include <iostream>

using namespace dlib;
using namespace std;

static inline cnn_face_detection *php_cnn_face_detection_from_obj(zend_object *obj) {
	return (cnn_face_detection*)((char*)(obj) - XtOffsetOf(cnn_face_detection, std));
}

#define Z_CNN_FACE_DETECTION_P(zv) php_cnn_face_detection_from_obj(Z_OBJ_P((zv)))

PHP_METHOD(CnnFaceDetection, __construct)
{
	char  *sz_cnn_face_detection_model_path;
	size_t cnn_face_detection_model_path_len;

	cnn_face_detection *cfd = Z_CNN_FACE_DETECTION_P(getThis());

	if (NULL == cfd) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to find obj in CnnFaceDetection::__construct()");
		return;
	}

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s",
				&sz_cnn_face_detection_model_path, &cnn_face_detection_model_path_len) == FAILURE){
		zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, "Unable to parse face_detection_model_path");
		return;
	}

	try {
		string cnn_face_detection_model_path(
				sz_cnn_face_detection_model_path, cnn_face_detection_model_path_len);
		net_type *pnet = new net_type;
		deserialize(cnn_face_detection_model_path) >> *pnet;
		cfd->net = pnet;
	} catch (exception& e) {
		zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, e.what());
		return;
	}
}

PHP_METHOD(CnnFaceDetection, detect)
{
	char *img_path;
	size_t img_path_len;
	long upsample_num = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s|l", &img_path, &img_path_len, &upsample_num) == FAILURE){
		zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, "Unable to parse detect arguments");
		RETURN_FALSE;
	}

	try {
		cnn_face_detection *cfd = Z_CNN_FACE_DETECTION_P(getThis());

		pyramid_down<2> pyr;
		matrix<rgb_pixel> img;
		load_image(img, img_path);

		// Upsampling the image will allow us to detect smaller faces but will cause the
		// program to use more RAM and run longer.
		//
		unsigned int levels = upsample_num;
		while (levels > 0)
		{
			levels--;
			pyramid_up(img, pyr);
		}

		net_type *pnet = cfd->net;
		auto dets = (*pnet)(img);
		int rect_count = 0;
		array_init(return_value);

		// Scale the detection locations back to the original image size
		// if the image was upscaled.
		//
		for (auto&& d: dets) {
			d.rect = pyr.rect_down(d.rect, upsample_num);
			// Create new assoc array with dimensions of found rect and confidence
			//
			zval rect_arr;
			array_init(&rect_arr);
			add_assoc_long(&rect_arr, "left", d.rect.left());
			add_assoc_long(&rect_arr, "top", d.rect.top());
			add_assoc_long(&rect_arr, "right", d.rect.right());
			add_assoc_long(&rect_arr, "bottom", d.rect.bottom());
			add_assoc_double(&rect_arr, "detection_confidence", d.detection_confidence);
			// Add this assoc array to returned array
			//
			add_next_index_zval(return_value, &rect_arr);
		}
	}
	catch (exception& e)
	{
		zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, e.what());
		return;
	}
}

