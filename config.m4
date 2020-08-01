if test -z "$PHP_DEBUG"; then
    AC_ARG_ENABLE(debug,
    [   --enable-debug          compile with debugging symbols],[
        PHP_DEBUG=$enableval
    ],[ PHP_DEBUG=no
    ])
fi

PHP_ARG_WITH(pdlib, for pdlib support,
dnl Make sure that the comment is aligned:
[  --with-pdlib             Include pdlib support])

if test "$PHP_PDLIB" != "no"; then
  PHP_REQUIRE_CXX()
  PHP_ADD_LIBRARY(stdc++, 1, PDLIB_SHARED_LIBADD)
  PHP_SUBST(PDLIB_SHARED_LIBADD)

  pdlib_src_files="pdlib.cc \
  src/chinese_whispers.cc \
  src/face_detection.cc \
  src/face_landmark_detection.cc \
  src/face_recognition.cc \
  src/cnn_face_detection.cc
  src/vector.cc"

  AC_MSG_CHECKING(for pkg-config)
  AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  if test -x "$PKG_CONFIG"; then
    AC_MSG_RESULT(found)
    AC_MSG_CHECKING(for dlib-1)
    if $PKG_CONFIG --exists dlib-1; then
      if $PKG_CONFIG dlib-1 --atleast-version 19.00; then
        LIBDLIB_CFLAGS=`$PKG_CONFIG dlib-1 --cflags`
        LIBDLIB_LIBDIR=`$PKG_CONFIG dlib-1 --libs`
        LIBDLIB_VERSON=`$PKG_CONFIG dlib-1 --modversion`
        AC_MSG_RESULT(from pkgconfig: dlib version $LIBDLIB_VERSON)
      else
        AC_MSG_ERROR(system dlib is too old: version 19.00 required)
      fi
    else
      AC_MSG_ERROR(dlib-1 not found)
    fi
  else
     AC_MSG_ERROR(pkg-config not found)
  fi

  PHP_EVAL_LIBLINE($LIBDLIB_LIBDIR, PDLIB_SHARED_LIBADD)
  PHP_EVAL_INCLINE($LIBDLIB_CFLAGS)

  dnl using C++11
  PHP_NEW_EXTENSION(pdlib, $pdlib_src_files, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1 -std=c++11, cxx)
fi

AC_CONFIG_COMMANDS_POST([
  echo ""
  echo "Build configuration for PDlib v1.0.2 done correctly."
  echo ""
  echo "  dlib version: $LIBDLIB_VERSON"
  echo ""
  echo "  CXXFLAGS    : $CXXFLAGS"
  echo "  LDFLAGS     : $LDFLAGS"
  echo "  LIBDIR:     : $LIBDLIB_LIBDIR"
  echo "  LIBADD:     : $PDLIB_SHARED_LIBADD"
  echo ""
  echo "Please submit bug reports at:"
  echo "  https://github.com/goodspb/pdlib/issues"
  echo ""
])