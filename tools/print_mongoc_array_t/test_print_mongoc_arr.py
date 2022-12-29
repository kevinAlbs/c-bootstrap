import unittest
import os
import subprocess

"""
Tests require the mongoc-array-example exectuable. To compile:
$ cmake -Bcmake-build -S. -DCMAKE_BUILD_TYPE=Debug
$ cmake --build cmake-build
"""

class TestArrayPrinter (unittest.TestCase):
    def _make_source (self, contents):
        with open(".lldb_source.sh", "w") as file:
            file.write(contents)

    def _check_golden_file (self, filename, got):
        if "REGENERATE_GOLDEN_FILES" in os.environ and os.environ["REGENERATE_GOLDEN_FILES"] == "ON":
            with open ("golden/{}".format(filename), "w") as file:
                file.write(got)
                print ("Regenerated {}".format(filename))
            return
        
        expect = ""
        with open ("golden/{}".format(filename), "r") as file:
            expect = file.read()
        self.assertEqual(expect, got, msg="Failed to match expected golden file: {}".format(filename))

    def test_int32 (self):
        self._make_source ("""
        command script import print_mongoc_arr
        target create cmake-build/mongoc-array-example.out
        b 173
        r
        print_mongoc_arr arr_i32 int32_t*
        """)
        got : subprocess.CompletedProcess = subprocess.run([
            "lldb",
            "--source", ".lldb_source.sh",
            "--source-quietly", # Do not echo commands.
            "--batch" # Quit after running sourced commands.
        ], capture_output=True)
        self.assertEqual(got.returncode, 0, "failed with stderr: {}".format(got.stderr.decode("utf8")))
        self._check_golden_file ("test_int32", got.stdout.decode("utf8"))

    def test_int32alias (self):
        self._make_source ("""
        command script import print_mongoc_arr
        target create cmake-build/mongoc-array-example.out
        b 173
        r
        print_mongoc_arr arr_i32alias i32alias*
        """)
        got : subprocess.CompletedProcess = subprocess.run([
            "lldb",
            "--source", ".lldb_source.sh",
            "--source-quietly", # Do not echo commands.
            "--batch" # Quit after running sourced commands.
        ], capture_output=True)
        self.assertEqual(got.returncode, 0, "failed with stderr: {}".format(got.stderr.decode("utf8")))
        self._check_golden_file ("test_int32alias", got.stdout.decode("utf8"))

    def test_struct (self):
        self._make_source ("""
        command script import print_mongoc_arr
        target create cmake-build/mongoc-array-example.out
        b 173
        r
        print_mongoc_arr arr_mystruct 'struct mystruct*'
        """)
        got : subprocess.CompletedProcess = subprocess.run([
            "lldb",
            "--source", ".lldb_source.sh",
            "--source-quietly", # Do not echo commands.
            "--batch" # Quit after running sourced commands.
        ], capture_output=True)
        self.assertEqual(got.returncode, 0, "failed with stderr: {}".format(got.stderr.decode("utf8")))
        self._check_golden_file ("test_struct", got.stdout.decode("utf8"))

    def test_int32ptr (self):
        self._make_source ("""
        command script import print_mongoc_arr
        target create cmake-build/mongoc-array-example.out
        b 173
        r
        print_mongoc_arr arr_i32ptr 'int32_t **'
        """)
        got : subprocess.CompletedProcess = subprocess.run([
            "lldb",
            "--source", ".lldb_source.sh",
            "--source-quietly", # Do not echo commands.
            "--batch" # Quit after running sourced commands.
        ], capture_output=True)
        self.assertEqual(got.returncode, 0, "failed with stderr: {}".format(got.stderr.decode("utf8")))
        # Do not check golden file. Pointer value is inconsistent.

if __name__ == "__main__":
    unittest.main()