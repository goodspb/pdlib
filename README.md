# PDlib - A PHP extension for Dlib
A PHP extension 

## Requirements
- Dlib 19.13+
- PHP 7.0+
- C++ 11

## Dependence

### Dlib

Install Dlib as share library

```bash
git clone git@github.com:davisking/dlib.git
cd dlib/dlib
mkdir build
cd build
cmake -DBUILD_SHARED_LIBS=ON ..
make
sudo make install
```

## Installation

```bash
git clone https://github.com/goodspb/pdlib.git
cd pdlib
phpize
./configure --enable-debug
make
sudo make install
```

## Configure

```
vim youpath/php.ini
```

Write the below content into `php.ini`

```
[pdlib]
extension="pdlib.so"
```

## Usage


#### face detection
```php
<?php

// face detection
$faceCount = dlib_face_detection("~/a.jpg");
// how mary face in the picture.
var_dump($faceCount);

```

#### face landmark detection

```php
<?php

// face landmark detection
$landmarks = dlib_face_landmark_detection("~/a.jpg");
var_dump($landmarks);

```

#### chinese whisers

Provides raw access to dlib's `chinese_whispers` function.
Client need to build and provide edges. Edges are provided
as numeric array. Each element of this array should also be
numeric array with 2 elements of long type.

Returned value is also numeric array, containing obtained labels.

```php
<?php
// This example will cluster nodes 0 and 1, but would leave 2 out.
// $labels will look like [0,0,1].
$edges = [[0,0], [0,1], [1,1], [2,2]];
$labels = dlib_chinese_whispers($edges);

```

## Features
- [x] 1.Face Detection
- [x] 2.Face Landmark Detection
- [ ] 3.Deep Face Recognition
- [x] 4.Deep Learning Face Detection
- [x] 5. Raw chinese_whispers

