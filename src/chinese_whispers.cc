
#include "../php_pdlib.h"
#include "chinese_whispers.h"

#include <zend_exceptions.h>
#include <dlib/clustering.h>
#include <iostream>

using namespace dlib;
using namespace std;

PHP_FUNCTION(dlib_chinese_whispers)
{
	zval *edges_arg;
	HashTable *edges_arg_hash;

	std::vector<sample_pair> edges;
	std::vector<unsigned long> labels;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "a", &edges_arg) == FAILURE){
        zend_throw_exception_ex(
                zend_ce_exception,
                0,
                "Unable to parse edges in dlib_chinese_whispers");
        return;
	}

	edges_arg_hash = Z_ARRVAL_P(edges_arg);

	try {
		HashPosition pos;
		zval *edge;
		HashTable *edge_hash;

		// Iterate for all given edges, check if they are valid and put them to edges
		//
		for (
				zend_hash_internal_pointer_reset_ex(edges_arg_hash, &pos);
				(edge = zend_hash_get_current_data_ex(edges_arg_hash, &pos)) != nullptr;
				zend_hash_move_forward_ex(edges_arg_hash, &pos)) {
			// Check that each given edge is actually array
			//
			if (Z_TYPE_P(edge) != IS_ARRAY) {
				zend_throw_exception_ex(
						zend_ce_exception,
						0,
						"Each edge provided in array needs to be numeric array of 2 elements");
				return;
			}

			edge_hash = Z_ARRVAL_P(edge);

			// Check that there are two elements in this edge
			//
			if (zend_hash_num_elements(edge_hash) != 2) {
				zend_throw_exception_ex(
						zend_ce_exception,
						0,
						"Edges need to contain exactly two elements");
				return;
			}

			// Check that this is regular array with integer keys
			//
			if (!zend_hash_index_exists(edge_hash, 0) ||
					!zend_hash_index_exists(edge_hash, 1)) {
				zend_throw_exception_ex(
						zend_ce_exception,
						0,
						"Edge should be numeric array with integer keys");
				return;
			}

			zval *elem_i = zend_hash_index_find(edge_hash, 0);
			zval *elem_j = zend_hash_index_find(edge_hash, 1);

			// Check that both elements in array are longs
			if ((Z_TYPE_P(elem_i) != IS_LONG) || (Z_TYPE_P(elem_j) != IS_LONG)) {
				zend_throw_exception_ex(
						zend_ce_exception,
						0,
						"Both elements in each edge must be of long type");
				return;
			}

			// Finally, put extracted elements to edges
			edges.push_back(sample_pair(Z_LVAL_P(elem_i), Z_LVAL_P(elem_j)));
		}

		chinese_whispers(edges, labels);

		// Preparing and generating response array containing labels
		//
		array_init(return_value);
		for (auto label = labels.begin(); label != labels.end(); label++) {
			add_next_index_long(return_value, *label);
		}
	} catch (exception& e)
	{
		zend_throw_exception_ex(zend_ce_exception, 0, "%s", e.what());
		return;
	}
}

