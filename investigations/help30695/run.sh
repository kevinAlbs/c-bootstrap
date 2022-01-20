python before_test.py
echo "5000 inserts in separate processes ... start"
if [ -n "$WITH_COLBY_FIX" ]; then
    echo "Using Colby's fix with LD_LIBRARY_PATH"
    export LD_LIBRARY_PATH=/home/ubuntu/code/c-bootstrap/install/mongo-c-driver-CDRIVER-4231-new-oid-randomness/lib
fi
echo "This may take about 5-10 minutes"
for i in {1..5000}; do
    ./cmake-build/main.out
done
echo "5000 inserts in separate processes ... end"
python after_test.py
