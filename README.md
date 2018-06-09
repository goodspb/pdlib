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
cmake ..
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

## Features
- [x] 1.Face Detection
- [x] 2.Face Landmark Detection
- [ ] 3.Deep Face Recognition
- [ ] 4.Deep Learning Face Detection

