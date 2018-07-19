--TEST--
Edge given in edges array for chinese_whispers functions is not having all values to be arrays with 2 elements
--SKIPIF--
<?php if (!extension_loaded("pdlib")) print "skip"; ?>
--FILE--
<?php
try {
    dlib_chinese_whispers([[0,0], [1]]);
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    dlib_chinese_whispers([[0,0], [1,1,1]]);
} catch (Exception $e) {
    var_dump($e->getMessage());
}
?>
--EXPECT--
string(42) "Edges need to contain exactly two elements"
string(42) "Edges need to contain exactly two elements"
