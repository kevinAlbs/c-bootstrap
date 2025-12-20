echo "Testing valid bson ..."
./build/print-invalid-arrays test-valid.bson
# 'array': [array]
# 'array.0': [utf8]
# 'array.1': [utf8]

echo "Testing valid nested bson ..."
./build/print-invalid-arrays test-nested.bson
# 'doc': [document]
# 'doc.foo': [array]
# 'doc.foo.0': [utf8]
# 'doc.foo.1': [utf8]

echo "Testing invalid bson ..."
./build/print-invalid-arrays test-invalid.bson
# 'array': [array]
# 'array.0': [utf8]
# 'array.2': [utf8]
# 'array' has invalid array keys: 0, 2
