--TEST--
Edge elements given in edges array for chinese_whispers functions are not of long type
--SKIPIF--
<?php if (!extension_loaded("pdlib")) print "skip"; ?>
--FILE--
<?php
try {
    dlib_chinese_whispers([[0,0], [1, "foo"]]);
} catch (Exception $e) {
    var_dump($e->getMessage());
}
try {
    dlib_chinese_whispers([[0,0], [1, 1.1]]);
} catch (Exception $e) {
    var_dump($e->getMessage());
}
--EXPECT--
string(47) "Both elements in each edge must be of long type"
string(47) "Both elements in each edge must be of long type"
