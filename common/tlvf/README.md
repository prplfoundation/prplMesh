
# TLV Factory

- [TLV Factory](#tlv-factory)
- [Overview](#overview)
- [Requirements](#requirements)
- [Unsupported features](#unsupported-features)
- [User Guide](#user-guide)
  - [YAML](#yaml)
    - [Namespaces](#namespaces)
    - [Enum &amp; Enum classes](#enum-amp-enum-classes)
      - [Enum storage](#enum-storage)
      - [Enum class](#enum-class)
      - [Constant values](#constant-values)
    - [Structs](#structs)
      - [Additional parameters](#additional-parameters)
      - [Bit-field structs](#bit-field-structs)
    - [Classes](#classes)
      - [Variable Length lists](#variable-length-lists)
      - [TLV Class](#tlv-class)
      - [Multi class file](#multi-class-file)
    - [Examples](#examples)
      - [Tlv class with a dynamic unknown length list](#tlv-class-with-a-dynamic-unknown-length-list)
      - [Tlv class with a dynamic known length list, a struct and an enum](#tlv-class-with-a-dynamic-known-length-list-a-struct-and-an-enum)
      - [Multi class](#multi-class)
    - [License file](#license-file)
    - [TLVf Configuration File](#tlvf-configuration-file)
    - [Source Files](#source-files)
      - [cBaseClass (.h, .cpp)](#cbaseclass-h-cpp)
      - [CmduMessage (.h, .cpp)](#cmdumessage-h-cpp)
      - [swap (.h)](#swap-h)
  - [CPP Code](#cpp-code)
    - [Generated Classes API](#generated-classes-api)
      - [Constructors](#constructors)
      - [Public Methods](#public-methods)
      - [Inherited methods (from cBaseClass)](#inherited-methods-from-cbaseclass)
      - [Private Methods](#private-methods)
      - [Simple type arrays methods](#simple-type-arrays-methods)
      - [Complex type arrays](#complex-type-arrays)
      - [Dynamic lists methods (both known and unknown length)](#dynamic-lists-methods-both-known-and-unknown-length)
    - [cCmduMessage API](#ccmdumessage-api)
  - [Python script](#python-script)

# Overview

Each Multi-AP message consists of a CMDU header and 1 or more data elements called  TLVs, which stands for "Type Length Value".

TLVF stands for TLV Factory, which is a python package which takes YAML files representing TLVs as input, and generates C++ classes which represent those TLVs as output.

Although the TLVF is designed for TLV classes, it can generate any type of data element supported in the design.

# Requirements

The following are needed for building the TLVF on any Linux machine (local and cross compile):

| Package      |  Version  |
| --------- | -----:|
| python  | >=2.7 |
| c++  |  >=11 |
| PyYaml  |   |
| source files  |   |

# Unsupported features

- bit-fields larger than 8-bit
- No validity check for bit-fields
- Out of order lists allocation / creation / addition

# User Guide

## YAML

YAML is a human-readable data serialization language.
All the classes, structs and enums in the TLVF are described in YAML files.

Generally, a YAML file contains a single class / stryct / enum, with 2 exceptions:

- Several similar classes (that have many mutual members) can be defined in the same file
- ENUMs or structs that are exclusively used within a specific class can be defined in the same class YAML file

The general format for a YAML line is `<param name> : <param value>` or `<member name> : <member value>`.
Multiple parameters for a single member are also supported, in which case indentation must be used:

```yaml
<member name>
  <param1 name> : <param1 value>
  <param2 name> : <param2 value>
```

Metadata keywords start with an underscore to distinguish them from members name or c++ types. For example: `_type: class` - `_type` is a keyword recognized by the TLVF python code generator.

Comments in YAML files starts with `#`.

### Namespaces

At the top of each YAML file, namespace for the generated code can be added by including an `_include` attribute at the top of the YAML:

```yaml
_include: <include_path> # No quotation marks or arrow heads)
```

Including multiple files is also supported:

```yaml
_include: {
    <include path 1>,
    <include path 2>
}
```

### Enum & Enum classes

Enums and enum classes can be defined as follows:

```yaml
<enum name>:
    _type: <enum or enum_class>
    _enum_storage: <enum type>
    <NAME1>: <VALUE1>
    <NAME2>: <VALUE2>
    <NAME3>: <VALUE3>
```

For example:

```yaml
eTlvType:
    _type: enum_class
    _enum_storage: uint16_t
    TLV_END_OF_MESSAGE: 0x00
    TLV_AL_MAC_ADDRESS: 0x01
    TLV_MAC_ADDRESS: 0x02
    TLV_DEVICE_INFORMATION: 0x03
```

#### Enum storage

Each enum has a storage type - `uint8_t, uint16_t, uint32_t, uint64_t` are the most common types used in the TLVF.
Default storage type can be changed between different compilers, therefore it is recommended to always specify the storage type.
For enum classes, where strong typing is enforced by C++, this parameter is mandatory.

#### Enum class

Enum classes are similar to simple enums, but require an explicit cast to their storage type. For this reason, enum classes should be used when strong typing is required. In addition, enum classes have a validation method that can check if an integer value represents a matching enum class value.

#### Constant values

Defining global constants can be achieved by defining an `enum` which contains the constant values.

### Structs

Structs should be defined as below:

```yaml
<struct_name>:
     _type: struct
     <member 1 name>: <member 1 type>
     <member 2 name>: <member 2 type>
     …
```

Supported types:
    • Std types (intX_t, uintX_t, char).
    • Enums and enum classes (that was defined in the same or in other yamls).
    • Other structs (that was defined in the same or in other yamls).

#### Additional parameters

Other than type, some other parameters can also be defined for a member, using the multiple parameters syntax.

```yaml
<member 1 name>:
     _type: <member 1 type>
     <parameter 1 type>: <parameter 1 value>
     <parameter 2 type>: <parameter 2 value>
     …
```

All parameters are pre-defined using metadata keywords (starting with underscore).

The parameters:
`_type` – the member type (mandatory).
`_value` – default value for the member.
`_length` – define an array of this type.
`_comment` – comment that will be written near this member in the .h file.

Arrays can be defined inside structs, using the following syntax:

```yaml
<array_name>:
     _type: <array_type>
     _length: [ <array_size> ]
```

Array size can be either a number or a const (represented by an enum value)

#### Bit-field structs

It is possible to define bit fields structs for fields that are smaller than a byte.Currently, bit field struct can be composed of 1 byte only (uint8_t), and the fields should fill the whole byte.
The bit field should be the only member of the struct.

The syntax is as followed:

```yaml
<struct_name>:
     _type: struct
     _bit_field: uintX_t (currently only uint8_t is supported)
     <field 1 name>:
          _bit_range: [<msb index>, <lsb index>]
          _value: <default value>  //optional
     <field 2 name>:
          _bit_range: [msb index, lsb index]
…
```

Example - if we want to implement this bit-field byte:

Last fragment indicator | Relay indicator | Reserved
| --------- | ----- | -------:|
| 1 bit | 1 bit | 6 bits |

The yaml syntax will be:

```yaml
sFlags:
     _type: struct
     _bit_field: uint8_t
     last_fragment_indicator:
          _bit_range: [0,0]
          _value: 0
     relay_indicator:
          _bit_range: [1,1]
          _value: 0
     reserved:
          _bit_range: [7,2]
          _value: 0
```

Bit field can also use the attribute `_bit_field_enum: <enum name>`
This attribute allows the bit field to accept enum values (as long as the enum value length is less than or equal to the bit field size).
Example:

```yaml
sFlags:
   _type: struct
   _bit_field: uint8_t
   preference:
      _bit_range: [7,4]
   reason_code:
      _bit_range: [3,0]
      _bit_field_enum: eReasonCode
```

### Classes

Class configuration is the same as struct, but without the option of bitfields members and with more optional parameters:
The parameters:
`_type` – the member type (mandatory).
`_value` – default value for the member.
`_length` – define an array or a list of this type.
`_comment` – comment that will be written near this member in the .h file.

`_value_const` – marks the member as const with the given value.
`_class_const` – defines a static const class member with the given value.
`_length_var` – a member which is marked with _length_var: True will represent a list length and will be automatically updated when new entries are inserted to the list.

#### Variable Length lists

There are 4 types of variable (dynamic) length lists:

- Simple Known length list
- Simple Unknown length list
- Class Known length list
- Class Unknown length list

Known length lists are variable length lists in which the length (number of elements) is part of the TLV - `list [ list_length ]`

Unknown length lists are variable length lists in which the length is unknown - `list [ ]`. An unknown list has to be the last member of the class so its length can be calculated from the TLV length field and all the other class member sizes.

Simple lists are lists with simple member type - either `uintX_t` or structs.
Class lists are lists in which the member type is another class.

Example TLV with multiple lists types:

```yaml
_is_tlv_class : True

tlvTestVarList:
  _type: class

  type:
    _type: uint8_t
    _value_const: 1
  length: uint16_t

  simple_known_len_list:
    _type: uint16_t
    _length: [ list_length ]

  class_known_len_list:
    _type: cElement
    _length: [ list_length ]

  simple_unknown_len_list:
    _type: uint16_t
    _length: []

```

Simple lists are generated with an `alloc_<name>` function which is used to allocate the list.
The `alloc_<name>` function allocates room on the buffer for the selected amount of list entries, moves the remainder of the buffer to the new end of the list, and updates all the member pointers which are defined after the list to point to the correct address on the buffer after the allocation.

The situation is a bit more complex for class lists, where the internal class can also contain variable length lists.
For this reason, the generated code does not include `alloc_<name>` but instead have `create_<name>` and `add_<name>` functions.

Creating a class list adds the internal class initial size on the buffer (moves the remainder of the buffer and updates the member pointers) and returns the class pointer.
The user can now allocate and set the internal class as needed.
Once done, `add_<list>` is called which does the final update to the TLV length and the member pointers following this list.

The following YAML and drawing shows the steps done on the buffer when adding variable length lists:

```yaml
#L1 - simple known length list
#L2 - class known length list
#var - variable defined after L1 and L2

cTlvVarLen:
    type: 0
    length: uint16_t

    lists_length: uint16_t
    var1: uint16_t
    L1:
        _type: uint16_t
        _length: [ lists_length ] 
    L2:
        _type cType
        _length: [ lists_length ]
    data: sData

```

```bash

Initial state:

    m_var1  m_data
 _____|______|___________________
|_____|_var1_|_________data______|
      |
     m_L1 (empty)
     m_L2 (empty)


alloc_L1:

         m_var1   m_data
 ___________|______|_________________
|_____|L1XXX|_var1_|_______data______|
      |
     m_L1
     m_L2 (empty)

create_L2:

            m_var1   m_data
 ______________|______|__________________
|_____|L1XXX|L2|_var1_|_______data______|
      |     |
     m_L1   m_L2

alloc_L2_members:

            m_var1*   m_data*       // m_var1 and m_data invalid
 ______________|______|____________________
|_____|L1XXX|L2YYY|_var1_|_______data______|
      |     |
     m_L1   m_L2


add_L2:

               m_var1   m_data
 _________________|______|_________________
|_____|L1XXX|L2YYY|_var1_|_______data______|
      |     |
     m_L1   m_L2


```

---
**NOTE**

TLVF doesn't (YET) support out of order lists allocations, therefore Variable length lists have to be allocated in the same order they are defined in the YAML.

---

#### TLV Class

Class can be defined as a TLV class to represent a TLV element.
The syntax and all the properties of the class are identical to simple class, and this line should be added above the class definition:
_is_tlv_class: True
Adding this option gives us 2 advantages:
    1. Validating that the class has type field and length field.
    2. The length field is automatically calculated based on the class fields.

#### Multi class file

As mentioned above, it’s possible to define several classes inside a single file.
These classes should have a common base of members and generally have logical similarity.
A yaml file that contains more than one class should has the multi class attribute before the classes definition:
_multi_class: True
The limitation of multi-class files is that the classes must be simple classes – without dynamic length lists.
The big advantage of defining multiple class in a single file, is the option to define mutual members for them. For that you should put the attribute _multi_class_auto_insert: above the classes definitions, and under it insert the mutual members, just like you would in a simple class. After that start defining the classes themselves.

Another useful option is to define a multiclass member with a different name for each class, derived from the class name:

```yaml
_multi_class_auto_insert:
   <parameter_name>:
      _type: <enum type>
      _value_const: [_auto_value_by_name, k]  
      _class_const: True
```

The [_auto_value_by_name, 1] will be replaced with the class_name removing the first k letters.
For example, if the class name is cMySimpleClass and k=1 then the const value of this parameter will be MySimpleClass (removing the fiest char ‘c’).

### Examples

#### Tlv class with a dynamic unknown length list

```yaml
_namespace: ieee1905_1

_is_tlv_class: True
tlvNon1905neighborDeviceList:
   _type: class
   type:
      _type: eTlvType
      _value_const: TLV_NON_1905_NEIGHBOR_DEVICE_LIST
   length: uint16_t
   mac_local_iface: sMacAddress
   mac_non_1905_device:
      _type: sMacAddress
      _length: []
```

Note - `eTlvType` and `sMacAddress` are defined in different files.

#### Tlv class with a dynamic known length list, a struct and an enum

```yaml
_namespace: ieee1905_1
_is_tlv_class : True

tlvDeviceInformation:
   _type: class
   type:
      _type: eTlvType
      _value_const: TLV_DEVICE_INFORMATION
   length: uint16_t
   mac: sMacAddress
   info_length:
      _type: uint8_t
      _length_var: True
   info:
      _type: sInfo
      _length: [ info_length ]

sInfo:
   _type: struct
   mac: sMacAddress
   media_type: eMediaType
   media_info_length: uint8_t
   media_info: eMediaType

eMediaType:
   _type: enum
   _enum_storage: uint16_t
   IEEE_802_3U_FAST_ETHERNET: 0x0000
   IEEE_802_3AB_GIGABIT_ETHERNET: 0x0001
   IEEE_802_11B_2_4_GHZ: 0x0100
   IEEE_802_11G_2_4_GHZ: 0x0101
   IEEE_802_11A_5_GHZ: 0x0102
   MOCA_V1_1: 0x0300
   UNKNONWN_MEDIA: 0xFFFF
```

#### Multi class

```yaml
_include: tlvf/beerocks/beerocks_message_common.h # including this file in all generated files
_namespace: beerocks_message # all classes will be under that namespace
_multi_class: True # this file contains multiple classes (and so will    the generated h and cpp files)
_multi_class_auto_insert: # all the parameters below (down to the first class declaration) will be included in all classes in this file
  action_op:
    _type: eActionOp_BML
    _value_const: [_auto_value_by_name, 1]  
    _class_const: True

cACTION_BML_SET_LEGACY_CLIENT_ROAMING_REQUEST:
  _type: class
  isEnable: uint8_t

cACTION_BML_GET_LEGACY_CLIENT_ROAMING_RESPONSE:
  _type: class
  isEnable: uint8_t

cACTION_BML_CHANGE_MODULE_LOGGING_LEVEL_REQUEST:
  _type: class
  params: sLoggingLevelChange

cACTION_BML_WIFI_CREDENTIALS_UPDATE_REQUEST:
  _type: class
  params: sWifiCredentials

cACTION_BML_WIFI_CREDENTIALS_UPDATE_RESPONSE:
  _type: class
  error_code: uint32_t
```

### License file

In addition to the generated code, each file can also contain a pre-defined text at its top.
This is currently used to attach the Intel license text, located in a text file.
Since this text will be copied as is into a code files (that should later be compiled), The entire text should be commented.

### TLVf Configuration File

The tlvf script takes its configuration from a yaml configuration file.
The configuration file includes the yaml files paths, files to be copied to output directory (e.g. base classes, helper libraries etc.), license file and debugging settings.
Below is the format of the configuration file with in-line description:

```yaml
---
include_yaml_path: {
# This section includes the yaml files paths. It can accept individual files or entire directories. The path 
# given is relative to the script file path.
  "yaml/tlvf/ieee_1905_1/",
  "yaml/tlvf/common/",
  "yaml/tlvf/wfa_map/eTlvTypeMap.yaml",
  "yaml/tlvf/wfa_map/tlvApCapability.yaml",
}

include_source_path: {
# This section includes the paths of code files that should be copied into the output directory.
# They will be copied to the same path in the output directory (for example the file swap.h below will be copied to the path output/src/include/tlvf/swap.h, with ‘output’ being the script output directory).
  "src/include/tlvf/swap.h",
  "src/include/tlvf/cBaseClass.h",
  "src/src/cBaseClass.cpp",
  "src/include/tlvf/cCmduMessage.h",
  "src/src/cCmduMessage.cpp",
}

# Here the license file path is indicated.
source_license_header: "src/intel/license.txt"

# Debug options
debug:
  log_file: "tlvf.log"
  log_format: '%(levelname)s %(funcName)s(%(lineno)d): %(message)s'
  log_level: "DEBUG" #"INFO", "WARNING", "ERROR", "OFF"
#the script uses inline markers to determine the location of code lines inside the generated file. These markers are by default removed before the final file is generated, but can also be kept for debugging.
  keep_source_marker: false  
  dump_include_files: false
  dump_yaml: true
  dump_db: true
  dump_stack: true
```

### Source Files

There are a few non-generated source file that are copied to the output directory and provide functionality for the programmer and for the generated files.

#### cBaseClass (.h, .cpp)

This is the base class for all the classes generated by the TLVF.
It contains Constructor, protected members and public get method for some of the members.
The class members will be further described in the API section.

#### CmduMessage (.h, .cpp)

This class provides functionality for working with CmduMessage, which is the “container” for the cmdu header and TLVs (all generated classes).
The class members will be further described in the API section.

#### swap (.h)

This file contains several swap method for different types, which are necessary for sending the messages on the network bus (swap from little to big endian and vice versa).

## CPP Code

The python tlvf script generated cpp code from the yaml files.
Each yaml file is generated into an .h file, and each class (or multi-class) file is generated into an .h file and a .cpp file.
In addition, there are cpp non-generated source files that provide base code or additional API.

### Generated Classes API

In general, each class data is saved on a given buffer and all data allocation and manipulation is done on that buffer. 
Most of the class members are pointers to locations on the buffer and not actual variables.
Generally, other than the set/get methods.

#### Constructors

`class(uint8_t* buff, size_t buff_len, bool parse, bool swap_needed)`

- buff – the buffer to work on
- buff_len – the buffer length
- parse – true if the buffer contains data to parse, false if the buffer is empty. Default set to false.
- swap_needed – whether or not the parsed data should be swapped. Default set to false.

`class(std::shared_ptr<cBaseClass> base, bool parse, bool swap_needed)`

- base – shared_ptr to a cBaseClass to use its buffer.
- parse, swap_needed – same as previous constructor.

#### Public Methods

`class_swap()` – performs swap to all the class members (if necessary).

`get_initial_size()` – get the size of all fixed members of the class (not including dynamic lists or nested classes).

`Get` methods to all fixed members defined in the yaml.

*Those get method return references to the members, so they can also be used for set  operations.

#### Inherited methods (from cBaseClass)

`getBuffPtr()` – get the pointer to the current location on the buffer (after writing the members of this class)

`getStartBuffPtr()` - get the pointer to the beginning of the buffer.

`getBuffRemainingBytes()` – get the size left on the buffer

`getLen()` – get the size used on the buffer by this class

`isInitialized()` – did the init() method succeed. If false is returned, an error occurred and the class instance should not be used.

#### Private Methods

`init()` – initializes the pointers to the buffer. If parse = true, also parses the data and swaps it if necessary. Called in the constructor.

#### Simple type arrays methods

This method is only generated for arrays of primitive types (e.g. `char`, `uint8_t` etc.)
`<type>* <list_name>(size_t length)`
For readability, Assume we defined a char array named interface_name:
char* interface_name(size_t length) - gets a pointer to the beginning of the array.
The array length should be specified for protection – making sure the programmer knows the size he has, so he won’t override other data on the buffer.

#### Complex type arrays

This method is only generated for arrays of struct or class types.
`std::tuple<bool, <type> > <list_name>(size_t idx)`

For readability, assume we defined an array of sMacAddress named addresses:

`std::tuple<bool, sMacAddress&> addresses(size_t idx)` – get the element in the location idx. 

This method returns a tuple (a kind of element that contains a pair of values) of bool and the list type (`sMacAddress` in our case).
If the bool value is true, then the element data is valid. 
If it’s false then the element data is invalid (may be null or garbage) – this happens when the index was out of boundary.

#### Dynamic lists methods (both known and unknown length)

These methods are only generated for dynamic length lists.
`std::tuple<bool, <type> > <list_name>(size_t idx)`

`bool alloc_<list_name>(size_t count)`

For readability, assume we defined a list of `sMacAddress` named addresses:
`std::tuple<bool, sMacAddress&> addresses(size_t idx)` - same as the method in complex type arrays.

`bool alloc_addresses(size_t count)` - This method allocates space on the buffer for count elements of the list type (sMacAddress in our case).
The method returns whether or not the allocation succeeded (it returns false when there is not enough space left on the buffer).

Other methods - `create_addresses()` and `add_addresses()` are for variable length class lists/arrays.

### cCmduMessage API

This non-generated class is for building a CMDU message.
This class contains a cCmduHeader (for the message header, generated class) and an array of class objects attached to the message (usually for the TLVs in the message).

`std::shared_ptr<cCmduHeader> parse(uint8_t* buff, size_t buff_len, bool swap_needed)`

parsing a message from a buffer. Only parsing the header, not the TLVs (these should be parsed separately).

Returns a shared pointer to the message header.

- buff – the buffer.
- buf_len – the buffer length.
- swap_needed – whether or not swap is needed to the parsed values.

`std::shared_ptr<cCmduHeader> create(uint8_t* buff, size_t buff_len, uint16_t id, eMessageType message_type)`

creating a new cmdu message on the buffer.

Returns a shared pointer to the message header.

- buff, buff_len – the buffer and its length.
- id – the message id.
- message_type – the message type (enum type, generated).

`std::shared_ptr<cCmduHeader> create(uint8_t* buff, size_t buff_len, uint16_t id, tlvVendorSpecific::eVendorOUI voui)`

Same as the create method above, but instead of message_type, this method gets a vendorOUI and create a Vendor Specific CMDU message.

`std::shared_ptr<cCmduHeader> getCmduHeader()` – get a shared pointer to the cmdu header.

`template <class T> std::shared_ptr<T> addClass()`

Add an object of type <T> to the array. 

- For parsed message - parse this type from the buffer.
- For created messages – allocate this type on the buffer.

it is not allowed to use addClass method if the last tlv is not fully initialized!

Returns a pointer to the added class.
In case of an error, this pointer will point to null.

`static uint16_t getCmduHeaderLength()` - static method. get the cmdu header length (const size).

`std::shared_ptr<cBaseClass> getClass(size_t idx)` - get a shared pointer to the element in the idx location in the class array.

`size_t getMessageLength()` – get the entire message length.

`const uint8_t* getMessageBuff()` – get the buffer of the message.

`bool getNextTlvType(eTlvType& tlvType)`

get the next tlv type on the message (for parsing). The method sets the tlv type on the received parameter.

Returns true if the tlv type is valid, false otherwise (when false, the parameter is not assigned).

`uint16_t getNextTlvLength()` – get the next tlv length. Return 0 in case of an invalid tlv.

`void reset()` – reset the message buffer.

`bool finalize()` - Prepares the message to be sent on the network bus.
Must be called before sending the message on the bus!
This method adds an end of message TLV (required in the end of every message) and performs swap to the message.
This method has to be called before sending the message on the bus!

## Python script

The python script needs 2 arguments to run: The output directory and the configuration file.
Running it is as followed:

`tlvf.py  <src> <yaml> <out> -c <configuration file path>`

In addition, it has 2 more optional arguments:

`--print-dependencies` - This option prints the list of yaml files read by the script (defined in the configuration file) and exits (without generating the files) – it is used in the cmake file for dependencies.

`--print-outputs` - This option prints the list of generated and copied files – it is used in the cmake file for a list of files to compile.
