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

* Local variables uses `lower_snake_case`.
* `#define` uses `UPPER_SNAKE_CASE`.
* Everything is in a namespace.
* Therefore, there are no prefixes.

#### Structs

Structs shall only be used for "plain-old-data" POD type with no logic or any functions except a data member initialization function (constructor).
Structs shall not have protected or private members.
Structs shall not inherit, not shall others inherit from it.

* Struct definition shall use camel case with `s` prefix: `sSomeStruct`.
* Struct members and initialization function shall use `lower_snake_case` with no prefix.
* Struct shall not be defined with `typedef` keyword on C++ code.

```cpp
struct sSomeStruct {
    int x_foo;
    int y_bar;
};
```

#### Classes

Classes shall be used to create complex object with functions and logic in it.

* Class definition shall use Pascal Case: `SomeClass`.
* Class members and functions shall use `lower_snake_case`. 
* Class data members have an `m_` prefix: `m_data`.

```cpp
class SomeClass {
public:
    SomeClass();
    ~SomeClass()

    set_foo(int foo);
    int calculate_something(int param_1, int param_2);
private:
    int x_foo;
    int y_bar;
};
```

#### Other types

Any other type shall be defined with `lower_snake_case` with `_t` suffix:

```cpp
typedef std::list<std::string> string_list_t;
```

### Coding patterns

This section describes some of the coding patterns we prefer to see in our code.
Using the same kind of coding patterns consistently makes it easier for others to understand the code at first glance.
Conversely, code that breaks the usual patterns takes more time to understand and may put the reader on the wrong foot.

Exceptions are not used at all in prplMesh code.
If a standard library function is called that may throw an exception, make sure to do the necessary checks before, to make sure an exception cannot be thrown.

#### Conditionals

Use no spaces inside parentheses. The if and else keywords belong on separate lines.

```cpp
    if (condition) {    // no spaces inside parentheses
        ...             // 4 space indent
    } else if (...) {   // The else goes on the same line as the closing brace
        ...
    } else {
        ...
    }
```

Always put a space between the if and the open parenthesis. Also have a space between the close parenthesis and the curly brace.

```cpp
    if(condition) {   // Bad - space missing after IF
    if (condition){   // Bad - space missing before {
    if(condition){    // Doubly bad

    if (condition) {  // Good - proper space after IF and before {
```

Always use curly braces, even for single-line statements. 
It makes complex conditions or statements more readable, and can prevent runtime errors when using multi-line MACROs.

```cpp
    if (condition)      // Bad - curly braces missing
        Foo();          // 4 space indent

    if (condition) {    // Good
        Bar();          // 4 space indent
    }
```

Short conditional statements may be written on one line if this enhances readability. 
You may use this only when the line is brief, simple and the statement does not use the else clause.
Can also be used for inline return conditions, as long as the condition is surrounded with parentheses.

Examples:
```cpp
    if (x == is_foo) { return new Foo(); }
    if (x == is_bar) { return new Bar(); }
    return (x == is_foo) ? Foo() : Bar();
```

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
        if (...) {
            it = nodes[i].erase(it);
        } else {
            it++;
        }
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

Non-scalar function arguments should be passed as const-reference `const foo&` instead of by value `foo` if the structure is larger than 32 bits.
This specifically applies to `std::string` arguments.
Even though the C++11 move constructor can avoid a large part of the copy overhead implied by passing by value, it is not always very clear when the move semantics apply.
Therefore, we choose to always explicitly use const-reference.
We empirically determined that GCC generates equal or better code for const-reference arguments for the 6-byte `sMacAddress` structure.
The difference is too small to matter much, but it's good to have a fixed rule of thumb, therefore we set the limit to 32 bits.

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

### Klocwork 

We use **Klocwork** as our static code analyzer.
One of the most common issues that the Klocwork complaints about is comparing the return value of a function to nullptr instead of checking for non-zero return value.

So to avoid creating more of these kinds of Klocwork issues, please always prefer `if (!ptr)` instead of `if (ptr == nullptr)` when the return type of the function is a smart pointer.
