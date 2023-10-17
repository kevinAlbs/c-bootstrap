#pragma once
// Declarations ... begin
#if !defined(LIB2_SHARED)
// Avoids "unresolved external symbol" error.
int built_with_empty__included_with_empty(void);
int __declspec(dllimport) built_with_empty__included_with_dllimport(void);
#endif
int built_with_dllexport__included_with_empty(void);
int __declspec( dllimport ) built_with_dllexport__included_with_dllimport(void);
// Declarations ... end

int __declspec(dllexport) foo()
{
    // Call all imported functions.
    int result = 0;
    #if !defined(LIB2_SHARED)
    // Avoids "unresolved external symbol" error.
    result += built_with_empty__included_with_empty();
    result += built_with_empty__included_with_dllimport();
    #endif
    result += built_with_dllexport__included_with_empty ();
    result += built_with_dllexport__included_with_dllimport();
    return result;
}