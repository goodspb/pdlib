--TEST--
Testing CnnFaceDetection constructor with model that do not exist
--SKIPIF--
<?php if (!extension_loaded("pdlib")) print "skip"; ?>
--FILE--
<?php
try {
	new CnnFaceDetection("foo");
} catch (Exception $e) {
	var_dump($e->getMessage());
}
?>
--EXPECT--
string(31) "Unable to open foo for reading."
