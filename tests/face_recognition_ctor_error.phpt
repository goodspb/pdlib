--TEST--
Testing FaceRecognition constructor without arguments
--SKIPIF--
<?php if (!extension_loaded("pdlib")) print "skip"; ?>
--FILE--
<?php
try {
	new FaceRecognition();
} catch (Exception $e) {
	var_dump($e->getMessage());
}
?>
--EXPECT--
Warning: FaceRecognition::__construct() expects exactly 1 parameter, 0 given in /home/branko/pdlib/tests/face_recognition_ctor_error.php on line 3
string(43) "Unable to parse face_recognition_model_path"