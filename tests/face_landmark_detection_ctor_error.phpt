--TEST--
Testing FaceLandmarkDetection constructor without arguments
--SKIPIF--
<?php if (!extension_loaded("pdlib")) print "skip"; ?>
--FILE--
<?php
try {
	new FaceLandmarkDetection();
} catch (Exception $e) {
	var_dump($e->getMessage());
}
try {
	new FaceLandmarkDetection("non-existent file");
} catch (Exception $e) {
	var_dump($e->getMessage());
}
?>
--EXPECT--
Warning: FaceLandmarkDetection::__construct() expects exactly 1 parameter, 0 given in /home/branko/pdlib/tests/face_landmark_detection_ctor_error.php on line 3
string(41) "Unable to parse shape_predictor_file_path"
string(45) "Unable to open non-existent file for reading."