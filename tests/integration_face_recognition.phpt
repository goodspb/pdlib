--TEST--
Full test for face recognition - download models, detect faces, landmark detection and face recognition.
--SKIPIF--
<?php
if (!extension_loaded("pdlib"))  die('skip pdlib extension missing');
if (!function_exists("bzopen"))  die('skip bz2 extension missing');
if (getenv("SKIP_ONLINE_TESTS")) die('skip online test');
?>
--FILE--
<?php
$models = array(
  "detection" => array("uri"=>"http://dlib.net/files/mmod_human_face_detector.dat.bz2"),
  "prediction" => array("uri"=>"http://dlib.net/files/shape_predictor_5_face_landmarks.dat.bz2"),
  "recognition" => array("uri"=>"http://dlib.net/files/dlib_face_recognition_resnet_model_v1.dat.bz2")
);

// Check if there are models in local tmp. Download them if not (lazy caching).
//
foreach ($models as $modelName => $modelBag) {
  printf("Processing %s model\n", $modelName);
  $bz2_filename = array_values(array_slice(explode("/", $modelBag["uri"]), -1))[0];
  $temp_bz2_file = sys_get_temp_dir() . "/" . $bz2_filename;
  $dat_filename = array_values(array_slice(explode(".", $bz2_filename), 0))[0] . ".dat";
  $temp_dat_file = sys_get_temp_dir() . "/" . $dat_filename;
  $models[$modelName]["local_path"] = $temp_dat_file;

  if (file_exists($temp_dat_file)) {
    continue;
  }
  file_put_contents($temp_bz2_file, fopen($modelBag["uri"], 'r'));
  $bz = bzopen($temp_bz2_file, "r");
  $decompressed_file = "";
  while (!feof($bz)) {
    $decompressed_file .= bzread($bz, 4096);
  }
  bzclose($bz);

  file_put_contents($temp_dat_file, $decompressed_file);
}

printf("Detection\n");
$fd = new CnnFaceDetection($models["detection"]["local_path"]);
$detected_faces = $fd->detect(__DIR__ . "/lenna.jpg");
printf("Faces found = %d\n", count($detected_faces));
foreach($detected_faces as $index => $detected_face) {
  printf("Face[%d] in bounding box (left=%d, top=%d, right=%d, bottom=%d)\n", $index,
    $detected_face["left"], $detected_face["top"], $detected_face["right"], $detected_face["bottom"]);
  $fld = new FaceLandmarkDetection($models["prediction"]["local_path"]);
  $landmarks = $fld->detect(__DIR__ . "/lenna.jpg", $detected_face);
  printf("Since we used model with 5 shape predictions, we found %d landmark parts\n", count($landmarks["parts"]));
  $fr = new FaceRecognition($models["recognition"]["local_path"]);
  $descriptor = $fr->computeDescriptor(__DIR__ . "/lenna.jpg", $landmarks);
  printf("Descriptor is vector of %d dimensions\n", count($descriptor));
}
?>
--EXPECT--
Processing detection model
Processing prediction model
Processing recognition model
Detection
Faces found = 1
Face[0] in bounding box (left=187, top=186, right=357, bottom=355)
Since we used model with 5 shape predictions, we found 5 landmark parts
Descriptor is vector of 128 dimensions
