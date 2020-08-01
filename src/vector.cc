
#include "../php_pdlib.h"

#include "vector.h"

#include <cmath>
#include <zend_exceptions.h>
#include <iostream>

using namespace std;

PHP_FUNCTION(dlib_vector_length)
{
	zval *x_arg, *y_arg;

	if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "aa", &x_arg, &y_arg) == FAILURE) {
		return;
	}

	zval *elem_x, *elem_y;
	double sum = 0.0;
	unsigned int i, len;

	len = zend_hash_num_elements(Z_ARRVAL_P(x_arg));

	if (len != zend_hash_num_elements(Z_ARRVAL_P(y_arg))) {
		zend_throw_exception_ex(
			zend_ce_exception,
			0,
			"The arrays have different sizes");
		return;
	}

	for (i = 0 ; i < len ; i++) {
		elem_x = zend_hash_index_find(Z_ARRVAL_P(x_arg), i);
		elem_y = zend_hash_index_find(Z_ARRVAL_P(y_arg), i);

		sum += (Z_DVAL_P(elem_x) - Z_DVAL_P(elem_y))*(Z_DVAL_P(elem_x) - Z_DVAL_P(elem_y));
	}

	RETURN_DOUBLE(sqrt(sum));
}

