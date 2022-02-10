set -o errexit
set -o xtrace

if [[ "$(basename $(pwd))" != "c-bootstrap" ]]; then
    echo "Error: run this script from c-bootstrap."
    exit 1
fi

DIR=$(pwd)
. ./etc/install_libbson.sh
cd $DIR
. ./etc/install_libmongocrypt.sh
cd $DIR
. ./etc/install_mongo_c_driver.sh
cd $DIR