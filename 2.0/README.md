In researching options for a C driver 2.0, I would like to produce a list of all current symbols included in public headers.

Does anyone have a recommendation for tools to use?

I tried abi-compliance-checker and compare against a (nearly empty) stub library. However, the "Source Compatibility" does not include all macros (e.g. `BCON_UTF8` is missing).

I tried clang's find-all-symbols, but it included an `#undef`'ed DECL_ATOMIC_STDINT.
