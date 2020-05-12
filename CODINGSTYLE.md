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

Prefer to use std algorithms as much as possible.
Get to know std algorithms.
std algorithms are better than hand crafted loops.
std algorithms are thoroughly tested, well documented, and do exactly as expected.
std algoritms are also well documented.
```cpp
std::transform(in.begin(), in.end(), out.begin(), 
               [](/*...*/) -> /*...*/ {
                   /*...*/
               } );
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

Prefer to use multiple points of exit from functions.  
One might argue that it is less confusing if there is only a single exit point, but it leads to more nesting and therefore more indentation which makes the code less readable.  
Another advantage of using multiple points of exit is that you can check preconditions and exit early at the start of a function, so that you know in the body of the function that certain conditions are true, without the entire body of the function being indented 5 miles off to the right.  
This usually minimizes the number of scopes you have to worry about, which makes code much easier to follow.  
Multiple points of exit means that you can exit anywhere you please.  
This used to be more confusing in the old days, but now that we have syntax-coloring editors and compilers that detect unreachable code, it's a lot easier to deal with.

Avoid using `goto` - The goto failure pattern is a corollary of the multiple exit points.  
It applies when there is cleanup to do, but since we use C++, there should never be any cleanup to do (it should all be handled by automatic variables).

Examples:

Good - multiple points of exit:

```cpp
bool func()
{
    if (something) {
        // do something
        return true;
    } else if (something else) {
        // do something else
        return false;
    } else {
        return false;
    }
}
```

Another example - the idea is to not use an else clause in this case to reduce indentation levels:

```cpp
bool func(int arg1)
{
    // validate arguments
    if (arg1 < 0) {
        return false;
    }

    // do something

    return true;
}
```

Bad - Single point of exit which adds an unecessary indentation level to the else clause, and makes the function longer:

```cpp
bool func(int arg1)
{
    bool result = false;

    // validate arguments
    if (arg1 < 0) {
        result = false;
    } else {
        // do something
    }

    return result;
}
```


Bad - single point of exit with `goto`:

```cpp
bool func()
{
    bool result = false;
    if (something) {
        goto failure;
    }
    // do something
    if (some condition) {
        goto failure;
    }
    result = true; // success
    goto success;

failure:
    cleanup();
success:
    return result;
}
```

## Comments
Comments are important part of the code base.
Comments are not compiled, nor have any influence on run time or any machinary.
Comments are for humans, for developers, for us.
Comments have the sole purpose of keeping the code maintainable.
A good comment is a one that you, the writer of the comment, can understand after a while, after getting out of the context of writing it.
Comments are written for future readers, it may be the future you, it may be someone else - write your comments with this in mind.
Comments normaly explain the *why* and not the *what*.

### Simple Comments
It is useless to state the obvious:
```cpp
// checking that the pointer is not null
if (ptr){
}
```

However, it is extremely helpful to state our expectations:
```cpp
// it is legal to receive an empty pointer.
// in case of exiting pointer pointer we need to ...
// and in case it is null we need to ...
if (ptr){
} else {
}
```

### Spanning Comments
Sometimes the code is not trivial even if it doesn't do tricky things. 
It just has its logic that might be buried in a few lines of technical details that have nothing to do with the logic itself, like error exits. Sometimes it is hard to see the intent behind the mechanics of the code.
In these cases it is more maintainable to comment the different sections in the function and their logic.
A small example can be seen [here](https://github.com/prplfoundation/prplMesh/blob/20388c47cf61f9419576fa223fd140e17adcb67d/agent/src/beerocks/slave/son_slave_thread.cpp#L4373-L4405).
This else clause starts with a story about what is done in it. 
In this case not what would you expect. 
It also states why it is fine and suggests future changes.
Later in the code the promise that was taken in the initial comment is implemented.
It is useful to read the first comment, then look forward in the code for more comments that support the story on top, and only then dive into the code to ensure it does what it says it does.

Here is another (imaginary) example that "tells the story" and makes it much easier to follow the code structure and logic even before reading the code itself:
```cpp
// in this section we have to gather information from few resources:
// A, B and C.
// The best would be to have all of them, but it is also legal and
// possible to progress with A and C only.
// therefore we try to get A, then we try to get C,
// and upon success we try to get B as well.
// the action is taken at the end.

// get A
/* 10 lines of complex machinery, iterating on a complex data structure */

// get C
/* 22 lines of code with 5 exit points in various conditions */

// get B
auto b = getB();

// take the action
if (a && b && c) {
    actionABC(a, b, c);
} else if (a && b) {
    actionAC(a, c);
} else {
// error
}
```
### Poor Comments
One indication for poor commenting process is that developers don't read comments.
It means that they get used to comments that are irrelevant, hard to read or just outdated.
This situation is in many cases worse then non commented code at all, because developers, seeking for assistance are reading misleading comments.

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
