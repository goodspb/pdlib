--TEST--
Just test php extension version
--SKIPIF--
<?php if (!extension_loaded("pdlib")) print "skip"; ?>
--FILE--
<?php
var_dump(phpversion('pdlib'));
?>
--EXPECT--
string(5) "1.0.2"