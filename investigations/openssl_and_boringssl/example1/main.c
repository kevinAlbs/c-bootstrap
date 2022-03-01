// Q: What happens if two files in the same executable have the same static symbol?
// A: nm reports two symbols with "t". "t" indicates a local symbol.

// nm reports: t _local
static int
local () {
    return 456;
}

extern int function_in_another_file();

// nm reports: T _main
int main () {
    return 123 + local() + function_in_another_file();
}
