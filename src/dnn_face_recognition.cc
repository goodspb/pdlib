
#include "../php_pdlib.h"
#include "dnn_face_recognition.h"

zend_class_entry *dnn_face_recognition_ce;

#include <dlib/dnn.h>
#include <dlib/gui_widgets.h>
#include <dlib/clustering.h>
#include <dlib/string.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>

using namespace dlib;
using namespace std;

std::vector<matrix<rgb_pixel>> jitter_image(
		const matrix<rgb_pixel>& img
);

PHP_METHOD(FaceRecognition, __construct)
{

}

ZEND_BEGIN_ARG_INFO_EX(set_shape_predictor_face_landmarks_arginfo, 0, 0, 1)
				ZEND_ARG_INFO(0, shapePredictorFaceLandmarks)
ZEND_END_ARG_INFO()

PHP_METHOD(FaceRecognition, setShapePredictorFaceLandmarks)
{
	zval *self = getThis();
	zend_string *shape_predictor_face_landmarks_path;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "S", &shape_predictor_face_landmarks_path) == FAILURE){
		RETURN_FALSE;
	}

	zend_update_property_str(Z_OBJCE_P(self), self, "shapePredictorFaceLandmarks", sizeof("shapePredictorFaceLandmarks") - 1, shape_predictor_face_landmarks_path);
	RETURN_TRUE;
}

ZEND_BEGIN_ARG_INFO_EX(set_face_recognition_resnet_model_arginfo, 0, 0, 1)
				ZEND_ARG_INFO(0, faceRecognitionResnetModel)
ZEND_END_ARG_INFO()

PHP_METHOD(FaceRecognition, setFaceRecognitionResnetModel)
{
	zval *self = getThis();
	zend_string *face_recognition_resnet_model_path;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "S", &face_recognition_resnet_model_path) == FAILURE){
		RETURN_FALSE;
	}

	zend_update_property_str(Z_OBJCE_P(self), self, "faceRecognitionResnetModel", sizeof("faceRecognitionResnetModel") - 1, face_recognition_resnet_model_path);
	RETURN_TRUE;
}

ZEND_BEGIN_ARG_INFO_EX(face_recognition_run_arginfo, 0, 0, 1)
				ZEND_ARG_INFO(0, imgPath)
ZEND_END_ARG_INFO()

PHP_METHOD(FaceRecognition, run)
{
	zval *self = getThis();
	char *img_path;
	size_t img_path_len;

	if(zend_parse_parameters(ZEND_NUM_ARGS(), "s", &img_path, &img_path_len) == FAILURE){
		RETURN_FALSE;
	}

	zval *shape_predictor_face_landmarks_z = zend_read_property(Z_OBJCE_P(self), self, "shapePredictorFaceLandmarks", sizeof("shapePredictorFaceLandmarks") - 1, 0, NULL);
	zval *face_recognition_resnet_model_z = zend_read_property(Z_OBJCE_P(self), self, "faceRecognitionResnetModel", sizeof("faceRecognitionResnetModel") - 1, 0, NULL);

	zend_string *shape_predictor_face_landmarks_s = zval_get_string(shape_predictor_face_landmarks_z);
	zend_string *face_recognition_resnet_model_s = zval_get_string(face_recognition_resnet_model_z);

	try {
		frontal_face_detector detector = get_frontal_face_detector();
		shape_predictor sp;
		deserialize(ZSTR_VAL(shape_predictor_face_landmarks_s)) >> sp;
		anet_type net;
		deserialize(ZSTR_VAL(face_recognition_resnet_model_s)) >> net;

		matrix<rgb_pixel> img;
		load_image(img, img_path);
		std::vector<matrix<rgb_pixel>> faces;

		for (auto face : detector(img)) {
			auto shape = sp(img, face);
			matrix<rgb_pixel> face_chip;
			extract_image_chip(img, get_face_chip_details(shape, 150, 0.25), face_chip);
			faces.push_back(move(face_chip));
		}

		if (faces.size() == 0) {
			RETURN_FALSE;
		}

		std::vector<matrix<float, 0, 1>> face_descriptors = net(faces);

		std::vector<sample_pair> edges;
		for (size_t i = 0; i < face_descriptors.size(); ++i) {
			for (size_t j = i; j < face_descriptors.size(); ++j) {
				if (length(face_descriptors[i] - face_descriptors[j]) < 0.6)
					edges.push_back(sample_pair(i, j));
			}
		}

		// face number
		zend_update_property_long(Z_OBJCE_P(self), self, "faceNumber", sizeof("faceNumber") - 1, face_descriptors.size());

		std::vector<unsigned long> labels;
		unsigned long num_clusters = chinese_whispers(edges, labels);

		// face clusters number
		zend_update_property_long(Z_OBJCE_P(self), self, "faceClustersNumber", sizeof("faceClustersNumber") - 1, num_clusters);

		// set property chineseWhispersLabels
		zval chineseWhispersLabels;
		array_init(&chineseWhispersLabels);
		for (auto label = labels.begin(); label != labels.end(); label++) {
			add_next_index_long(&chineseWhispersLabels, *label);
		}
		zend_update_property(Z_OBJCE_P(self), self, "chineseWhispersLabels", sizeof("chineseWhispersLabels") - 1, &chineseWhispersLabels);

		array_init(return_value);
		for(auto face_descriptor: face_descriptors) {
			add_next_index_double(return_value, face_descriptor);
		}
		return;
	}
	catch (std::exception& e)
	{
		zend_update_property_string(Z_OBJCE_P(self), self, "lastError", sizeof("lastError") - 1, e.what());
	}
	RETURN_FALSE;
}


const zend_function_entry dnn_face_recognition_method[] = {
		PHP_ME(FaceRecognition, __construct, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_CTOR)
		PHP_ME(FaceRecognition, setShapePredictorFaceLandmarks, set_shape_predictor_face_landmarks_arginfo, ZEND_ACC_PUBLIC)
		PHP_ME(FaceRecognition, setFaceRecognitionResnetModel, set_face_recognition_resnet_model_arginfo, ZEND_ACC_PUBLIC)
		PHP_ME(FaceRecognition, run, face_recognition_run_arginfo, ZEND_ACC_PUBLIC)
		PHP_FE_END
};

void dnn_face_recognition_init(){
	zend_class_entry ce;
	//init class with namespace
	INIT_NS_CLASS_ENTRY(ce, DLIB_DNN_NS, "FaceRecognition", dnn_face_recognition_method);
	dnn_face_recognition_ce = zend_register_internal_class(&ce);

	//declare class property
	zend_declare_property_string(dnn_face_recognition_ce, "shapePredictorFaceLandmarks", sizeof("shapePredictorFaceLandmarks") - 1, "", ZEND_ACC_PRIVATE);
	zend_declare_property_string(dnn_face_recognition_ce, "faceRecognitionResnetModel", sizeof("faceRecognitionResnetModel") - 1, "", ZEND_ACC_PRIVATE);
	zend_declare_property_long(dnn_face_recognition_ce, "faceNumber", sizeof("faceNumber") - 1, 0, ZEND_ACC_PUBLIC);
	zend_declare_property_long(dnn_face_recognition_ce, "faceClustersNumber", sizeof("faceClustersNumber") - 1, 0, ZEND_ACC_PUBLIC);
	zend_declare_property_null(dnn_face_recognition_ce, "chineseWhispersLabels", sizeof("chineseWhispersLabels") - 1, ZEND_ACC_PUBLIC);
	zend_declare_property_string(dnn_face_recognition_ce, "lastError", sizeof("lastError") - 1, "", ZEND_ACC_PUBLIC);
}


std::vector<matrix<rgb_pixel>> jitter_image(
		const matrix<rgb_pixel>& img
)
{
	thread_local dlib::rand rnd;

	std::vector<matrix<rgb_pixel>> crops;
	for (int i = 0; i < 100; ++i)
		crops.push_back(jitter_image(img,rnd));

	return crops;
}