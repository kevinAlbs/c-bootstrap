from pathlib import Path
import bson

valid_bytes = bson.encode({"array": ["foo", "bar"]})
Path("test-valid.bson").write_bytes(valid_bytes)

# Mess up an array index by changing '1' to '2'
assert valid_bytes[27] == ord('1')
invalid_bytes = bytearray(valid_bytes)
invalid_bytes[27] = ord('2')

Path("test-invalid.bson").write_bytes(invalid_bytes)

nested = bson.encode({"doc": { "foo": ["bar", "baz"]}})
Path("test-nested.bson").write_bytes(nested)
