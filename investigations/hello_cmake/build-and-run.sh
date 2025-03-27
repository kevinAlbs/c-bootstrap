# Install C driver:
brew install mongo-c-driver
# Configure application:
cmake -S. -Bcmake-build
# Build application
cmake --build cmake-build --target main.out
# Run application
./cmake-build/main.out
