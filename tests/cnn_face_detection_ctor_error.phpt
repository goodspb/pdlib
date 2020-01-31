--TEST--
Testing CnnFaceDetection constructor without arguments
--SKIPIF--
<?php if (!extension_loaded("pdlib")) print "skip"; ?>
--FILE--
<?php
try {
	new CnnFaceDetection();
} catch (Exception $e) {
	var_dump($e->getMessage());
}
?>
--EXPECTF--
Warning: CnnFaceDetection::__construct() expects exactly 1 parameter, 0 given in %s on line 3
string(41) "Unable to parse face_detection_model_path"
