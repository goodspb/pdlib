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
--EXPECT--
Warning: dlib_chinese_whispers() expects parameter 1 to be array, string given in /home/branko/pdlib/tests/chinese_whispers_wrong_arg_type_error.php on line 3
string(46) "Unable to parse edges in dlib_chinese_whispers"
