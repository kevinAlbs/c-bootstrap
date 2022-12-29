import lldb
import shlex

help = """
print_mongoc_arr prints a mongoc_array_t.

Usage:
print_mongoc_arr <variable name> <pointer to value type>

Example:
(lldb) print_mongoc_arr arr_i32 int*
(int *) $19 = 0x00005555555592a0 {
  (int) [0] = 1
  (int) [1] = 2
}
""".strip()

def print_mongoc_arr (debugger : lldb.SBDebugger, command: str, result: lldb.SBCommandReturnObject, internal_dict : dict):
    args = shlex.split(command)
    if len(args) == 2:
        varname = args[0]
        ptr_type = args[1]
    else:
        print ("Error: expected arguments")
        print (help)
        return

    target : lldb.SBTarget = debugger.GetSelectedTarget()
    process : lldb.SBProcess = target.GetProcess()
    thread : lldb.SBThread = process.GetThreadAtIndex(0)
    frame : lldb.SBFrame = thread.GetSelectedFrame()
    arr_sbval : lldb.SBValue = frame.FindVariable (varname)
    if not arr_sbval.IsValid ():
        print ("Failed to find variable '{}': {}".format(varname, arr_sbval.GetError()))
        return

    len_sbval : lldb.SBValue = arr_sbval.GetChildMemberWithName ("len")
    if not len_sbval.IsValid ():
        print ("Failed to get member 'len': {}".format(len_sbval.GetError()))
        return

    cmd = "parray {} ({})({}.data)".format(len_sbval.GetValueAsUnsigned(), ptr_type, varname)
    debugger.HandleCommand(cmd)


def __lldb_init_module (debugger : lldb.SBDebugger, dict):
    debugger.HandleCommand("command script add --function print_mongoc_arr.print_mongoc_arr print_mongoc_arr")
    print ("print_mongoc_arr has been imported. Try `print_mongoc_arr <varname> <type>`.")
