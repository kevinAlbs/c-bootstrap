// main.c has another static int local().
// nm reports two t _local entries.
// nm reports: t _local
static int
local () {
    return 456;
}

int function_in_another_file () {
    return local();
}