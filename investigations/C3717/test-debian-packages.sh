function assert_eq () {
    a="$1"
    b="$2"
    if [[ "$a" != "$b" ]]; then
        echo "Assertion failed: $a != $b"
        # Print caller
        caller
        exit 1
    fi
}

function assert_contains ()   {
    a="$1"
    b="$2"
    if [[ "$a" != *"$b"* ]]; then
        echo "Assertion failed: '$a' does not contain '$b'"
        # Print caller
        caller
        exit 1
    fi
}

function assert_not_contains ()   {
    a="$1"
    b="$2"
    if [[ "$a" == *"$b"* ]]; then
        echo "Assertion failed: '$a' does contain '$b'"
        # Print caller
        caller
        exit 1
    fi
}

function cleanup () {
    (
        # Uninstall packages.
        sudo apt remove libmongoc-1.0-0 libmongoc-dev libmongoc-doc -y
    ) > /dev/null 2>&1
}

# Test attempting to develop with libmongoc-1.0-0 fails.
{
    cleanup
    captured=$((
        sudo apt install libmongoc-1.0-0
        gcc -o main.out main.c $(pkg-config --libs --cflags libmongoc-1.0)
    )2>&1)
    assert_contains "$captured" "foobar"
}
