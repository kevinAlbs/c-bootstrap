python before_test.py
echo "5000 inserts in separate processes ... start"
echo "This may take about 5-10 minutes"
for i in {1..5000}; do
    ./cmake-build/main.out
done
echo "5000 inserts in separate processes ... end"
python after_test.py
