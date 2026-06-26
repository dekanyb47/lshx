lshx
====
Forked from [lsh][2].

lshx is a simple implementation of a shell in C.  It demonstrates the basics of how a shell works.
That is: read, parse, fork, exec, and wait.  Since its purpose is demonstration
(not feature completeness or even fitness for casual use), it has many
limitations, including:

* Commands must be on a single line.
* Arguments must be separated by whitespace.
* No piping or redirection.
* Few builtins.

Features added
--------------
- New builtins (cd, ls, mkdir, mv, stat)
- Custom tokenizer for splitting input line, and support for quoting arguments
- Flag parsing for builtins

Running
-------

Use `cd src && gcc -o ../lsh *.c` to compile, and then `./lsh` to run.

License
-------

This code is in the public domain (see [UNLICENSE](UNLICENSE) for more details).
This means you can use, modify, and distribute it without any restriction.  I
appreciate, but don't require, acknowledgement in derivative works.

[1]: http://brennan.io/2015/01/16/write-a-shell-in-c/
[2]: https://github.com/brenns10/lsh
