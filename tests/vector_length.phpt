--TEST--
Basic tests for dlib_vector_length
--SKIPIF--
<?php if (!extension_loaded("pdlib")) print "skip"; ?>
--FILE--
<?php
var_dump(dlib_vector_length([0.0, 0.0],       [1.0, 0.0]));
var_dump(dlib_vector_length([0.0, 0.0, -1.0], [0.0, 0.0, 1.0]));
var_dump(dlib_vector_length([0.0, 2.5,  1.0], [0.0, 1.0, 1.0]));
?>
--EXPECT--
float(1)
float(2)
float(1.5)