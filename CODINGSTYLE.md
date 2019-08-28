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

### Coding patterns

This section describes some of the coding patterns we prefer to see in our code.
Using the same kind of coding patterns consistently makes it easier for others to understand the code at first glance.
Conversely, code that breaks the usual patterns takes more time to understand and may put the reader on the wrong foot.

Exceptions are not used at all in prplMesh code.
If a standard library function is called that may throw an exception, make sure to do the necessary checks before, to make sure an exception cannot be thrown.

#### Loops

Avoid endless loops like `while (true)`.
Some static analysis tools give an error or have a hard time analysing it.
They are usually able to analyse some forms of endless loops, but it's better to be consistent and never have them.
Example:

```cpp
    static const size_t max_tlv = 256;
    size_t tlv_idx;
    for (tlv_idx = 0; tlv_idx < max_tlv; tlv_idx++) {
       ...
    }
    if (tlv_idx == max_tlv) {
        LOG(ERROR) << "Reached stop condition";
        return false;
    }
```

Prefer to use ranged loops if at all possible:

```cpp
    for (const auto& element : elements) {
        // code
    }
```

If a ranged loop is not possible, still use C++-style iterator rather than indexing:

```cpp
    for (auto it = nodes[i].begin(); it != nodes[i].end();) {
        if (...)
            it = nodes[i].erase(it);
        else
            it++;
    }
```

In case only a C-style "next" function is available, use assignment-in-while:

```cpp
    while ((type = cmdu_rx.getNextTlvType()) != int(ieee1905_1::eTlvType::TLV_END_OF_MESSAGE)) {
        ...
    }
```

#### Functions

Member functions must be declared const, unless they modify the object.

Non-scalar function arguments should be passed as const-reference `const foo&` instead of by value `foo`.
This specifically applies to `std::string` arguments.
Even though the C++11 move constructor can avoid a large part of the copy overhead implied by passing by value, it is not always very clear when the move semantics apply.
Therefore, we choose to always explicitly use const-reference.

Functions should almost always return a success or failure status.
If a function needs to return some other information as well, use that as the return value, and use a sentinel value to indicate failure.
Only use output arguments if more than one piece of information needs to be returned.
In the following example, the empty string is the sentinel value indicating failure.

```cpp
/**
 * @brief Parse bss_info string into bss_info_conf_struct.
 *
 * @param[in] bss_info_str String conataining bss info configuration
 * @param[out] bss_info_conf Controller database struct filled with configuration.
 * @return al_mac on success, empty string otherwise.
 */
std::string parse_bss_info(const std::string &bss_info_str, db::bss_info_conf_t &bss_info_conf);
```

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
