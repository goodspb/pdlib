--TEST--
Basic tests for dlib_vector_length
--SKIPIF--
<?php if (!extension_loaded("pdlib")) print "skip"; ?>
--FILE--
<?php
$vectorAL = [0.0, 0.0];
$vectorAR = [0.0, 1.0];
var_dump(dlib_vector_length($vectorAL, $vectorAR));
$vectorBL = [0.0, 0.0, -1.0];
$vectorBR = [0.0, 0.0, 1.0];
var_dump(dlib_vector_length($vectorBL, $vectorBR));
$vectorCL = [0.0, 2.5, 1.0];
$vectorCR = [0.0, 1.0, 1.0];
var_dump(dlib_vector_length($vectorCL, $vectorCR));
?>
--EXPECT--
float(1)
float(2)
float(1.5)