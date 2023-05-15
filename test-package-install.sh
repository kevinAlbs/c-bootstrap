set -o errexit
cat << 'HEREDOC' > test.c
// test.c
#include <mongocrypt/mongocrypt.h>
#include <stdio.h>
 
int main () {
    printf ("mongocrypt_version=%s", mongocrypt_version(NULL));
}
HEREDOC
sudo sh -c 'curl -s --location https://www.mongodb.org/static/pgp/libmongocrypt.asc | gpg --dearmor >/etc/apt/trusted.gpg.d/libmongocrypt.gpg'
UBUNTU_VERSION=bionic
LIBMONGOCRYPT_VERSION=1.6
echo "deb https://libmongocrypt.s3.amazonaws.com/apt/ubuntu $UBUNTU_VERSION/libmongocrypt/$LIBMONGOCRYPT_VERSION universe" | sudo tee /etc/apt/sources.list.d/libmongocrypt.list
sudo apt-get update || true
sudo apt-get install -y libmongocrypt-dev
gcc test.c $(pkg-config --libs --cflags libmongocrypt)
rm test.c a.out
sudo apt-get remove -y libmongocrypt-dev libmongocrypt0