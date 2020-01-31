--TEST--
Args given to chinese_whispers functions is not correct
--SKIPIF--
<?php if (!extension_loaded("pdlib")) print "skip"; ?>
--FILE--
<?php
try {
    dlib_chinese_whispers("foo");
} catch (Exception $e) {
    var_dump($e->getMessage());
}
?>
--EXPECTF--
Warning: dlib_chinese_whispers() expects parameter 1 to be array, string given in %s on line 3
string(46) "Unable to parse edges in dlib_chinese_whispers"
