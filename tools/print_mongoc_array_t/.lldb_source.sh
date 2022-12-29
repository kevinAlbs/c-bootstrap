
        command script import print_mongoc_arr
        target create cmake-build/mongoc-array-example.out
        b 173
        r
        print_mongoc_arr arr_mystruct 'struct mystruct*'
        