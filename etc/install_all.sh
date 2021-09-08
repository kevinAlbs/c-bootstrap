set -o errexit
set -o xtrace

if [[ "$(basename $(pwd))" != "c-bootstrap" ]]; then
    echo "Error: run this script from c-bootstrap."
    exit 1
fi

./etc/install_libbson.sh
./etc/install_libmongocrypt.sh
./etc/install_libmongocrypt.sh