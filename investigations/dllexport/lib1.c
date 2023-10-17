#pragma once
// Declarations ... begin
int built_with_empty__included_with_empty(void);
int built_with_empty__included_with_dllimport(void);
int __declspec( dllexport ) built_with_dllexport__included_with_empty(void);
int __declspec( dllexport ) built_with_dllexport__included_with_dllimport(void);
// Declarations ... end

// Definitions ... begin
int built_with_empty__included_with_empty(void) { return 123; }
int built_with_empty__included_with_dllimport(void) { return 123; }    
int built_with_dllexport__included_with_empty(void) { return 123; }    
int built_with_dllexport__included_with_dllimport(void) { return 123; }
// Definitions ... end