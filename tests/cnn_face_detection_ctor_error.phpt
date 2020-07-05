--TEST--
Testing CnnFaceDetection constructor without arguments
--SKIPIF--
<?php if (!extension_loaded("pdlib")) print "skip"; ?>
--FILE--
<?php
try {
	new CnnFaceDetection();
} catch (Error $e) {
	var_dump($e->getMessage());
}
?>
--EXPECTF--
string(68) "CnnFaceDetection::__construct() expects exactly 1 parameter, 0 given"

