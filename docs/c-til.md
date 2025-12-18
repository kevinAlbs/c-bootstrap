TIL: C99 permits taking address of a compound literal:

```c
mystruct* s = &(mystruct) { .x = 123 };
```
