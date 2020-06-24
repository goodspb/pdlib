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

#ifndef PHP_PDLIB_H
#define PHP_PDLIB_H

extern "C" {
    #include "php.h"
    #ifdef ZTS
    #include "TSRM.h"
    #endif
};

extern zend_module_entry pdlib_module_entry;
#define phpext_pdlib_ptr &pdlib_module_entry

#define PHP_PDLIB_NAME "pdlib"
#define PHP_PDLIB_VERSION "1.0.2"

#ifdef PHP_WIN32
#	define PHP_PDLIB_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_PDLIB_API __attribute__ ((visibility("default")))
#else
#	define PHP_PDLIB_API
#endif

/*
  	Declare any global variables you may need between the BEGIN
	and END macros here:

ZEND_BEGIN_MODULE_GLOBALS(pdlib)
	zend_long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(pdlib)
*/

/* Always refer to the globals in your function as PDLIB_G(variable).
   You are encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/
#define PDLIB_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(pdlib, v)

#if defined(ZTS) && defined(COMPILE_DL_PDLIB)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#define PARSE_LONG_FROM_ARRAY(hashtable, key, error_key_missing, error_key_not_long) \
    zval* data##key; \
    /* Tries to find given key in array */ \
    data##key = zend_hash_str_find(hashtable, #key, sizeof(#key)-1); \
    if (data##key == nullptr) { \
        zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, #error_key_missing); \
        return; \
    } \
    \
    /* We also need to check proper type of value in associative array */ \
    if (Z_TYPE_P(data##key) != IS_LONG) { \
        zend_throw_exception_ex(zend_ce_exception, 0 TSRMLS_CC, #error_key_not_long); \
        return; \
    } \
    zend_long key = Z_LVAL_P(data##key); \

#endif	/* PHP_PDLIB_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
