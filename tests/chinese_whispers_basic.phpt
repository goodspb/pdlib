--TEST--
Basic tests for chinese_whispers
--SKIPIF--
<?php if (!extension_loaded("pdlib")) print "skip"; ?>
--FILE--
<?php
var_dump(dlib_chinese_whispers([[0,0]])); // Case with only one node
var_dump(dlib_chinese_whispers([[0,0], [1,1]])); // Case with two separate nodes
var_dump(dlib_chinese_whispers([[0,0], [0,1], [1,0], [1,1]])); // Case with two connected nodes
?>
--EXPECT--
array(1) {
  [0]=>
  int(0)
}
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(1)
}
array(2) {
  [0]=>
  int(0)
  [1]=>
  int(0)
}
