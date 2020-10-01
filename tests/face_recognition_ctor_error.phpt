--TEST--
Testing FaceRecognition constructor without arguments
--SKIPIF--
<?php if (!extension_loaded("pdlib")) print "skip"; ?>
--FILE--
<?php
try {
	new FaceRecognition();
} catch (Error $e) {
	var_dump($e->getMessage());
}
?>
--EXPECTF--
string(%d) "FaceRecognition::__construct() expects exactly 1 %s, 0 given"
