/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

extern "C" {
	#ifdef HAVE_CONFIG_H
	#include "config.h"
	#endif
	#include "php.h"
	#include "php_ini.h"
	#include "ext/standard/info.h"
}

#include <dlib/simd/simd_check.h>

#include "php_pdlib.h"
#include "src/chinese_whispers.h"
#include "src/face_detection.h"
#include "src/face_recognition.h"
#include "src/cnn_face_detection.h"
#include "src/face_landmark_detection.h"
#include "src/vector.h"

/* If you declare any globals in php_pdlib.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(pdlib)
*/

/* True global resources - no need for thread safety here */
static int le_pdlib;

static zend_class_entry *cnn_face_detection_ce = nullptr;
static zend_object_handlers cnn_face_detection_obj_handlers;

static zend_class_entry *face_landmark_detection_ce = nullptr;
static zend_object_handlers face_landmark_detection_obj_handlers;

static zend_class_entry *face_recognition_ce = nullptr;
static zend_object_handlers face_recognition_obj_handlers;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("pdlib.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_pdlib_globals, pdlib_globals)
    STD_PHP_INI_ENTRY("pdlib.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_pdlib_globals, pdlib_globals)
PHP_INI_END()
*/
/* }}} */

/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_pdlib_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_pdlib_init_globals(zend_pdlib_globals *pdlib_globals)
{
	pdlib_globals->global_value = 0;
	pdlib_globals->global_string = NULL;
}
*/
/* }}} */

const zend_function_entry cnn_face_detection_class_methods[] = {
	PHP_ME(CnnFaceDetection, __construct, cnn_face_detection_ctor_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(CnnFaceDetection, detect, cnn_face_detection_detect_arginfo, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

zend_object* php_cnn_face_detection_new(zend_class_entry *class_type TSRMLS_DC)
{
	cnn_face_detection *cfd = (cnn_face_detection*)ecalloc(1, sizeof(cnn_face_detection));
	zend_object_std_init(&cfd->std, class_type TSRMLS_CC);
	object_properties_init(&cfd->std, class_type);
	cfd->std.handlers = &cnn_face_detection_obj_handlers; //zend_get_std_object_handlers();

	return &cfd->std;
}

static void php_cnn_face_detection_free(zend_object *object)
{
	cnn_face_detection *cfd = (cnn_face_detection*)((char*)object - XtOffsetOf(cnn_face_detection, std));
	delete cfd->net;
	zend_object_std_dtor(object);
}

const zend_function_entry face_landmark_detection_class_methods[] = {
	PHP_ME(FaceLandmarkDetection, __construct, face_landmark_detection_ctor_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(FaceLandmarkDetection, detect, face_landmark_detection_detect_arginfo, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

zend_object* php_face_landmark_detection_new(zend_class_entry *class_type TSRMLS_DC)
{
	face_landmark_detection *fld = (face_landmark_detection*)ecalloc(1, sizeof(face_landmark_detection));
	zend_object_std_init(&fld->std, class_type TSRMLS_CC);
	object_properties_init(&fld->std, class_type);
	fld->std.handlers = &face_landmark_detection_obj_handlers;

	return &fld->std;
}

static void php_face_landmark_detection_free(zend_object *object)
{
	face_landmark_detection *fld = (face_landmark_detection*)((char*)object - XtOffsetOf(face_landmark_detection, std));
	delete fld->sp;
	zend_object_std_dtor(object);
}

const zend_function_entry face_recognition_class_methods[] = {
	PHP_ME(FaceRecognition, __construct, face_recognition_ctor_arginfo, ZEND_ACC_PUBLIC)
	PHP_ME(FaceRecognition, computeDescriptor, face_recognition_compute_descriptor_arginfo, ZEND_ACC_PUBLIC)
	PHP_FE_END
};

zend_object* php_face_recognition_new(zend_class_entry *class_type TSRMLS_DC)
{
	face_recognition *fr = (face_recognition*)ecalloc(1, sizeof(face_recognition));
	zend_object_std_init(&fr->std, class_type TSRMLS_CC);
	object_properties_init(&fr->std, class_type);
	fr->std.handlers = &face_recognition_obj_handlers;

	return &fr->std;
}

static void php_face_recognition_free(zend_object *object)
{
	face_recognition *fr = (face_recognition*)((char*)object - XtOffsetOf(face_recognition, std));
	delete fr->net;
	zend_object_std_dtor(object);
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(pdlib)
{
	zend_class_entry ce;
	// CnnFaceDetection class definition
	//
	INIT_CLASS_ENTRY(ce, "CnnFaceDetection", cnn_face_detection_class_methods);
	cnn_face_detection_ce = zend_register_internal_class(&ce TSRMLS_CC);
	cnn_face_detection_ce->create_object = php_cnn_face_detection_new;
	memcpy(&cnn_face_detection_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	cnn_face_detection_obj_handlers.offset = XtOffsetOf(cnn_face_detection, std);
	cnn_face_detection_obj_handlers.free_obj = php_cnn_face_detection_free;

	// FaceLandmarkDetection class definition
	//
	INIT_CLASS_ENTRY(ce, "FaceLandmarkDetection", face_landmark_detection_class_methods);
	face_landmark_detection_ce = zend_register_internal_class(&ce TSRMLS_CC);
	face_landmark_detection_ce->create_object = php_face_landmark_detection_new;
	memcpy(&face_landmark_detection_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	face_landmark_detection_obj_handlers.offset = XtOffsetOf(face_landmark_detection, std);
	face_landmark_detection_obj_handlers.free_obj = php_face_landmark_detection_free;

	// FaceRecognition class definition
	//
	INIT_CLASS_ENTRY(ce, "FaceRecognition", face_recognition_class_methods);
	face_recognition_ce = zend_register_internal_class(&ce TSRMLS_CC);
	face_recognition_ce->create_object = php_face_recognition_new;
	memcpy(&face_recognition_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	face_recognition_obj_handlers.offset = XtOffsetOf(face_recognition, std);
	face_recognition_obj_handlers.free_obj = php_face_recognition_free;

	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}

/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(pdlib)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(pdlib)
{
#if defined(COMPILE_DL_PDLIB) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(pdlib)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(pdlib)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "PDlib support", "enabled");
	php_info_print_table_header(2, "Pdlib Version", PHP_PDLIB_VERSION);
//	php_info_print_table_header(2, "Dlib Version", DLIB_VERSION);
#ifdef DLIB_USE_CUDA
	php_info_print_table_header(2, "DLIB_USE_CUDA", "true");
#else
	php_info_print_table_header(2, "DLIB_USE_CUDA", "false");
#endif
#ifdef DLIB_USE_BLAS
	php_info_print_table_header(2, "DLIB_USE_BLAS", "true");
#else
	php_info_print_table_header(2, "DLIB_USE_BLAS", "false");
#endif
#ifdef DLIB_USE_LAPACK
	php_info_print_table_header(2, "DLIB_USE_LAPACK", "true");
#else
	php_info_print_table_header(2, "DLIB_USE_LAPACK", "false");
#endif
#ifdef DLIB_HAVE_AVX
	php_info_print_table_header(2, "USE_AVX_INSTRUCTIONS", "true");
#else
	php_info_print_table_header(2, "USE_AVX_INSTRUCTIONS", "false");
#endif
#ifdef DLIB_HAVE_AVX2
	php_info_print_table_header(2, "USE_AVX2_INSTRUCTIONS", "true");
#else
	php_info_print_table_header(2, "USE_AVX2_INSTRUCTIONS", "false");
#endif
#ifdef DLIB_HAVE_NEON
	php_info_print_table_header(2, "USE_NEON_INSTRUCTIONS", "true");
#else
	php_info_print_table_header(2, "USE_NEON_INSTRUCTIONS", "false");
#endif
#ifdef DLIB_HAVE_SSE2
	php_info_print_table_header(2, "USE_SSE2_INSTRUCTIONS", "true");
#else
	php_info_print_table_header(2, "USE_SSE2_INSTRUCTIONS", "false");
#endif
#ifdef DLIB_HAVE_SSE41
	php_info_print_table_header(2, "USE_SSE4_INSTRUCTIONS", "true");
#else
	php_info_print_table_header(2, "USE_SSE4_INSTRUCTIONS", "false");
#endif
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ pdlib_functions[]
 *
 * Every user visible function must have an entry in pdlib_functions[].
 */
const zend_function_entry pdlib_functions[] = {
	PHP_FE(dlib_chinese_whispers, dlib_chinese_whispers_arginfo)
	PHP_FE(dlib_face_detection, dlib_face_detection_arginfo)
	PHP_FE(dlib_face_landmark_detection, dlib_face_landmark_detection_arginfo)
	PHP_FE(dlib_vector_length, dlib_vector_length_arginfo)
	PHP_FE_END	/* Must be the last line in pdlib_functions[] */
};
/* }}} */

/* {{{ pdlib_module_entry
 */
zend_module_entry pdlib_module_entry = {
	STANDARD_MODULE_HEADER,
	PHP_PDLIB_NAME,
	pdlib_functions,
	PHP_MINIT(pdlib),
	PHP_MSHUTDOWN(pdlib),
	PHP_RINIT(pdlib),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(pdlib),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(pdlib),
	PHP_PDLIB_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PDLIB
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(pdlib)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
