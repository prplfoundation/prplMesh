# prplMesh coding style

This document describes the intended coding style.
Note that existing code does not always comply to it, but is in the process of aligning to it - this is work in progress.

Line length is 100 characters, but this is not a hard limit.

## C++ code

Code formatting is done with clang-format.
See the [.clang-format](.clang-format) file for the specification.
Sometimes the code doesn't look good with clang-format.
In these cases, it can be overridden by surrounding it with `clang-format-off` and `clang-format-on`.

### Naming convention

* Almost everything uses `lower_snake_case`.
* `#define` uses `UPPER_SNAKE_CASE`.
* Everything is in a namespace.
* Therefore, there are no prefixes.
* Private data members have an "m" prefix: `m_data`.
* Getter functions use the name without "m": `data()`.
* Setter functions use the "set" prefix: `set_data(value)`.

## Python code

Follow PEP8.
This can be checked with the `flake8` tool.
Remember to set line length to 100.

## Documentation

Documentation is written in MarkDown (Github-flavored) so it can be viewed directly online.

Lines in markdown documents are not wrapped, but every sentence should start on a new line.
Wrapping in text files is annoying, because adding a single word can cause the entire paragraph to be rewrapped.
On the other hand, putting everything on one line is not readable either.
As a compromise, no fixed wrapping is done.
Instead, each sentence should be started on a new line.
This way, the diff will only show the sentences that are really affected by the change.
For very long sentences, subsentences may also be placed on a separate line.

Non-inline blocks (lists, preformatted, ...) must be explicitly on a separate paragraph.
I.e., leave a blank line before and after the block.
