--TEST--
Args given to chinese_whispers functions is not correct
--SKIPIF--
<?php if (!extension_loaded("pdlib")) print "skip"; ?>
--FILE--
<?php
try {
    dlib_chinese_whispers("foo");
} catch (Error $e) {
    var_dump($e->getMessage());
}
?>
--EXPECTF--
string(%d) "%s type array, string given"
