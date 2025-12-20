# Print Invalid Arrays

A tool to check for BSON arrays with invalid keys. Quoting https://bsonspec.org/spec.html:

> A BSON document with integer values for the keys, starting at 0 and continuing sequentially. For example, the array `['red', 'blue']` encodes as the document `{'0': 'red', '1': 'blue'}`.

**This is not a supported MongoDB product**

To build, first install the MongoDB C driver. On macOS this can be done with `brew install mongo-c-driver`:

```bash
cmake -B build
cmake --build build
```

To run:
```bash
./build/print-invalid-arrays test-invalid.bson
```

test-invalid.bson is a BSON document like: `{ "array": [ "foo", "bar" ]}` except the keys in the array are "0" and "2".

Get output like:
```
'array': [array]
'array.0': [utf8]
'array.2': [utf8]
'array' has invalid array keys: 0, 2
```
