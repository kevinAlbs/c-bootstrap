if [ "Darwin" == "$(uname)" ]; then
    SHARED_EXT="dylib"
else
    SHARED_EXT="so"
fi

echo "nm of libpoc_boringssl.out.a"
nm ./cmake-build/libpoc_boringssl.out.a
echo "nm of libpoc_openssl.out.$SHARED_EXT"
nm ./cmake-build/libpoc_openssl.out.$SHARED_EXT
echo "nm of libpoc_tensorflow.out.$SHARED_EXT"
nm ./cmake-build/libpoc_tensorflow.out.$SHARED_EXT
echo "nm of libpoc_mongoc.out.$SHARED_EXT"
nm ./cmake-build/libpoc_mongoc.out.$SHARED_EXT
echo "nm of app.out"
nm ./cmake-build/app.out
