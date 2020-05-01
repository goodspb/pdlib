--TEST--
Frontal face detection.
--SKIPIF--
<?php if (!extension_loaded("pdlib") print "skip"; ?>
--FILE--
<?php
printf("Simple detection\n");
$detected_faces = dlib_face_detection(__DIR__ . "/lenna.jpg");
printf("Faces found = %d\n", count($detected_faces));
foreach($detected_faces as $index => $detected_face) {
  printf("Face[%d] in bounding box (left=%d, top=%d, right=%d, bottom=%d)\n", $index,
    $detected_face["left"], $detected_face["top"], $detected_face["right"], $detected_face["bottom"]);
}
printf("Detection with upsampling\n");
$detected_faces = dlib_face_detection(__DIR__ . "/lenna.jpg", 1);
printf("Faces found = %d\n", count($detected_faces));
foreach($detected_faces as $index => $detected_face) {
  printf("Face[%d] in bounding box (left=%d, top=%d, right=%d, bottom=%d)\n", $index,
    $detected_face["left"], $detected_face["top"], $detected_face["right"], $detected_face["bottom"]);
}
?>
--EXPECT--
Simple detection
Faces found = 1
Face[0] in bounding box (left=214, top=194, right=393, bottom=373)
Detection with upsampling
Faces found = 1
Face[0] in bounding box (left=201, top=180, right=386, bottom=366)