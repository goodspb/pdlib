--TEST--
Edge given in edges array for chinese_whispers functions is associative array
--SKIPIF--
<?php if (!extension_loaded("pdlib")) print "skip"; ?>
--FILE--
<?php
try {
    dlib_chinese_whispers([[0,0], ["foo"=>0, "bar"=>1]]);
} catch (Exception $e) {
    var_dump($e->getMessage());
}
--EXPECT--
string(46) "Edge should be numeric array with integer keys"
