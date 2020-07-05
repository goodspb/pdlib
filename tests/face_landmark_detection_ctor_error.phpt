--TEST--
Testing FaceLandmarkDetection constructor without arguments
--SKIPIF--
<?php if (!extension_loaded("pdlib")) print "skip"; ?>
--FILE--
<?php
try {
	new FaceLandmarkDetection();
} catch (Error $e) {
	var_dump($e->getMessage());
}
try {
	new FaceLandmarkDetection("non-existent file");
} catch (Exception $e) {
	var_dump($e->getMessage());
}
?>
--EXPECTF--
string(73) "FaceLandmarkDetection::__construct() expects exactly 1 parameter, 0 given"
string(45) "Unable to open non-existent file for reading."
