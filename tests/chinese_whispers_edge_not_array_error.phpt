--TEST--
Edge given in edges array is not array for chinese_whispers functions
--SKIPIF--
<?php if (!extension_loaded("pdlib")) print "skip"; ?>
--FILE--
<?php
try {
    dlib_chinese_whispers([[0,0], "foo"]);
} catch (Exception $e) {
    var_dump($e->getMessage());
}

try {
    dlib_chinese_whispers([[0,0], 1]);
} catch (Exception $e) {
    var_dump($e->getMessage());
}
?>
--EXPECT--
string(67) "Each edge provided in array needs to be numeric array of 2 elements"
string(67) "Each edge provided in array needs to be numeric array of 2 elements"
