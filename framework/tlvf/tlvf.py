#!/usr/bin/python

# SPDX-License-Identifier: BSD-2-Clause-Patent
#
#
#
# Copyright (c) 2019 the prplMesh contributors (see AUTHORS.md)
#
#
#
# This code is subject to the terms of the BSD+Patent license.
#
# See LICENSE file for more details.
#
#

import argparse
import sys
import os
import yaml
import yaml.constructor
import pprint
import logging
import traceback
import shutil

#https://pyyaml.org/wiki/PyYAMLDocumentation
#https://learnxinyminutes.com/docs/yaml/
#http://python.omics.wiki/data-structures/dictionary/multiple-keys

logConsoleDisable = False
def logConsole(msg):
    if not logConsoleDisable: sys.stdout.write(msg)

try:
    from collections import OrderedDict
except ImportError:
    # try importing the backported drop-in replacement
    # it's available on PyPI
    from ordereddict import OrderedDict

class OrderedDictYAMLLoader(yaml.Loader):
    def __init__(self, *args, **kwargs):
        yaml.Loader.__init__(self, *args, **kwargs)
        self.add_constructor(u'tag:yaml.org,2002:map', type(self).constructYamlMap)

    def constructYamlMap(self, node):
        data = OrderedDict()
        yield data
        value = self.constructMapping(node)
        data.update(value)

    def constructMapping(self, node, deep=False):
        if isinstance(node, yaml.MappingNode):
            self.flatten_mapping(node)
        else:
            raise yaml.constructor.ConstructorError(None, None,
                'expected a mapping node, but found %s' % node.id, node.start_mark)

        mapping = OrderedDict()
        for key_node, value_node in node.value:
            key = self.construct_object(key_node, deep=deep)
            try:
                hash(key)
            except TypeError as exc:
                raise yaml.constructor.ConstructorError('while constructing a mapping',
                    node.start_mark, 'found unacceptable key (%s)' % exc, key_node.start_mark)
            value = self.construct_object(value_node, deep=deep)
            mapping[key] = value
        return mapping

class LastUpdatedOrderedDict(OrderedDict):
    def __setitem__(self, key, value):
        if key in self:
            del self[key]
        OrderedDict.__setitem__(self, key, value)


class TypeInfo:
    ERROR = "ERROR"
    NUMBER = "NUMBER"
    STD = "STD"
    CHAR = "CHAR"
    ENUM = "ENUM"
    ENUM_CLASS = "ENUM_CLASS"
    STRUCT = "STRUCT"
    CLASS = "CLASS"
    STRUCT_SWAP_FUNCTION_NAME = "struct_swap()"
    STRUCT_INIT_FUNCTION_NAME = "struct_init()"
    CLASS_SWAP_FUNCTION_NAME = "class_swap()"

    INT8 = "INT8"
    UINT8 = "UINT8"
    INT16 = "INT16"
    UINT16 = "UINT16"
    INT32 = "INT32"
    UINT32 = "UINT32"
    INT64 = "INT64"
    UINT64 = "UINT64"
    STD_TYPES = [INT8, UINT8, INT16, UINT16, INT32, UINT32, INT64, UINT64]

    def __init__(self, type_str):
        self.type_str = type_str
        self.type = TypeInfo.ERROR
        self.swap_needed = False
        self.swap_prefix = ""
        self.swap_suffix = ""
        self.swap_is_func = False
        self.is_std_type = False

        if type(self.type_str) == str:
            if self.type_str.find("::") != -1:
                self.type_str = type_str[type_str.rfind("::")+2:]
            if self.type_str.startswith("uint") or self.type_str.startswith("int"):
                self.set_type(self.type_str[:-2].upper()) # [:-2] removing the '_t' suffix
                if self.type == TypeInfo.INT64 or self.type == TypeInfo.UINT64:
                    self.swap_prefix = "tlvf_swap(64, reinterpret_cast<uint8_t*>("
                    self.swap_suffix = "))"
                    self.swap_needed = True
                elif self.type == TypeInfo.INT32 or self.type == TypeInfo.UINT32:
                    self.swap_prefix = "tlvf_swap(32, reinterpret_cast<uint8_t*>("
                    self.swap_suffix = "))"
                    self.swap_needed = True
                elif self.type == TypeInfo.INT16 or self.type == TypeInfo.UINT16:
                    self.swap_prefix = "tlvf_swap(16, reinterpret_cast<uint8_t*>("
                    self.swap_suffix = "))"
                    self.swap_needed = True
                elif not (self.type == TypeInfo.INT8 or self.type == TypeInfo.UINT8):
                    self.set_type(TypeInfo.ERROR)
            elif self.type_str.startswith("char"):
                self.set_type(TypeInfo.CHAR)
            elif self.type_str == "enum_class":
                self.set_type(TypeInfo.ENUM_CLASS)
            elif self.type_str == "size_t":
                self.set_type(TypeInfo.ERROR)
            elif len(self.type_str) > 2 and (str(self.type_str[1]).isupper() or str(self.type_str[1]).isdigit()):
                if self.type_str[0] == "e":
                    self.set_type(TypeInfo.ENUM)
                    self.swap_prefix = "tlvf_swap(8*sizeof("
                    self.swap_prefix += type_str
                    self.swap_prefix += "), reinterpret_cast<uint8_t*>("
                    self.swap_suffix = "))"
                    self.swap_needed = True
                elif self.type_str[0] == "s":
                    self.set_type(TypeInfo.STRUCT)
                    self.swap_suffix = TypeInfo.STRUCT_SWAP_FUNCTION_NAME
                    self.swap_needed = True
                    self.swap_is_func = True
                elif self.type_str[0] == "c":
                    self.set_type(TypeInfo.CLASS)
                    self.swap_suffix = TypeInfo.CLASS_SWAP_FUNCTION_NAME
                    self.swap_needed = True
                    self.swap_is_func = True
        elif str(self.type_str).isdigit():
            self.set_type(TypeInfo.NUMBER)

    def set_type(self, new_type):
        self.type = new_type
        self.is_std_type = (self.type in TypeInfo.STD_TYPES)

    def __repr__(self):
        pp = pprint.PrettyPrinter(indent=4)
        return pp.pformat( vars(self) )


class MetaData:
    TYPE_CLASS  = "class"
    TYPE_STRUCT = "struct"
    TYPE_ENUM   = "enum"
    TYPE_ENUM_CLASS   = "enum_class"
    META_PREFIX = "_"
    KEY_TYPE = "_type"
    KEY_ENUM_STORAGE = "_enum_storage"
    KEY_BIT_FIELD = "_bit_field"
    KEY_BIT_FIELD_ENUM = "_bit_field_enum"
    KEY_BIT_RANGE = "_bit_range"
    KEY_VALUE = "_value"
    KEY_VALUE_CONST = "_value_const"
    KEY_CLASS_CONST = "_class_const"
    KEY_OPTIONAL = "_optional"
    KEY_COMMENT = "_comment"
    KEY_LENGTH = "_length"
    KEY_LENGTH_VAR = "_length_var"
    KEY_LENGTH_MAX = "_length_max"
    LENGTH_TYPE_INT = "_int_"
    LENGTH_TYPE_CONST = "_const_"
    LENGTH_TYPE_VAR = "_var_"
    LENGTH_TYPE_DYNAMIC = "_dynamic_"
    DECELERATION_NAMESPACE               = "_namespace"
    DECELERATION_INCLUDE                 = "_include"
    DECELERATION_MULTI_CLASS             = "_multi_class"
    DECELERATION_MULTI_CLASS_AUTO_INSERT = "_multi_class_auto_insert"
    DECELERATION_IS_TLV_CLASS            = "_is_tlv_class"
    AUTO_VALUE_BY_NAME = "_auto_value_by_name"
    TLV_TYPE_LENGTH = "length"
    TLV_TYPE_TYPE = "type"

    def __init__(self, fname, name, dict):
        self.fname = fname
        self.name = name
        self.error = None
        self.namespace = None
        self.type = None
        self.type_info = None
        self.enum_storage = None
        self.bit_field = None
        self.bit_field_enum = None
        self.bit_field_pos = None
        self.bit_range = None
        self.value = None
        self.value_const = None
        self.class_const = False
        self.length = None
        self.length_type = None
        self.length_var = False
        self.length_max = 0
        self.length_var_tlv = False
        self.comment = None
        self.optional = False
        self.is_tlv_class = False
        self.constractor_h_lines = []
        self.constractor_cpp_lines = []
        self.alloc_list = []
        self.fillMetaData(dict)
        self.errorCheck(dict)
        self.children_types = {}
        self.list_index = 0
        self.lock_allocation_member_added = False
        self.lock_order_member_added = False
        self.marker_post_init_method_added = False
        
    def errPrefix(self):
        return "%s.yaml, param name=%s --> " % (self.fname, self.name)

    def fillMetaData(self, dict):
        for key , value in dict.items():
            if key.startswith(MetaData.META_PREFIX):
                if key == MetaData.KEY_TYPE:
                    if ( value == MetaData.TYPE_CLASS or
                           value == MetaData.TYPE_STRUCT or
                           value == MetaData.TYPE_ENUM ):
                        self.type = value
                    else:
                        self.type_info = TypeInfo(value)
                        if self.type_info == TypeInfo.ERROR:
                            self.error = self.errPrefix() + "bad type %s in dict=%s" % (value, str(dict))
                        else:
                            self.type = value
                elif key == MetaData.DECELERATION_IS_TLV_CLASS:
                    self.is_tlv_class = value
                elif key == MetaData.KEY_ENUM_STORAGE:
                    self.enum_storage = value
                    self.type_info = TypeInfo(value)
                elif key == MetaData.KEY_BIT_FIELD:
                    self.bit_field = value
                    if value.find("32") != -1:   self.abort("bit field of more than 8 bit is not supported")
                    elif value.find("16") != -1: self.abort("bit field of more than 8 bit is not supported")
                    elif value.find("8") != -1:  self.bit_field_pos = 7
                    else:
                        self.error = self.errPrefix() + "bit_field not supported: %s" % (value)
                elif key == MetaData.KEY_BIT_FIELD_ENUM:
                    self.bit_field_enum = value
                elif key == MetaData.KEY_BIT_RANGE:
                    self.bit_range = value
                elif key == MetaData.KEY_VALUE:
                    self.value = value
                elif key == MetaData.KEY_VALUE_CONST:
                    self.value_const = value
                elif key == MetaData.KEY_OPTIONAL:
                    self.optional = value
                elif key == MetaData.KEY_COMMENT:
                    self.comment = value
                elif key == MetaData.KEY_LENGTH:
                    if not (type(value) is list):
                        self.error = self.errPrefix() + "can't get _length value, not a list: %s" % (value)
                    elif len(value) > 1:
                        self.error = self.errPrefix() + "%s, has more than one value: %s" % (MetaData.KEY_LENGTH, value)
                    elif len(value) == 1:
                        self.length = value[0]
                        if str(value[0]).isdigit():
                            self.length_type = MetaData.LENGTH_TYPE_INT
                        elif str(value[0]).isupper():
                            self.length_type = MetaData.LENGTH_TYPE_CONST
                        elif str(value[0]).find("::") != -1:
                            self.length_type = MetaData.LENGTH_TYPE_CONST
                        else:
                            self.length_type = MetaData.LENGTH_TYPE_VAR
                    else:
                        self.length = []
                        self.length_type = MetaData.LENGTH_TYPE_DYNAMIC
                elif key == MetaData.KEY_LENGTH_VAR:
                    self.length_var = value
                elif key == MetaData.KEY_LENGTH_MAX:
                    self.length_max = value
                elif key == MetaData.KEY_CLASS_CONST:
                    self.class_const = True
                else:
                    self.error = self.errPrefix() + "unknown key: %s, dict: %s" % (key, dict)
    
    def errorCheck(self, dict):
        if self.type == None:
            if self.bit_range == None:
                self.error = self.errPrefix() + "mandatory key '%s' not set for dict: %s" % (MetaData.KEY_TYPE, dict)

    def __repr__(self):
        pp = pprint.PrettyPrinter(indent=4)
        return pp.pformat( vars(self) )

    @staticmethod
    def getConstValue(name, value_const):
        ret_val = ""
        ret_err = True
        if type(value_const) == list:
            if len(value_const) == 2 and value_const[0] == MetaData.AUTO_VALUE_BY_NAME and str(value_const[1]).isdigit():
                ret_val = name[value_const[1]:]
                ret_err = False
        else:
            ret_val = hex(value_const) if str(value_const).isdigit() else value_const
            ret_err = False
        return [ret_val, ret_err]
    
    @staticmethod
    def getFormattedValue(param_value):
        try:
            retVal = hex(param_value)
        except TypeError:
            # If it's not an integer, assume it's an enum value or other constant that can be used directly as a string.
            retVal = param_value
        
        return retVal



class TlvF:
    def __init__(self, src_path, yaml_path, out_path, conf_path, print_dependencies, print_outputs):
        self.CMAKE_PROPERTIES_VERSION = "1.0.0"
        self.CMAKE_SO_VERSION         = "1.0.0"
        
        # class templates const #
        self.AUTO_GENERATED_MESSAGE                 = "///////////////////////////////////////\n// AUTO GENERATED FILE - DO NOT EDIT //\n///////////////////////////////////////\n"
        self.CODE_INDENTATION                       = "    "
        self.CODE_INCLUDE_INSERT                    = "//~include_insert"
        self.CODE_FORWARD_DECLARATION_INSERT        = "//~forward_declaration_insert"
        self.CODE_START_INSERT                      = "//~code_start_insert"
        self.CODE_END_INSERT                        = "//~code_end_insert"
        self.CODE_NAMESPACE_INSERT                  = "//~namespace_insert"
        self.CODE_CLASS_START                       = "//~class_start"
        self.CODE_CLASS_END                         = "//~class_end"
        self.CODE_CLASS_CONSTRACTOR                 = "//~class_constractor"
        self.CODE_CLASS_DISTRACTOR                  = "//~class_distractor"
        self.CODE_CLASS_PUBLIC_VARS_INSERT          = "//~class_public_vars_insert"
        self.CODE_CLASS_PUBLIC_VARS_END_INSERT      = "//~class_public_vars_end_insert"
        self.CODE_CLASS_PRIVATE_VARS_INSERT         = "//~class_private_vars_insert"
        self.CODE_CLASS_ALLOC_INSERT                = "//~class_alloc_func_insert"
        self.CODE_CLASS_PUBLIC_FUNC_INSERT          = "//~class_public_func_insert"
        self.CODE_CLASS_PRIVATE_FUNC_INSERT         = "//~class_private_func_insert"
        self.CODE_CLASS_INIT_FUNC_INSERT            = "//~class_init_func_insert"
        self.CODE_CLASS_INIT_FUNC_SWAP_INSERT       = "//~class_init_func_swap_insert"
        self.CODE_CLASS_SWAP_FUNC_INSERT            = "//~class_swap_func_insert"
        self.CODE_CLASS_SIZE_FUNC_INSERT            = "//~class_size_func_insert"
        self.CODE_CLASS_IS_INIT_FUNC                = "//~class_is_init_func"
        self.CODE_ENUM_INSERT                       = "//~enum_insert"
        self.CODE_ENUM_VALIDATION_INSERT            = "//~enum_validation_insert"
        self.CODE_STRUCT_INSERT                     = "//~struct_insert"
        self.CODE_STRUCT_BITFIELD_INSERT            = "//~struct_bitfield_insert"
        self.CODE_STRUCT_REVERSED_BITFIELD_INSERT   = "//~struct_reversed_bitfield_insert"
        self.CODE_STRUCT_SWAP_FUNC_INSERT           = "//~struct_swap_func_insert"
        self.CODE_STRUCT_INIT_FUNC_INSERT           = "//~struct_init_func_insert"

        self.MEMBER_PARSE           = "parse"
        self.MEMBER_LOCK_ALLOCATION = "lock_allocation"
        self.MEMBER_LOCK_ORDER_COUNTER = "lock_order_counter"
        self.MEMBER_BUFF = "buff"
        self.MEMBER_BUFF_PTR = "buff_ptr"

        self.src_path = os.path.abspath(src_path)
        self.yaml_root_path = os.path.abspath(yaml_path)
        self.yaml_conf_name = os.path.abspath(conf_path)
        self.conf_output_path = os.path.abspath(out_path)
        self.print_dependencies = print_dependencies
        self.print_outputs = print_outputs
        global logConsoleDisable
        if print_dependencies or print_outputs: logConsoleDisable = True
        self.logger = None
        self.yaml_file_list = []
        self.generated_file_list = []
        self.copied_file_list = []
        self.db = LastUpdatedOrderedDict()
        self.db_enum_storage_type = {}
        self.db_yaml_paths = {}
        self.output_directories_h = []
        self.output_directories_cpp = []        

        # processing flow #
        logConsole("TlvF started...\n")
        self.loadConf()
        self.initLogger()
        self.loadYamlFileNames()
        if self.print_dependencies:
            dependencies = ";".join(self.yaml_file_list + [self.yaml_conf_name])
            sys.stdout.write(dependencies)
            sys.exit(0)

        self.loadAllYamlFilesToDB()
        self.generateCode()

        if self.print_outputs:
            outputs = ";".join(self.generated_file_list + self.copied_file_list)
            sys.stdout.write(outputs)

        logConsole("All Done.\n")
        sys.exit(0)
       
    def generateCode(self):
        logConsole("Generating source code...")
        for filename in self.yaml_file_list:
            self.openFile(filename)
            # first iteration: list local objects in order
            for (fname, obj_name) , dict_value in self.db.items():
                if fname != self.yaml_fname or obj_name.startswith(MetaData.META_PREFIX): continue
                self.local_include_list.append('"' + self.yaml_path + "/"  + obj_name + '.h"' )
                self.local_obj_list.append(obj_name)

            # second iteration: generate objects
            db_items_list = list(self.db.items())
            db_items_list_len = len(db_items_list)
            root_obj_meta = None
            for db_item_idx in range(db_items_list_len):
                (fname, obj_name) , dict_value = db_items_list[db_item_idx]
                if fname != self.yaml_fname: continue

                if obj_name.startswith(MetaData.META_PREFIX):
                    self.processDeceleration(obj_name, dict_value)
                    continue

                last_item = obj_name
                last_fname = fname #TODO: future use
                # self.logger.debug("fname=%s, name=%s, obj_meta:\n%s" % (fname, name, obj_meta) )
                self.logger.debug("fname=%s, name=%s\n" % (fname, obj_name) )

                obj_meta = MetaData(fname, obj_name, dict_value)
                if obj_meta.error: self.abort(obj_meta.error)
                if obj_meta.type == None: self.abort("%s.yaml --> '_type' not defined" % (self.yaml_fname))

                self.openObject(obj_meta, dict_value, root_obj_meta)
                if (obj_meta.type == MetaData.TYPE_CLASS and not self.multi_class):
                    root_obj_meta = obj_meta
                if self.root_obj_meta == None: self.root_obj_meta = obj_meta

                self.generateObject(obj_meta, dict_value)

                self.closeObject(obj_meta)
            self.closeFile()

        logConsole("Done\n")

    def processDeceleration(self, obj_name, dict_value):
        if obj_name == MetaData.DECELERATION_NAMESPACE:
            self.openNamespace(dict_value)
        elif obj_name == MetaData.DECELERATION_INCLUDE:
            inc_list = [ dict_value ] if type(dict_value) == str else dict_value
            for inc_value in inc_list: self.declared_include_list.append('"' + inc_value + '"')
        elif obj_name == MetaData.DECELERATION_MULTI_CLASS:
            self.multi_class = dict_value
        elif obj_name == MetaData.DECELERATION_MULTI_CLASS_AUTO_INSERT:
            self.multi_class_auto_insert = dict_value
        elif obj_name == MetaData.DECELERATION_IS_TLV_CLASS:
            self.is_tlv_class =  dict_value
        else:
            self.abort("%s.yaml --> unknown key: %s" % (self.yaml_fname, obj_name))

    def generateObject(self, obj_meta, dict_value):
        for param_name, param_dict in list(dict_value.items()):
            if param_name.startswith(MetaData.META_PREFIX): continue
            if ( type( param_dict ) is OrderedDict ):
                param_meta = MetaData(self.yaml_fname, param_name, param_dict)
                if param_meta.error: self.abort(param_meta.error)
                param_type = param_meta.type
                param_type_info = param_meta.type_info
                param_type_real = TypeInfo(param_type)
                if param_type_real.type == TypeInfo.ENUM:
                    try:
                        value = self.db_enum_storage_type[(self.yaml_fname, param_type_real.type_str)]
                    except:
                        value = self.db_enum_storage_type[(param_type_real.type_str, param_type_real.type_str)]
                    param_type_info = TypeInfo(value[MetaData.KEY_ENUM_STORAGE])
                    param_type_info.set_type(param_meta.type_info.type)
                    if param_meta.type == param_type_real.type_str:
                        self.include_list.append('"' + self.yaml_path + "/" + param_type + ".h" + '"')

            else:
                param_type = param_dict
                param_meta = None
                param_type_info = TypeInfo(param_type)

            obj_meta.children_types[param_name] = param_type_info

            if param_type != None:
                if param_type_info.type == TypeInfo.ERROR: self.abort("%s.yaml --> bad type: %s" % (self.yaml_fname, param_type))
                if len(self.declared_include_list) == 0:
                    if (param_type_info.type == TypeInfo.ENUM or
                        param_type_info.type == TypeInfo.STRUCT or
                        param_type_info.type == TypeInfo.CLASS or
                        param_type_info.type == TypeInfo.ENUM_CLASS):
                        if (self.db_yaml_paths.__contains__(param_type)):
                            self.include_list.append('"' + self.db_yaml_paths[param_type] + "/" + param_type + ".h" + '"')
                        else:
                            self.include_list.append('"' + self.yaml_path + "/" + param_type + ".h" + '"')

            if obj_meta.type == MetaData.TYPE_ENUM or obj_meta.type == MetaData.TYPE_ENUM_CLASS:
                if param_meta: self.abort("%s.yaml --> metadata not supported on enum values" % self.yaml_fname)
                try: self.insertLineH(obj_meta.name, self.CODE_ENUM_INSERT, "%s = %s," % (param_name, hex(param_type)) )
                except: self.abort("%s.yaml --> bad param_type=%s" % (self.yaml_fname, str(param_type)) )
            elif obj_meta.type == MetaData.TYPE_STRUCT:
                self.addStructParam(obj_meta, param_name, param_type, param_type_info, param_meta)
            elif obj_meta.type == MetaData.TYPE_CLASS:
                if obj_meta.bit_field: self.abort("%s.yaml --> _bit_field not allowed in class " % self.yaml_fname)
                self.addClassParam(obj_meta, param_name, param_type, param_type_info, param_meta)
            else:
                self.abort("%s.yaml --> unknown obj_meta.type:%s" % (self.yaml_fname, obj_meta.type) )

            if obj_meta.type == MetaData.TYPE_ENUM_CLASS:
                self.insertLineH(obj_meta.name, self.CODE_ENUM_VALIDATION_INSERT, "case %s:" % (hex(param_type)))


    def addStructParam(self, obj_meta, param_name, param_type, param_type_info, param_meta):
        bit_field_type = None
        swap_func_lines = []
        if param_meta == None:
            line = "%s %s;" % (param_type, param_name)
            if param_type_info.swap_needed:
                t_name = ("&" if not param_type_info.swap_is_func else "") + param_name + ("." if param_type_info.swap_is_func else "")
                swap_func_lines.append("%s%s%s;" % (param_type_info.swap_prefix, t_name, param_type_info.swap_suffix))
            if TypeInfo(param_type).type == TypeInfo.STRUCT:
	            self.insertLineH(obj_meta.name, self.CODE_STRUCT_INIT_FUNC_INSERT, "%s.%s;" %  (param_name, TypeInfo.STRUCT_INIT_FUNCTION_NAME) )
        else:
            if param_meta.value_const != None: self.abort("%s.yaml --> value_const not supported in struct" % self.yaml_fname)
            if obj_meta.bit_field:
                if len(param_meta.bit_range) != 2: self.abort("%s.yaml --> bit_range error for: %s" % (self.yaml_fname, param_name) )
                bits = param_meta.bit_range[0] - param_meta.bit_range[1] + 1
                bit_field_type = param_meta.bit_field_enum if param_meta.bit_field_enum != None else obj_meta.bit_field
                if param_meta.value != None: self.insertLineH(obj_meta.name, self.CODE_STRUCT_INIT_FUNC_INSERT, "%s = %s;" %  (param_name, MetaData.getFormattedValue(param_meta.value)) )
                line = "%s %s : %d;" % (bit_field_type, param_name, bits)
            else:
                bit_field_type = None
                if param_meta.type == None: self.abort("%s.yaml --> _type not defined, param_name=%s" % (self.yaml_fname, param_name) )
                if (param_meta.length_type == MetaData.LENGTH_TYPE_INT or
                    param_meta.length_type == MetaData.LENGTH_TYPE_CONST):
                    line = "%s %s[%s];" % (param_meta.type, param_meta.name, param_meta.length)
                    if TypeInfo(param_meta.type).type == TypeInfo.STRUCT:
                        t_name = "(%s[i]).%s" % (param_name, TypeInfo.STRUCT_INIT_FUNCTION_NAME)
                        self.insertLineH(obj_meta.name, self.CODE_STRUCT_INIT_FUNC_INSERT, "%sfor (size_t i = 0; i < %s; i++) {" %  (self.getIndentation(1), str(param_meta.length)))
                        self.insertLineH(obj_meta.name, self.CODE_STRUCT_INIT_FUNC_INSERT, "%s%s;" %  (self.getIndentation(2), t_name))
                        self.insertLineH(obj_meta.name, self.CODE_STRUCT_INIT_FUNC_INSERT, "%s}" %  (self.getIndentation(1)))
                    if param_meta.value != None:
                        t_name = "%s[i]" % (param_name)
                        self.insertLineH(obj_meta.name, self.CODE_STRUCT_INIT_FUNC_INSERT, "%sfor (size_t i = 0; i < %s; i++) {" %  (self.getIndentation(1), str(param_meta.length)))
                        self.insertLineH(obj_meta.name, self.CODE_STRUCT_INIT_FUNC_INSERT, "%s%s = %s;" %  (self.getIndentation(2), t_name, MetaData.getFormattedValue(param_meta.value)))
                        self.insertLineH(obj_meta.name, self.CODE_STRUCT_INIT_FUNC_INSERT, "%s}" %  (self.getIndentation(1)))

                    if param_type_info.swap_needed:
                        t_name = "%s(%s[i])%s" % (("&" if not param_type_info.swap_is_func else ""), param_name, ("." if param_type_info.swap_is_func else ""))
                        swap_func_lines.append( "for (size_t i = 0; i < %s; i++){" % (str(param_meta.length)) )
                        swap_func_lines.append( "%s%s%s%s;" % (self.getIndentation(1), param_type_info.swap_prefix, t_name, param_type_info.swap_suffix))
                        swap_func_lines.append( "}")
                    
                    
                elif (param_meta.length_type == MetaData.LENGTH_TYPE_DYNAMIC or
                      param_meta.length_type == MetaData.LENGTH_TYPE_VAR):
                    line = "%s* %s; //TLVF_TODO: not supported yet" % (param_meta.type, param_meta.name)
                else:
                    line = "%s %s;" % (param_meta.type, param_meta.name)
                    if param_type_info.swap_needed:
                        t_name = ("&" if not param_type_info.swap_is_func else "") + param_name + ("." if param_type_info.swap_is_func else "")
                        swap_func_lines.append("%s%s%s;" % (param_type_info.swap_prefix, t_name, param_type_info.swap_suffix))
                    if param_meta.value != None: 
                        self.insertLineH(obj_meta.name, self.CODE_STRUCT_INIT_FUNC_INSERT, "%s = %s;" %  (param_name, MetaData.getFormattedValue(param_meta.value) ) )
                self.insertLineH(obj_meta.name, self.CODE_STRUCT_INSERT, self.getCommentLines(param_meta.comment))

        # add defult value
        if param_meta != None and param_meta.value != None:
            lines_h = []
            if param_meta.length:
                if ( param_meta.length_type == MetaData.LENGTH_TYPE_INT or
                     param_meta.length_type == MetaData.LENGTH_TYPE_CONST ):
                    lines_h.append( "for (size_t i = 0; i < %s; i++){" % str(param_meta.length) )
                    lines_h.append( "%s%s[i] = %s;" %  (self.getIndentation(1), param_name, MetaData.getFormattedValue(param_meta.value)) )
                    lines_h.append( "}")
                else:
                    self.abort("%s.yaml --> seeting a value to a dynamic length array is not supported." % self.yaml_fname)
            else:
                lines_h.append("%s = %s;" %  (param_name, MetaData.getFormattedValue(param_meta.value)) )

        if len(swap_func_lines) > 0: self.insertLineH(obj_meta.name, self.CODE_STRUCT_SWAP_FUNC_INSERT, swap_func_lines)
        if bit_field_type:
            self.insertLineH(obj_meta.name, self.CODE_STRUCT_INSERT, line, True)
            self.insertLineH(obj_meta.name, self.CODE_STRUCT_BITFIELD_INSERT, line)
        else:
            self.insertLineH(obj_meta.name, self.CODE_STRUCT_INSERT, line)

    ##########################################################################################
    # Empty class member initialization support
    # 
    # In order to make sure that a class that contains a class member is fully
    # initialized, the virtual method isPostInitSucceeded is overridden to
    # validate that and return true or false to indicate.
    #
    # For each TLV that contains a member of a type class, a private bool member
    # is created (for each class member) which indicates if that member class is
    # initialized. This bool variable is initialized to FALSE.
    #
    # When the user calls the add_member method, the value of the bool is set to TRUE.
    # In those cases, the method isPostInitSucceeded is overridden to include
    # more conditions to validate that each of the members of the type class
    # in TLV is fully initialized.
    #
    ##########################################################################################
    def overrideIsPostInitSucceeded(self, obj_meta, param_name,lines_h):
        #for each TLV member of type class add a bool variable to indicate if it is initialized 
        lines_h.append("bool m_%s_init = false;" % (param_name))
        lines_cpp = []
        #if this is the first member of type class override the method 
        if not obj_meta.marker_post_init_method_added:
            #Add decleration in header file
            line_h = "bool isPostInitSucceeded();" 
            self.insertLineH(obj_meta.name, self.CODE_CLASS_PUBLIC_FUNC_INSERT, line_h)
                    
            #Override isPostInitSucceeded method to verify the class is completly initialized
            lines_cpp.append("bool %s::isPostInitSucceeded() {" % (obj_meta.name))
            #if this member is not added by the user return false
            lines_cpp.append("%sif (!m_%s_init) {" % (self.getIndentation(1) ,param_name))
            lines_cpp.append("%sTLVF_LOG(ERROR) << \"%s is not initialized\";" %(self.getIndentation(2), param_name))
            lines_cpp.append("%sreturn false;" % (self.getIndentation(2)))
            lines_cpp.append("%s}" % (self.getIndentation(1)))

            #in case there is more than one member of type class, add a marker 
            #before the return statement
            marker = "%s_%s" %(self.CODE_CLASS_IS_INIT_FUNC, obj_meta.name)
            lines_cpp.append(marker)
            lines_cpp.append("%sreturn true; " % self.getIndentation(1))
            lines_cpp.append("}")
            lines_cpp.append("")
            obj_meta.marker_post_init_method_added= True
            self.insertLineCpp(obj_meta.name, self.CODE_CLASS_PUBLIC_FUNC_INSERT, lines_cpp)

        #if the method is already overridden, just add the right condition
        else:
            #add a condition to validate the user added this member      
            lines_cpp.append("%sif (!m_%s_init) {" % (self.getIndentation(1) ,param_name))
            lines_cpp.append("%sTLVF_LOG(ERROR) << \"%s is not initialized\";" %(self.getIndentation(2), param_name))
            lines_cpp.append("%sreturn false;" % (self.getIndentation(2)))
            lines_cpp.append("%s}" % (self.getIndentation(1)))
            self.insertLineCpp(obj_meta.name, self.CODE_CLASS_IS_INIT_FUNC, lines_cpp)

    def addClassParam(self, obj_meta, param_name, param_type, param_type_info, param_meta=None):
        param_val_const = None
        param_val = None
        param_comment_line = []
        param_length_type = None
        param_length = None
        param_length_var = False
        param_class_const = False
        swap_func_lines = []
        inc_name = '"' + self.yaml_path + "/"  + param_type + '.h"'
        if inc_name in self.local_include_list and param_type_info.type != TypeInfo.CLASS and self.yaml_fname == self.root_obj_meta.name:
            param_type_full = (obj_meta.name + "::" + param_type) 
        else:
            is_local = False
            param_type_full = param_type

        if param_meta:
            if param_meta.type == None: self.abort("%s.yaml --> _type not defined, param_name=%s" % (self.yaml_fname, param_name) )
            if param_meta.type == MetaData.TYPE_STRUCT: self.abort("%s.yaml --> struct definition not suported inside class, param_name=%s" % (self.yaml_fname, param_name) )
            if param_meta.value != None and param_meta.value_const != None: self.abort("%s.yaml --> _value and _value_const are not supported together, param_name=%s" % (self.yaml_fname, param_name) )
            if param_meta.value_const != None:
                [param_val_const, err] = MetaData.getConstValue(obj_meta.name, param_meta.value_const)
                if err: self.abort("%s.yaml --> error in val_const=%s" % (self.yaml_fname, param_meta.value_const) )
            if param_meta.value != None: param_val = MetaData.getFormattedValue(param_meta.value)
            if param_meta.comment: param_comment_line = self.getCommentLines(param_meta.comment)
            param_length_type = param_meta.length_type
            param_length = param_meta.length
            param_length_var = param_meta.length_var
            param_type = param_meta.type
            param_class_const = param_meta.class_const

            param_has_dynamic_length = (param_length_type == MetaData.LENGTH_TYPE_DYNAMIC)
            if self.class_last_param_has_dynamic_length and param_has_dynamic_length: self.abort("%s.yaml --> only last parameter in class can have dynamic length, param_name=%s" % (self.yaml_fname, param_name) )
            self.class_last_param_has_dynamic_length = param_has_dynamic_length

        is_var_len = (param_length_type == MetaData.LENGTH_TYPE_VAR)
        is_int_len = (param_length_type == MetaData.LENGTH_TYPE_INT)
        is_const_len = (param_length_type == MetaData.LENGTH_TYPE_CONST)
        is_dynamic_len = (param_length_type == MetaData.LENGTH_TYPE_DYNAMIC)

        ##############################################################################################
        # variable length list support
        #
        # If the yaml file contains variable length lists, we already added allocation functions
        # (alloc for lists of simple types, create and add for lists of classes).
        # In each allocation function we added a call to std::memmove which copies the buffer after
        # the list by the allocation size, and added a marker - which is used here to update
        # all the class member pointers which come after the list member.
        ##############################################################################################
        for (marker, func) in obj_meta.alloc_list:
            if func == "add":
                length_str = "len - ptr->get_initial_size()"
            else:
                length_str = "len"
            line = "%sm_%s = (%s *)((uint8_t *)(m_%s) + %s);" %(self.getIndentation(1), param_name, param_type, param_name, length_str)
            self.insertLineCpp("", marker.strip(), line)

        lines_h = []
        lines_cpp = []
        if param_length_type == None:
            if TypeInfo(param_type).type == TypeInfo.CLASS:                
                lines_h.append("%s *m_%s = nullptr;" % (param_type, param_name))
                lines_h.append("std::shared_ptr<%s> m_%s_ptr = nullptr;" % (param_type, param_name))
                self.overrideIsPostInitSucceeded(obj_meta,param_name,lines_h)
                
                if not obj_meta.lock_allocation_member_added:
                    lines_h.append("bool m_%s__ = false;" % self.MEMBER_LOCK_ALLOCATION)
                    obj_meta.lock_allocation_member_added = True

                if not obj_meta.lock_order_member_added:
                    lines_h.append("int m_%s__ = 0;" % self.MEMBER_LOCK_ORDER_COUNTER)
                    obj_meta.lock_order_member_added = True
                self.insertLineH(obj_meta.name, self.CODE_CLASS_PRIVATE_VARS_INSERT, lines_h)
                lines_h = []

                if not param_meta:
                    param_dict = OrderedDict({"_type" : "class" })
                    param_meta = MetaData(self.yaml_fname, param_name, param_dict)
                
                param_meta.list_index = obj_meta.list_index
                param_meta.length_type = MetaData.LENGTH_TYPE_VAR
                obj_meta.list_index = obj_meta.list_index + 1

                # Add param to swap list
                swap_func_lines = ["if (m_%s_ptr) { m_%s_ptr->class_swap(); }" %(param_name, param_name)]

                # Add allocation methods
                self.addClassVarLenMethods(obj_meta, param_type, param_name, param_meta, param_length, False, False)

                # Add function to return pointer
                lines_h.append("std::shared_ptr<%s> %s() { return m_%s_ptr; }" % (param_type, param_name, param_name))

                # Add param handling to init function
                lines_cpp.append("m_%s = (%s*)m_%s__;" % ( param_name, param_type, self.MEMBER_BUFF_PTR))
                lines_cpp.append("if (m_%s__) {" %(self.MEMBER_PARSE))
                lines_cpp.append("%sauto %s = create_%s();" %( self.getIndentation(1), param_name, param_name))
                lines_cpp.append("%sif (!%s) {" %(self.getIndentation(1), param_name))
                lines_cpp.append("%sTLVF_LOG(ERROR) << \"create_%s() failed\";" %(self.getIndentation(2), param_name))
                lines_cpp.append("%sreturn false;" %(self.getIndentation(2)))
                lines_cpp.append("%s}" %(self.getIndentation(1)))
                lines_cpp.append("%sif (!add_%s(%s)) {" %(self.getIndentation(1), param_name, param_name))
                lines_cpp.append("%sTLVF_LOG(ERROR) << \"add_%s() failed\";" %(self.getIndentation(2), param_name))
                lines_cpp.append("%sreturn false;" %(self.getIndentation(2)))
                lines_cpp.append("%s}" %(self.getIndentation(1)))
                lines_cpp.append("%s// swap back since %s will be swapped as part of the whole class swap" %(self.getIndentation(1), param_name))
                lines_cpp.append("%s%s->class_swap();" %(self.getIndentation(1), param_name))
                lines_cpp.append("}")
                self.insertLineCpp(obj_meta.name, self.CODE_CLASS_INIT_FUNC_INSERT, lines_cpp)
                lines_cpp = []

            else:
                # add private pointer
                line = "%s* m_%s = nullptr;" % (param_type, param_name)
                self.insertLineH(obj_meta.name, self.CODE_CLASS_PRIVATE_VARS_INSERT, line)

                lines_h = []
                lines_cpp = []


                if param_class_const:
                    [ret_const_val, ret_err] = MetaData.getConstValue(param_name, param_val_const)
                    if ret_err: self.abort("%s.yaml --> can't get const val for param_name=%s" % (self.yaml_fname, param_name) )
                    lines_h.append( "static %s get_%s(){" % (param_type, param_name) )
                    lines_h.append( "%sreturn (%s)(%s);" % (self.getIndentation(1), param_type, ret_const_val) )
                    lines_h.append( "}" )
                else:
                    # add default value to init func
                    lines_cpp.append("m_%s = (%s*)m_%s__;" % ( param_name, param_type, self.MEMBER_BUFF_PTR))
                    if obj_meta.is_tlv_class and param_name == MetaData.TLV_TYPE_LENGTH:
                        lines_cpp.append("if (!m_%s__) *m_%s = 0;" % (self.MEMBER_PARSE, param_name) )
                    elif param_val_const != None:
                        if (param_type_info.type == TypeInfo.ENUM or param_type_info.type == TypeInfo.ENUM_CLASS):
                            param_val_const = param_type + "::" + param_val_const
                        lines_cpp.append("if (!m_%s__) *m_%s = %s;" % ( self.MEMBER_PARSE, param_name, param_val_const) )
                    elif param_val != None: lines_cpp.append("if (!m_%s__) *m_%s = %s;" % ( self.MEMBER_PARSE, param_name, param_val) )
                    elif param_length_var: lines_cpp.append("if (!m_%s__) *m_%s = 0;" % ( self.MEMBER_PARSE, param_name) )
                    lines_cpp.append("%sif (!buffPtrIncrementSafe(sizeof(%s))) {" % (self.getIndentation(0), param_type) )
                    lines_cpp.append("%sLOG(ERROR) << \"buffPtrIncrementSafe(\" << std::dec << sizeof(%s) << \") Failed!\";" % (self.getIndentation(1), param_type))
                    lines_cpp.append("%sreturn false;" %(self.getIndentation(1)))
                    lines_cpp.append("%s}" %(self.getIndentation(0)))

                    if obj_meta.is_tlv_class and param_name != MetaData.TLV_TYPE_TYPE and param_name != MetaData.TLV_TYPE_LENGTH:
                        lines_cpp.append( "if(m_length && !m_%s__){ (*m_length) += sizeof(%s); }" % ( self.MEMBER_PARSE, param_type) )

                    if TypeInfo(param_type).type == TypeInfo.STRUCT:
                            lines_cpp.append("if (!m_%s__) { m_%s->struct_init(); }" % (self.MEMBER_PARSE, param_name))
                    self.insertLineCpp(obj_meta.name, self.CODE_CLASS_INIT_FUNC_INSERT, lines_cpp)
                    
                    # Add type checking
                    lines_cpp = []
                    if obj_meta.is_tlv_class and param_name == MetaData.TLV_TYPE_TYPE and param_val_const:
                        lines_cpp.append("if (m_%s__) {" % (self.MEMBER_PARSE))
                        lines_cpp.append( "%sif (*m_type != %s) {" % ( self.getIndentation(1), param_val_const ) )
                        lines_cpp.append( '%sTLVF_LOG(ERROR) << "TLV type mismatch. Expected value: " << int(%s) << ", received value: " << int(*m_type);' %  (self.getIndentation(2), param_val_const) )
                        lines_cpp.append( "%sreturn false;" % self.getIndentation(2))
                        lines_cpp.append( "%s}" % self.getIndentation(1) )
                        lines_cpp.append( "}" )
                        self.insertLineCpp(obj_meta.name, self.CODE_CLASS_INIT_FUNC_SWAP_INSERT, lines_cpp)

                    lines_h   = []
                    lines_cpp = []
                    
                    # add class size calculation
                    if TypeInfo(param_type).type == TypeInfo.CLASS:
                        lines_cpp.append( "class_size += %s::get_initial_size(); // %s" % ( param_type, param_name) )
                    else:
                        lines_cpp.append( "class_size += sizeof(%s); // %s" % ( param_type, param_name) )
                    self.insertLineCpp(obj_meta.name, self.CODE_CLASS_SIZE_FUNC_INSERT, lines_cpp)
    
                    lines_h   = []
                    lines_cpp = []

                    # add comment
                    lines_h.extend(param_comment_line)

                    # add function to return reference
                    const = "const " if param_val_const != None or obj_meta.is_tlv_class and param_name == MetaData.TLV_TYPE_LENGTH else ""
                    lines_h.append( "%s%s& %s();" % (const, param_type, param_name) ) #const
                    lines_cpp.append( "%s%s& %s::%s() {" % (const, param_type_full, obj_meta.name, param_name) )
                    lines_cpp.append( "%sreturn (%s%s&)(*m_%s);" % (self.getIndentation(1), const, param_type, param_name) )
                    lines_cpp.append( "}" )
                    lines_cpp.append( "" )

                # add var to swap list
                if param_type_info.swap_needed:
                    t_name = ("m_%s->" % param_name) if param_type_info.swap_is_func else ("m_%s" % param_name)
                    swap_func_lines.append( "%s%s%s;" % (param_type_info.swap_prefix, t_name, param_type_info.swap_suffix) )

        elif ( is_int_len or is_const_len or is_var_len or is_dynamic_len ):

            # add private pointer
            self.include_list.append("<tuple>")
            var_lines = ["%s* m_%s = nullptr;" % (param_type, param_name),
                         "size_t m_%s_idx__ = 0;" % (param_name) ]
            if (is_var_len or is_dynamic_len ) and TypeInfo(param_type).type == TypeInfo.CLASS:
                self.include_list.append("<vector>")
                var_lines.append("std::vector<std::shared_ptr<%s>> m_%s_vector;" % (param_type, param_name))
                if not obj_meta.lock_allocation_member_added:
                    var_lines.append("bool m_%s__ = false;" % self.MEMBER_LOCK_ALLOCATION)
                    obj_meta.lock_allocation_member_added = True
            if not obj_meta.lock_order_member_added:
                var_lines.append("int m_%s__ = 0;" % self.MEMBER_LOCK_ORDER_COUNTER)
                obj_meta.lock_order_member_added = True
            self.insertLineH(obj_meta.name, self.CODE_CLASS_PRIVATE_VARS_INSERT, var_lines)

            lines_h = []
            lines_cpp = []

            # add default value to init func
            lines_cpp.append("m_%s = (%s*)m_%s__;" % (param_name, param_type, self.MEMBER_BUFF_PTR) )
            if is_dynamic_len:
                if obj_meta.is_tlv_class:
                    lines_cpp.append("if (m_length && m_%s__) {" % self.MEMBER_PARSE)
                    lines_cpp.append("%ssize_t len = *m_length;" % (self.getIndentation(1)))
                    lines_cpp.append("%stlvf_swap(16, reinterpret_cast<uint8_t*>(&len));" % (self.getIndentation(1)))
                    lines_cpp.append("%slen -= (m_%s__ - sizeof(*m_type) - sizeof(*m_length) - m_%s__);" % (self.getIndentation(1), self.MEMBER_BUFF_PTR, self.MEMBER_BUFF))
                    if TypeInfo(param_type).type == TypeInfo.CLASS:
                        lines_cpp.append("%swhile (len > 0) {" % (self.getIndentation(1)))
                        lines_cpp.append("%sif (len < %s::get_initial_size()) {" %(self.getIndentation(2), param_type))
                        lines_cpp.append("%sTLVF_LOG(ERROR) << \"Invalid length (%s)\";" %(self.getIndentation(3), param_name))
                        lines_cpp.append("%sreturn false;" %(self.getIndentation(3)))
                        lines_cpp.append("%s}" %(self.getIndentation(2)))
                        lines_cpp.append("%sauto %s = create_%s();" %( self.getIndentation(2), param_name, param_name))
                        lines_cpp.append("%sif (!%s) {" %(self.getIndentation(2), param_name))
                        lines_cpp.append("%sTLVF_LOG(ERROR) << \"create_%s() failed\";" %(self.getIndentation(3), param_name))
                        lines_cpp.append("%sreturn false;" %(self.getIndentation(3)))
                        lines_cpp.append("%s}" %(self.getIndentation(2)))
                        lines_cpp.append("%sif (!add_%s(%s)) {" %(self.getIndentation(2), param_name, param_name))
                        lines_cpp.append("%sTLVF_LOG(ERROR) << \"add_%s() failed\";" %(self.getIndentation(3), param_name))
                        lines_cpp.append("%sreturn false;" %(self.getIndentation(3)))
                        lines_cpp.append("%s}" %(self.getIndentation(2)))
                        lines_cpp.append("%s// swap back since %s will be swapped as part of the whole class swap" %(self.getIndentation(2), param_name))
                        lines_cpp.append("%s%s->class_swap();" %(self.getIndentation(2), param_name))
                        lines_cpp.append("%slen -= %s->getLen();" %(self.getIndentation(2), param_name))
                        lines_cpp.append("%s}" %(self.getIndentation(1)))
                    else:
                        lines_cpp.append("%sm_%s_idx__ = len/sizeof(%s);" % (self.getIndentation(1), param_name, param_type))
                        lines_cpp.append("%sif (!buffPtrIncrementSafe(len)) {" % (self.getIndentation(1)) )
                        lines_cpp.append("%sLOG(ERROR) << \"buffPtrIncrementSafe(\" << std::dec << len << \") Failed!\";" % (self.getIndentation(2)))
                        lines_cpp.append("%sreturn false;" %(self.getIndentation(2)))
                        lines_cpp.append("%s}" %(self.getIndentation(1)))
                    lines_cpp.append("}")
                else:
                    lines_cpp.append("m_%s_idx__ = getBuffRemainingBytes();" % param_name)
            if is_var_len or is_dynamic_len:
                param_meta.list_index = obj_meta.list_index
                obj_meta.list_index = obj_meta.list_index + 1
            if is_var_len:
                # variable length list support - swap length value for calculating list size
                length_type = obj_meta.children_types[param_length]
                lines_cpp.append("%s %s = *m_%s;" %(length_type.type_str, param_length, param_length))
                if length_type.swap_needed:
                    lines_cpp.append("if (m_%s__) {  %s&%s%s; }" %(self.MEMBER_PARSE, length_type.swap_prefix, param_length, length_type.swap_suffix))
                if TypeInfo(param_type).type == TypeInfo.CLASS:
                    lines_cpp.append("m_%s_idx__ = 0;" % (param_name))
                    lines_cpp.append("for (size_t i = 0; i < %s; i++) {" % (param_length))
                    # Add param handling to init function
                    lines_cpp.append("%sauto %s = create_%s();" %( self.getIndentation(1), param_name, param_name))
                    lines_cpp.append("%sif (!%s) {" %(self.getIndentation(1), param_name))
                    lines_cpp.append("%sTLVF_LOG(ERROR) << \"create_%s() failed\";" %(self.getIndentation(2), param_name))
                    lines_cpp.append("%sreturn false;" %(self.getIndentation(2)))
                    lines_cpp.append("%s}" %(self.getIndentation(1)))
                    lines_cpp.append("%sif (!add_%s(%s)) {" %(self.getIndentation(1), param_name, param_name))
                    lines_cpp.append("%sTLVF_LOG(ERROR) << \"add_%s() failed\";" %(self.getIndentation(2), param_name))
                    lines_cpp.append("%sreturn false;" %(self.getIndentation(2)))
                    lines_cpp.append("%s}" %(self.getIndentation(1)))
                    lines_cpp.append("%s// swap back since %s will be swapped as part of the whole class swap" %(self.getIndentation(1), param_name))
                    lines_cpp.append("%s%s->class_swap();" %(self.getIndentation(1), param_name))
                    lines_cpp.append("}")
                else:
                    lines_cpp.append("m_%s_idx__ = %s;" % (param_name, param_length))
                    lines_cpp.append("%sif (!buffPtrIncrementSafe(sizeof(%s) * (%s))) {" % (self.getIndentation(0), param_type, param_length))
                    lines_cpp.append("%sLOG(ERROR) << \"buffPtrIncrementSafe(\" << std::dec << sizeof(%s) * (%s) << \") Failed!\";" % (self.getIndentation(1), param_type,param_length))
                    lines_cpp.append("%sreturn false;" %(self.getIndentation(1)))
                    lines_cpp.append("%s}" %(self.getIndentation(0)))
            if is_int_len or is_const_len:
                lines_cpp.append("%sif (!buffPtrIncrementSafe(sizeof(%s) * (%s))) {" % (self.getIndentation(0), param_type, param_length))
                lines_cpp.append("%sLOG(ERROR) << \"buffPtrIncrementSafe(\" << std::dec << sizeof(%s) * (%s) << \") Failed!\";" % (self.getIndentation(1), param_type,param_length))
                lines_cpp.append("%sreturn false;" %(self.getIndentation(1)))
                lines_cpp.append("%s}" %(self.getIndentation(0)))
                lines_cpp.append("m_%s_idx__  = %s;" % (param_name, param_length))
                if obj_meta.is_tlv_class or TypeInfo(param_type).type == TypeInfo.STRUCT:
                    lines_cpp.append("if (!m_parse__) {")
                if obj_meta.is_tlv_class:
                    lines_cpp.append( "%sif (m_length) { (*m_length) += (sizeof(%s) * %s); }" % (self.getIndentation(1), param_type, param_length) )
                if TypeInfo(param_type).type == TypeInfo.STRUCT:
                    lines_cpp.append("%sfor (size_t i = 0; i < %s; i++) { m_%s->struct_init(); }" % (self.getIndentation(1), param_length, param_name))
                elif (param_val_const or param_val):
                    if not param_type_info.is_std_type: self.abort("%s.yaml --> only std types are allowed in val / val_const" % self.yaml_fname)
                    lines_cpp += ["%sfor (size_t i = 0; i < %s; i++){" % (self.getIndentation(1), str(param_length)),
                                 "%sm_%s[i] = %s;"% (self.getIndentation(2), param_name, str(param_val) if param_val != None else str(param_val_const)),
                                 "%s}" % (self.getIndentation(1))]
                if obj_meta.is_tlv_class or TypeInfo(param_type).type == TypeInfo.STRUCT:
                    lines_cpp.append("}")

            if (param_val_const or param_val):
                if not (is_int_len or is_const_len):
                    self.abort("%s.yaml --> val / val_const are not supported for variable length types" % self.yaml_fname)

            self.insertLineCpp(obj_meta.name, self.CODE_CLASS_INIT_FUNC_INSERT, lines_cpp)

            # add var to swap list
            if param_type_info.swap_needed:
                if (param_type_info.type == TypeInfo.CLASS):
                    t_name = ("&" if not param_type_info.swap_is_func else "") + ("std::get<1>(%s(i))" % param_name) + ("." if param_type_info.swap_is_func else "")
                else:
                    t_name = ("&" if not param_type_info.swap_is_func else "") + ("m_%s[i]" % param_name) + ("." if param_type_info.swap_is_func else "")
                if is_dynamic_len: t_length = ("m_" + param_name + "_idx__")
                elif is_var_len: t_length = ("(size_t)*m_" + param_meta.length) 
                else: t_length = str(param_meta.length)
                swap_func_lines.append( "for (size_t i = 0; i < %s; i++){" % (t_length) )
                swap_func_lines.append( "%s%s%s%s;" % (self.getIndentation(1), param_type_info.swap_prefix, t_name, param_type_info.swap_suffix))
                swap_func_lines.append( "}")

            lines_h = []
            lines_cpp = []

            # add class size calculations
            if is_int_len or is_const_len:
                lines_cpp.append( "class_size += %s * sizeof(%s); // %s" % (param_length, param_type, param_name) )
                self.insertLineCpp(obj_meta.name, self.CODE_CLASS_SIZE_FUNC_INSERT, lines_cpp)

            lines_h   = []
            lines_cpp = []

            # lines_h.append( "//TLVF_TODO: Debug: param_name=%s, param_length_type=%r, param_length=%r" % (param_name, param_length_type, param_length) )

            # add comment
            lines_h.extend(param_comment_line)
            if is_dynamic_len:
                if param_type_info.type == TypeInfo.CLASS:
                    lines_h.append("size_t %s_length();" %(param_name))
                    lines_cpp.append("size_t %s::%s_length()" %(obj_meta.name, param_name))
                    lines_cpp.append("{")
                    lines_cpp.append("%ssize_t len = 0;" %(self.getIndentation(1)))
                    lines_cpp.append("%sfor (size_t i = 0; i < m_%s_idx__; i++) {" %(self.getIndentation(1), param_name))
                    lines_cpp.append("%slen += m_%s_vector[i]->getLen();" %(self.getIndentation(2), param_name))
                    lines_cpp.append("%s}" %(self.getIndentation(1)))
                    lines_cpp.append("%sreturn len;" %(self.getIndentation(1)))
                    lines_cpp.append("}")
                    lines_cpp.append("")
                else:
                    lines_h.append("size_t %s_length() { return m_%s_idx__ * sizeof(%s); }" % (param_name, param_name, param_type_full))

            if param_type_info.type == TypeInfo.CHAR:
                self.include_list.append("<string.h>")
                self.include_list.append('<tlvf/tlvfutils.h>')

                #add function to get std::string                
                lines_h.append( "std::string %s_str();" % (param_name) )
                lines_cpp.append( "std::string %s::%s_str() {" % (obj_meta.name, param_name) )
                lines_cpp.append( "%schar *%s_ = %s();" % (self.getIndentation(1), param_name, param_name) )
                lines_cpp.append( "%sif (!%s_) { return std::string(); }" % (self.getIndentation(1), param_name) )
                lines_cpp.append( "%sreturn std::string(%s_, m_%s_idx__);" % (self.getIndentation(1), param_name, param_name) )
                lines_cpp.append( "}" )
                lines_cpp.append( "" )

                #add function to get char*
                lines_h.append( "%s* %s(size_t length = 0);" % (param_type, param_name) )
                lines_cpp.append( "%s* %s::%s(size_t length) {" % (param_type_full, obj_meta.name, param_name) )
                lines_cpp.append( "%sif( (m_%s_idx__ == 0) || (m_%s_idx__ < length) ) {" % (self.getIndentation(1), param_name, param_name) )
                lines_cpp.append( '%sTLVF_LOG(ERROR) << "%s length is smaller than requested length";' %  (self.getIndentation(2), param_name) )
                lines_cpp.append( "%sreturn nullptr;" % self.getIndentation(2))
                lines_cpp.append( "%s}" % self.getIndentation(1) )
                if param_meta.length_max:
                    lines_cpp.append( "%sif (m_%s_idx__ > %s )  {" % (self.getIndentation(1), param_name, param_meta.length_max) )
                    lines_cpp.append( '%sTLVF_LOG(ERROR) << "Invalid length -  " << m_%s_idx__ << " elements (max length is " << %s << ")";' % ( self.getIndentation(2), param_name, param_meta.length_max ))
                    lines_cpp.append( "%sreturn nullptr;" % self.getIndentation(2))
                    lines_cpp.append( "%s}" % self.getIndentation(1) )
                lines_cpp.append( "%sreturn ((%s*)m_%s);" % (self.getIndentation(1), param_type, param_name) )
                lines_cpp.append( "}" )
                lines_cpp.append( "" )

                lines_h.append( "bool set_%s(const std::string& str);" % (param_name) )
                lines_cpp.append( "bool %s::set_%s(const std::string& str) { return set_%s(str.c_str(), str.size()); }" % (obj_meta.name, param_name, param_name) )

                lines_h.append( "bool set_%s(const char buffer[], size_t size);" % (param_name) )
                lines_cpp.append( "bool %s::set_%s(const char str[], size_t size) {" % (obj_meta.name, param_name) )
                lines_cpp.append( "%sif (str == nullptr) {" % self.getIndentation(1))
                lines_cpp.append( '%sTLVF_LOG(WARNING) << "set_%s received a null pointer.";' %  (self.getIndentation(2), param_name) )
                lines_cpp.append( "%sreturn false;" % self.getIndentation(2))
                lines_cpp.append( "%s}" % self.getIndentation(1) )
                if is_const_len or is_int_len:
                    lines_cpp.append( "%sif (size > %s) {"  % (self.getIndentation(1), param_length))
                    lines_cpp.append( '%sTLVF_LOG(ERROR) << "Received buffer size is smaller than string length";' %  self.getIndentation(2) )
                    lines_cpp.append( "%sreturn false;" % self.getIndentation(2))
                    lines_cpp.append( "%s}" % self.getIndentation(1) )
                else:
                    lines_cpp.append( "%sif (!alloc_%s(size)) { return false; }"  % (self.getIndentation(1), param_name))
                lines_cpp.append( "%sstd::copy(str, str + size, m_%s);" % (self.getIndentation(1), param_name))
                lines_cpp.append( "%sreturn true;" % self.getIndentation(1))
                lines_cpp.append( "}")

            elif param_type_info.is_std_type:
                lines_h.append( "%s* %s(size_t idx = 0);" % (param_type, param_name) )
                lines_cpp.append( "%s* %s::%s(size_t idx) {" % (param_type_full, obj_meta.name, param_name) )
                lines_cpp.append( "%sif ( (m_%s_idx__ == 0) || (m_%s_idx__ <= idx) ) {" % (self.getIndentation(1), param_name, param_name) )
                lines_cpp.append( '%sTLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";' %  self.getIndentation(2) )
                lines_cpp.append( '%sreturn nullptr;' %  self.getIndentation(2) )
                lines_cpp.append( "%s}" % self.getIndentation(1) )
                lines_cpp.append( "%sreturn &(m_%s[idx]);" % (self.getIndentation(1), param_name) )
                lines_cpp.append( "}" )
                lines_cpp.append( "" )
                
                if param_type_info.type == TypeInfo.UINT8:
                    lines_h.append( "bool set_%s(const void* buffer, size_t size);" % (param_name) )
                    lines_cpp.append( "bool %s::set_%s(const void* buffer, size_t size) {" % (obj_meta.name, param_name) )
                    lines_cpp.append( "%sif (buffer == nullptr) {" % self.getIndentation(1))
                    lines_cpp.append( '%sTLVF_LOG(WARNING) << "set_%s received a null pointer.";' %  (self.getIndentation(2), param_name) )
                    lines_cpp.append( "%sreturn false;" % self.getIndentation(2))
                    lines_cpp.append( "%s}" % self.getIndentation(1) )
                    if is_const_len or is_int_len:
                        lines_cpp.append( "%sif (size > %s) {"  % (self.getIndentation(1), param_length))
                        lines_cpp.append( '%sTLVF_LOG(ERROR) << "Received buffer size is smaller than buffer length";' %  self.getIndentation(2) )
                        lines_cpp.append( "%sreturn false;" % self.getIndentation(2))
                        lines_cpp.append( "%s}" % self.getIndentation(1) )
                    else:
                        lines_cpp.append( "%sif (!alloc_%s(size)) { return false; }"  % (self.getIndentation(1), param_name))
                    lines_cpp.append( "%sstd::copy_n(reinterpret_cast<const uint8_t *>(buffer), size, m_%s);" % (self.getIndentation(1), param_name))
                    lines_cpp.append( "%sreturn true;" % self.getIndentation(1))
                    lines_cpp.append( "}")

            else:
                #add function to get reference
                lines_h.append( "std::tuple<bool, %s&> %s(size_t idx);" % (param_type, param_name) )
                lines_cpp.append( "std::tuple<bool, %s&> %s::%s(size_t idx) {" % (param_type_full, obj_meta.name, param_name) )
                lines_cpp.append( "%sbool ret_success = ( (m_%s_idx__ > 0) && (m_%s_idx__ > idx) );" % (self.getIndentation(1), param_name, param_name) )
                lines_cpp.append( "%ssize_t ret_idx = ret_success ? idx : 0;" % (self.getIndentation(1)) )
                lines_cpp.append( "%sif (!ret_success) {" % (self.getIndentation(1)) )
                lines_cpp.append( '%sTLVF_LOG(ERROR) << "Requested index is greater than the number of available entries";' %  self.getIndentation(2) )
                lines_cpp.append( "%s}" % self.getIndentation(1) )
                if TypeInfo(param_type).type == TypeInfo.CLASS: #TODO:only if it's the last member of the class
                    lines_cpp.append( "%sreturn std::forward_as_tuple(ret_success, *(m_%s_vector[ret_idx]));" % (self.getIndentation(1), param_name) )
                else:
                    lines_cpp.append( "%sreturn std::forward_as_tuple(ret_success, m_%s[ret_idx]);" % (self.getIndentation(1), param_name) )
                lines_cpp.append( "}" )
                lines_cpp.append( "" )
            
            self.insertLineH(obj_meta.name, self.CODE_CLASS_PUBLIC_FUNC_INSERT, lines_h)
            self.insertLineCpp(obj_meta.name, self.CODE_CLASS_PUBLIC_FUNC_INSERT, lines_cpp)
            lines_cpp = []
            lines_h = []

            #add function to allocate memory
            if is_var_len or is_dynamic_len:
                self.addClassVarLenMethods(obj_meta, param_type, param_name, param_meta, param_length, is_var_len, is_dynamic_len)
        else:
            self.abort("%s.yaml --> unsupported length type: %r, param_name=%s" % (self.yaml_fname, param_length_type, param_name))

        if len(swap_func_lines) > 0: self.insertLineCpp(obj_meta.name, self.CODE_CLASS_SWAP_FUNC_INSERT, swap_func_lines)
        self.insertLineH(obj_meta.name, self.CODE_CLASS_PUBLIC_FUNC_INSERT, lines_h)
        self.insertLineCpp(obj_meta.name, self.CODE_CLASS_PUBLIC_FUNC_INSERT, lines_cpp)

    def addClassVarLenMethods(self, obj_meta, param_type, param_name, param_meta, param_length, is_var_len, is_dynamic_len):
        lines_cpp = []
        lines_h = []
        if TypeInfo(param_type).type == TypeInfo.CLASS:
            lines_h.append( "std::shared_ptr<%s> create_%s();" % (param_type, param_name) ) #TODO: maybe change to 'create' and add '_entry' postfix
            lines_cpp.append( "std::shared_ptr<%s> %s::create_%s() {" % (param_type, obj_meta.name, param_name) )
            lines_cpp.append( "%sif (m_%s__ > %s) {" %(self.getIndentation(1), self.MEMBER_LOCK_ORDER_COUNTER, param_meta.list_index) )
            lines_cpp.append( "%sTLVF_LOG(ERROR) << \"Out of order allocation for variable length list %s, abort!\";" %(self.getIndentation(2), param_name) )
            lines_cpp.append( "%sreturn nullptr;" %self.getIndentation(2))
            lines_cpp.append( "%s}" %self.getIndentation(1))
            lines_cpp.append( "%ssize_t len = %s::get_initial_size();" % (self.getIndentation(1), param_type) )
            lines_cpp.append( "%sif (m_%s__ || getBuffRemainingBytes() < len) {" % (self.getIndentation(1), self.MEMBER_LOCK_ALLOCATION) )
            lines_cpp.append( '%sTLVF_LOG(ERROR) << "Not enough available space on buffer";' %  self.getIndentation(2) )
            lines_cpp.append( "%sreturn nullptr;" % self.getIndentation(2))
            lines_cpp.append( "%s}" % self.getIndentation(1) )
            lines_cpp.append( "%sm_%s__ = %s;" %(self.getIndentation(1), self.MEMBER_LOCK_ORDER_COUNTER, param_meta.list_index) )
            lines_cpp.append( "%sm_%s__ = true;" % (self.getIndentation(1), self.MEMBER_LOCK_ALLOCATION) )
            lines_cpp.extend(self.addAllocationMarkersCreate(obj_meta, param_meta, param_length, True)) # Variable length lists support
            if is_dynamic_len:
                lines_cpp.append( "%sreturn std::make_shared<%s>(getBuffPtr(), getBuffRemainingBytes(), m_%s__);" % (self.getIndentation(1), param_type, self.MEMBER_PARSE) )
            else:
                lines_cpp.append( "%sreturn std::make_shared<%s>(src, getBuffRemainingBytes(src), m_%s__);" % (self.getIndentation(1), param_type, self.MEMBER_PARSE) )
            lines_cpp.append( "}" )
            lines_cpp.append( "" )

            lines_h.append( "bool add_%s(std::shared_ptr<%s> ptr);" % (param_name, param_type) ) #TODO: maybe change to add and end '_entry' postfix
            lines_cpp.append( "bool %s::add_%s(std::shared_ptr<%s> ptr) {" % (obj_meta.name, param_name, param_type) )

            lines_cpp.append( "%sif (ptr == nullptr) {" % self.getIndentation(1) )
            lines_cpp.append( '%sTLVF_LOG(ERROR) << "Received entry is nullptr";' %  self.getIndentation(2) )
            lines_cpp.append( "%sreturn false;" % self.getIndentation(2))
            lines_cpp.append( "%s}" % self.getIndentation(1) )

            lines_cpp.append( "%sif (m_%s__ == false) {" % (self.getIndentation(1), self.MEMBER_LOCK_ALLOCATION) )
            lines_cpp.append( '%sTLVF_LOG(ERROR) << "No call to create_%s was called before add_%s";' %  (self.getIndentation(2), param_name, param_name) )
            lines_cpp.append( "%sreturn false;" % self.getIndentation(2))
            lines_cpp.append( "%s}" % self.getIndentation(1) )

            lines_cpp.append("%suint8_t *src = (uint8_t *)m_%s;" % (self.getIndentation(1), param_meta.name))
            if param_length != None:
                lines_cpp.append("%sif (m_%s_idx__ > 0) {" % (self.getIndentation(1), param_name))
                ptr = "(uint8_t *)m_%s_vector[m_%s_idx__ - 1]->getBuffPtr()" %(param_meta.name,param_name)
                lines_cpp.append("%ssrc = %s;" %(self.getIndentation(2), ptr))                    
                lines_cpp.append("%s}" %self.getIndentation(1))

            lines_cpp.append( "%sif (ptr->getStartBuffPtr() != src) {" % (self.getIndentation(1)) )
            lines_cpp.append( '%sTLVF_LOG(ERROR) << "Received entry pointer is different than expected (expecting the same pointer returned from add method)";' %  self.getIndentation(2) )
            lines_cpp.append( "%sreturn false;" % self.getIndentation(2))
            lines_cpp.append( "%s}" % self.getIndentation(1) )

            lines_cpp.append( "%sif (ptr->getLen() > getBuffRemainingBytes(ptr->getStartBuffPtr())) {;" % self.getIndentation(1) )
            lines_cpp.append( '%sTLVF_LOG(ERROR) << "Not enough available space on buffer";' %  self.getIndentation(2) )
            lines_cpp.append( "%sreturn false;" % self.getIndentation(2))
            lines_cpp.append( "%s}" % self.getIndentation(1) )

            if param_length or is_dynamic_len:
                lines_cpp.append( "%sm_%s_idx__++;" % (self.getIndentation(1), param_name) )
                if is_var_len:
                    lines_cpp.append( "%sif (!m_parse__) { (*m_%s)++; }" % (self.getIndentation(1), param_length) )
            else: 
                # When the user calls the add_member method, the value of the bool variable is set to TRUE.
                # For more information, please read overrideIsPostInitSucceeded method documentation 
                lines_cpp.append("%sm_%s_init = true;" %(self.getIndentation(1),param_meta.name))
            lines_cpp.append( "%ssize_t len = ptr->getLen();" % (self.getIndentation(1) ))
            lines_cpp.extend(self.addAllocationMarkersAdd(obj_meta, param_meta, param_length, False)) # Variable length lists support
            if param_length or is_dynamic_len:
                lines_cpp.append( "%sm_%s_vector.push_back(ptr);" % (self.getIndentation(1), param_name ))
            else:
                lines_cpp.append( "%sm_%s_ptr = ptr;" % (self.getIndentation(1), param_name ))
            lines_cpp.append("%sif (!buffPtrIncrementSafe(len)) {" % (self.getIndentation(1)) )
            lines_cpp.append("%sLOG(ERROR) << \"buffPtrIncrementSafe(\" << std::dec << len << \") Failed!\";" % (self.getIndentation(2)))
            lines_cpp.append("%sreturn false;" %(self.getIndentation(2)))
            lines_cpp.append("%s}" %(self.getIndentation(1)))
            if obj_meta.is_tlv_class:
                lines_cpp.append( "%sif(!m_parse__ && m_length){ (*m_length) += len; }" % (self.getIndentation(1)))
            lines_cpp.append( "%sm_%s__ = false;" % (self.getIndentation(1), self.MEMBER_LOCK_ALLOCATION))
            lines_cpp.append( "%sreturn true;" % (self.getIndentation(1)) )
            lines_cpp.append( "}" )
            lines_cpp.append( "" )
        else: #simple list
            lines_h.append( "bool alloc_%s(size_t count = 1);" % (param_name) )
            lines_cpp.append( "bool %s::alloc_%s(size_t count) {" % (obj_meta.name, param_name) )
            lines_cpp.append( "%sif (m_%s__ > %s) {;" %(self.getIndentation(1), self.MEMBER_LOCK_ORDER_COUNTER, param_meta.list_index) )
            lines_cpp.append( "%sTLVF_LOG(ERROR) << \"Out of order allocation for variable length list %s, abort!\";" %(self.getIndentation(2), param_name) )
            lines_cpp.append( "%sreturn false;" %self.getIndentation(2))
            lines_cpp.append( "%s}" %self.getIndentation(1))
            lines_cpp.append( "%ssize_t len = sizeof(%s) * count;" % (self.getIndentation(1), param_type) )
            lines_cpp.append( "%sif(getBuffRemainingBytes() < len )  {" % (self.getIndentation(1)) )
            lines_cpp.append( '%sTLVF_LOG(ERROR) << "Not enough available space on buffer - can\'t allocate";' %  self.getIndentation(2) )
            lines_cpp.append( "%sreturn false;" % self.getIndentation(2))
            lines_cpp.append( "%s}" % self.getIndentation(1) )
            if param_meta.length_max:
                lines_cpp.append( "%sif (count > %s )  {" % (self.getIndentation(1), param_meta.length_max) )
                lines_cpp.append( '%sTLVF_LOG(ERROR) << "Can\'t allocate " << count << " elements (max length is " << %s << ")";' % ( self.getIndentation(2), param_meta.length_max ))
                lines_cpp.append( "%sreturn false;" % self.getIndentation(2))
                lines_cpp.append( "%s}" % self.getIndentation(1) )
            lines_cpp.append( "%sm_%s__ = %s;" %(self.getIndentation(1), self.MEMBER_LOCK_ORDER_COUNTER, param_meta.list_index) )
            lines_cpp.extend( self.addAllocationMarkersAlloc(obj_meta, param_meta, param_length, True) ) # Variable length lists support
            lines_cpp.append( "%sm_%s_idx__ += count;" % (self.getIndentation(1), param_name) )
            if is_var_len:
                lines_cpp.append( "%s*m_%s += count;" % (self.getIndentation(1), param_length) )
            lines_cpp.append("%sif (!buffPtrIncrementSafe(len)) {" % (self.getIndentation(1)) )
            lines_cpp.append("%sLOG(ERROR) << \"buffPtrIncrementSafe(\" << std::dec << len << \") Failed!\";" % (self.getIndentation(2)))
            lines_cpp.append("%sreturn false;" %(self.getIndentation(2)))
            lines_cpp.append("%s}" %(self.getIndentation(1)))
            if obj_meta.is_tlv_class:
                lines_cpp.append( "%sif(m_length){ (*m_length) += len; }" % (self.getIndentation(1)))
            if TypeInfo(param_type).type == TypeInfo.STRUCT:
                lines_cpp.append("%sif (!m_%s__) { " % (self.getIndentation(1), self.MEMBER_PARSE))
                lines_cpp.append("%sfor (size_t i = m_%s_idx__ - count; i < m_%s_idx__; i++) { m_%s[i].struct_init(); }" % (self.getIndentation(2), param_name, param_name, param_name))
                lines_cpp.append("%s}" % (self.getIndentation(1)))

            lines_cpp.append( "%sreturn true;" % (self.getIndentation(1)) )
            lines_cpp.append( "}" )
            lines_cpp.append( "" )
        self.insertLineH(obj_meta.name, self.CODE_CLASS_PUBLIC_FUNC_INSERT, lines_h)
        self.insertLineCpp(obj_meta.name, self.CODE_CLASS_PUBLIC_FUNC_INSERT, lines_cpp)


    #########################################################################
    # variable length list support
    #
    # If this is a variable length list alloc, add a call to std::copy_n to
    # move the contents of the buffer starting at the current list entry pointer
    # till the new list pointer (ptr + allocation size). The number of bytes we
    # move is the the calculated as END of buffer - current list pointer.
    # Then, we need to add a marker so that on following members we will
    # update their pointers accordingly.
    #
    # Note - dynamic "unknown" length lists are lists without a size parameter
    # (list[]) which can only be the last member of each class, therefore no
    # action is needed for these lists.
    ##########################################################################
    def addAllocationMarkersCreate(self, obj_meta, param_meta, param_length, memmove):
        lines_cpp = []
        if param_meta.length_type == MetaData.LENGTH_TYPE_VAR or param_meta.length_type == MetaData.LENGTH_TYPE_DYNAMIC:
            if memmove:
                lines_cpp.append("%suint8_t *src = (uint8_t *)m_%s;" % (self.getIndentation(1), param_meta.name))
                # Since list of classes does not have length parameter like list of native type, 
                # taking the buffer pointer from classes vector last element  
                if param_length != None:
                    lines_cpp.append("%sif (m_%s_idx__ > 0) {" % (self.getIndentation(1), param_meta.name))
                    ptr = "(uint8_t *)m_%s_vector[m_%s_idx__ - 1]->getBuffPtr()" %(param_meta.name,param_meta.name)
                    lines_cpp.append("%ssrc = %s;" %(self.getIndentation(2), ptr))                    
                    lines_cpp.append("%s}" %self.getIndentation(1))

                lines_cpp.append("%sif (!m_parse__) {" %self.getIndentation(1))
                lines_cpp.append("%suint8_t *dst = src + len;" %(self.getIndentation(2)))
                lines_cpp.append("%ssize_t move_length = getBuffRemainingBytes(src) - len;" %self.getIndentation(2))
                lines_cpp.append("%sstd::copy_n(src, move_length, dst);" %self.getIndentation(2))
                lines_cpp.append("%s}" %self.getIndentation(1))

            marker = "%s_%s_%s_%s" %(self.CODE_CLASS_ALLOC_INSERT, obj_meta.name, "create", param_meta.name)
            obj_meta.alloc_list.append((marker, "create"))
            lines_cpp.append(marker)

        return lines_cpp

    def addAllocationMarkersAlloc(self, obj_meta, param_meta, param_length, memmove):
        lines_cpp = []
        if param_meta.length_type == MetaData.LENGTH_TYPE_VAR or MetaData.LENGTH_TYPE_DYNAMIC:
            if memmove:
                if param_meta.length_type == MetaData.LENGTH_TYPE_VAR:
                    lines_cpp.append("%suint8_t *src = (uint8_t *)&m_%s[*m_%s];" % (self.getIndentation(1), param_meta.name, param_length))
                else:
                    lines_cpp.append("%suint8_t *src = (uint8_t *)m_%s;" % (self.getIndentation(1), param_meta.name))
                lines_cpp.append("%suint8_t *dst = src + len;" %(self.getIndentation(1)))
                lines_cpp.append("%sif (!m_parse__) {" %self.getIndentation(1))                
                lines_cpp.append("%ssize_t move_length = getBuffRemainingBytes(src) - len;" %self.getIndentation(2))
                lines_cpp.append("%sstd::copy_n(src, move_length, dst);" %self.getIndentation(2))
                lines_cpp.append("%s}" %self.getIndentation(1))

            marker = "%s_%s_%s_%s" %(self.CODE_CLASS_ALLOC_INSERT, obj_meta.name, "alloc", param_meta.name)
            obj_meta.alloc_list.append((marker, "alloc"))
            lines_cpp.append(marker)

        return lines_cpp

    def addAllocationMarkersAdd(self, obj_meta, param_meta, param_length, memmove):
        lines_cpp = []
        if param_meta.length_type == MetaData.LENGTH_TYPE_VAR:
            marker = "%s_%s_%s_%s" %(self.CODE_CLASS_ALLOC_INSERT, obj_meta.name, "add", param_meta.name)
            obj_meta.alloc_list.append((marker, "add"))
            lines_cpp.append(marker)
            
        return lines_cpp

    def getCommentLines(self, comment):
        ret = []
        if comment:
            for c in comment.split("\n"):
                c = c.replace("\n","")
                if len(c) > 0: ret.append("//" + c)
        return ret

    def appendLineH(self, line):
        self.code_template_h.append(line)
    
    def appendLineCpp(self, line):
        self.code_template_cpp.append(line)

    def insertLineH(self, name, marker, line, below=False):
        if name != "": marker = marker +  "_" + name
        self.insertLine__(self.code_template_h, marker, line, below)

    def insertLineCpp(self, name, marker, line, below=False):
        if name != "": marker = marker +  "_" + name
        self.insertLine__(self.code_template_cpp, marker, line, below)

    def insertLine__(self, template, marker, line, below):
        for idx in range(0,len(template)):
            if marker == template[idx].strip():
                m = template[idx]
                idx_ident = len(m) - len(m.lstrip())
                idx_insert = (idx + 1) if below else (idx)
                lines = line if type(line) == list else [line]
                for l in lines:
                    template.insert(idx_insert, m[:idx_ident] + l)
                    idx_insert+=1
                return
        self.abort("%s.yaml --> insertLine, can't find marker %s in code template" % (self.yaml_fname, marker) )

    def hasMarker(self, template, marker):
        for idx in range(0,len(template)):
            if marker in template[idx]:
                return True
        return False

    def openFile(self, filename):
        self.yaml_fname = os.path.splitext(os.path.basename(filename))[0]
        self.yaml_path = os.path.relpath(os.path.dirname(filename), self.yaml_root_path)
        self.logger.debug("openFile: %s" % (self.yaml_fname) )
        self.root_obj_meta = None
        self.is_root_obj = True
        self.hasClass = False
        self.multi_class = False
        self.multi_class_auto_insert = None
        self.is_tlv_class = False
        self.code_template_h = []
        self.code_template_cpp = []
        self.include_list = []
        self.local_include_list = []
        self.declared_include_list = []
        self.local_obj_list = []
        self.namespace = None
        self.class_last_param_has_dynamic_length = False

        self.appendLineH("#ifndef _%s_%s_H_" % (self.yaml_path.upper().replace('/', '_'), self.yaml_fname.upper()))
        self.appendLineH("#define _%s_%s_H_" % (self.yaml_path.upper().replace('/', '_'), self.yaml_fname.upper()))
        self.appendLineH("")
        self.appendLineH(self.CODE_INCLUDE_INSERT)
        self.appendLineH(self.CODE_NAMESPACE_INSERT)
        self.appendLineH(self.CODE_FORWARD_DECLARATION_INSERT)
        self.appendLineH(self.CODE_START_INSERT)
        self.appendLineH(self.CODE_END_INSERT)

    def closeFile(self):
        self.closeNamespace()
        self.appendLineH("")
        self.appendLineH("#endif //_%s_%s_H_" % (self.yaml_path.upper(), self.yaml_fname.upper()))
        self.writeFile(self.code_template_h, ".h")
        self.writeFile(self.code_template_cpp, ".cpp")

    def openNamespace(self, namespace):
        if self.namespace:
            self.abort("%s.yaml --> Nested namespaces are not supported" % self.yaml_fname)
        else:
            self.insertLineH("", self.CODE_NAMESPACE_INSERT, "")
            self.insertLineH("", self.CODE_NAMESPACE_INSERT, "namespace %s {" % namespace)
            self.insertLineH("", self.CODE_NAMESPACE_INSERT, "")
            self.namespace = namespace

    def closeNamespace(self):
        if self.namespace:
            self.appendLineH("")
            self.appendLineH("}; // close namespace: %s" % self.namespace)

    def addInitialCode(self, obj_meta):
        if self.is_root_obj:
            self.include_list.append("<cstddef>")
            self.include_list.append("<stdint.h>")
            self.include_list.append('<tlvf/swap.h>')
        if not self.hasClass and obj_meta.type == MetaData.TYPE_CLASS:
            self.include_list.append('<string.h>')
            self.include_list.append('<memory>')
            self.include_list.append('<tlvf/BaseClass.h>')
            self.include_list.append('<tlvf/ClassList.h>')

            self.appendLineCpp('#include <%s/%s.h>' % (self.yaml_path, self.yaml_fname) )
            self.appendLineCpp('#include <tlvf/tlvflogging.h>')
            self.appendLineCpp(self.CODE_INCLUDE_INSERT)
            self.appendLineCpp("")
            if self.namespace:
                self.appendLineCpp("using namespace %s;" % self.namespace)
                self.appendLineCpp("")
            self.appendLineCpp(self.CODE_END_INSERT)
            self.appendLineCpp("")
            self.hasClass = True

    def openObject(self, obj_meta, dict_value, root_obj_meta):
        self.logger.debug("Object: is_root=%r, obj_meta=%s" % (self.is_root_obj, obj_meta) )
        if (root_obj_meta == None):
            root_obj_meta = self.root_obj_meta

        name = obj_meta.name

        self.addInitialCode(obj_meta)
        if self.is_root_obj:
            insert_name   = ""
            insert_marker = self.CODE_END_INSERT
        
        elif self.multi_class:
            insert_name   = ""
            insert_marker = self.CODE_END_INSERT
            
        elif root_obj_meta.type == MetaData.TYPE_CLASS:
            insert_name = name
            if obj_meta.type == MetaData.TYPE_CLASS and obj_meta.name == root_obj_meta.name:
                insert_marker = self.CODE_CLASS_PUBLIC_VARS_END_INSERT + "_" + root_obj_meta.name
                self.insertLineH(root_obj_meta.name, self.CODE_CLASS_PUBLIC_VARS_END_INSERT, insert_marker + "_" + insert_name)
            elif obj_meta.type == MetaData.TYPE_CLASS:
                self.insertLineH("", self.CODE_FORWARD_DECLARATION_INSERT, "class %s;" % obj_meta.name)
                insert_name   = ""
                insert_marker = self.CODE_END_INSERT
            else:
                insert_marker = self.CODE_CLASS_PUBLIC_VARS_INSERT + "_" + root_obj_meta.name
                self.insertLineH(root_obj_meta.name, self.CODE_CLASS_PUBLIC_VARS_INSERT, insert_marker + "_" + insert_name)
        else:
            insert_name   = ""
            insert_marker = self.CODE_END_INSERT

        if obj_meta.type == MetaData.TYPE_CLASS:
            self.addClassCode(insert_name, insert_marker, name, obj_meta)
            obj_meta.lock_order_member_added = False

            if self.multi_class_auto_insert != None:
                for param_name, param_dict in self.multi_class_auto_insert.items():
                    param_meta = MetaData(self.yaml_fname, param_name, param_dict)
                    if param_meta.error: self.abort(param_meta.error)
                    self.addClassParam(obj_meta, param_name, param_meta.type, param_meta.type_info, param_meta)

        elif obj_meta.type == MetaData.TYPE_STRUCT:
            self.addStructCode(insert_name, insert_marker, name)
            if obj_meta.bit_field:
                self.include_list.append("<asm/byteorder.h>")
                self.insertLineH(insert_name, self.CODE_STRUCT_INSERT, "#if defined(__LITTLE_ENDIAN_BITFIELD)")
                self.insertLineH(insert_name, self.CODE_STRUCT_BITFIELD_INSERT, "#elif defined(__BIG_ENDIAN_BITFIELD)")
                self.insertLineH(insert_name, self.CODE_STRUCT_REVERSED_BITFIELD_INSERT, "#else")
                self.insertLineH(insert_name, self.CODE_STRUCT_REVERSED_BITFIELD_INSERT, "#error \"Bitfield macros are not defined\"")
                self.insertLineH(insert_name, self.CODE_STRUCT_REVERSED_BITFIELD_INSERT, "#endif")
        elif obj_meta.type == MetaData.TYPE_ENUM:
            self.addEnumCode(insert_name, insert_marker, name, obj_meta.enum_storage)
        elif obj_meta.type == MetaData.TYPE_ENUM_CLASS:
            self.addEnumClassCode(insert_name, insert_marker, name, obj_meta.enum_storage)
        else:
            self.abort("%s.yaml --> error in _type=%s" % (self.yaml_fname, dict_value))
    
    def addClassCode(self, insert_name, insert_marker, name, obj_meta):
        #class start
        self.insertLineH(insert_name, insert_marker, "")
        self.insertLineH(insert_name, insert_marker,"%sclass %s : public BaseClass" % (self.getIndentation(0), name))
        self.insertLineH(insert_name, insert_marker, "{" )
        self.insertLineH(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(1), self.CODE_CLASS_START, name) )
        self.insertLineH(insert_name, insert_marker, "%spublic:" % self.getIndentation(1))

        self.addClassConstructor(insert_name, insert_marker, name)
        self.addClassPublicMembers(insert_name, insert_marker, name)
        self.addClassSwapMethod(insert_name, insert_marker, name)
        self.addClassFinalizeMethod(insert_name, insert_marker, name, obj_meta)
        self.addClassSizeMethod(insert_name, insert_marker, name)

        self.insertLineH(insert_name, insert_marker, "" )
        self.insertLineH(insert_name, insert_marker, "%sprivate:" % self.getIndentation(1))

        self.addClassInitMethod(insert_name, insert_marker, name)
        self.addClassPrivateMembers(insert_name, insert_marker, name)
        #class end
        self.insertLineH(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(1), self.CODE_CLASS_END, name))
        self.insertLineH(insert_name, insert_marker, "};")

    def addStructCode(self, insert_name, insert_marker, name):
        self.insertLineH(insert_name, insert_marker, "typedef struct %s {" % (name))
        self.insertLineH(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(1), self.CODE_STRUCT_INSERT, name))
        self.insertLineH(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(1), self.CODE_STRUCT_BITFIELD_INSERT, name))
        self.insertLineH(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(1), self.CODE_STRUCT_REVERSED_BITFIELD_INSERT, name))

        self.insertLineH(insert_name, insert_marker, "%svoid %s{" % (self.getIndentation(1), TypeInfo.STRUCT_SWAP_FUNCTION_NAME) )
        self.insertLineH(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(2), self.CODE_STRUCT_SWAP_FUNC_INSERT, name))
        self.insertLineH(insert_name, insert_marker, "%s}" % (self.getIndentation(1)) )

        self.insertLineH(insert_name, insert_marker, "%svoid %s{" % (self.getIndentation(1), TypeInfo.STRUCT_INIT_FUNCTION_NAME) )
        self.insertLineH(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(2), self.CODE_STRUCT_INIT_FUNC_INSERT, name))
        self.insertLineH(insert_name, insert_marker, "%s}" % (self.getIndentation(1)) )

        self.insertLineH(insert_name, insert_marker, "} __attribute__((packed)) %s;" % (name))
        self.insertLineH(insert_name, insert_marker, "")

    def addEnumCode(self, insert_name, insert_marker, name, enum_storage):
        storage_str = "" if enum_storage == None else (": %s" % enum_storage)
        self.insertLineH(insert_name, insert_marker, "%senum %s%s {" % (self.getIndentation(0), name, storage_str))
        self.insertLineH(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(1), self.CODE_ENUM_INSERT, name))
        self.insertLineH(insert_name, insert_marker, "%s};" % self.getIndentation(0))
        self.insertLineH(insert_name, insert_marker, "")

    def addEnumClassCode(self, insert_name, insert_marker, name, enum_storage):
        if enum_storage == None: self.abort("%s.yaml error: enum class type must be specified")
        self.insertLineH(insert_name, insert_marker, "%senum class %s : %s {" % (self.getIndentation(0), name, enum_storage))
        self.insertLineH(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(1), self.CODE_ENUM_INSERT, name))
        self.insertLineH(insert_name, insert_marker, "%s};" % self.getIndentation(0))
        self.insertLineH(insert_name, insert_marker, "%sclass %sValidate {" % (self.getIndentation(0), name))
        self.insertLineH(insert_name, insert_marker, "%spublic:" % (self.getIndentation(0)))
        self.insertLineH(insert_name, insert_marker, "%sstatic bool check(%s value) {" % (self.getIndentation(1), enum_storage))
        self.insertLineH(insert_name, insert_marker, "%sbool ret = false;" % (self.getIndentation(2)))
        self.insertLineH(insert_name, insert_marker, "%sswitch (value) {" % (self.getIndentation(2)))
        self.insertLineH(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(2), self.CODE_ENUM_VALIDATION_INSERT, name))
        self.insertLineH(insert_name, insert_marker, "%sret = true;" % (self.getIndentation(4)))
        self.insertLineH(insert_name, insert_marker, "%sbreak;" % (self.getIndentation(4)))
        self.insertLineH(insert_name, insert_marker, "%sdefault:" % (self.getIndentation(3)))
        self.insertLineH(insert_name, insert_marker, "%sret = false;" % (self.getIndentation(4)))
        self.insertLineH(insert_name, insert_marker, "%sbreak;" % (self.getIndentation(4)))
        self.insertLineH(insert_name, insert_marker, "%s}" % self.getIndentation(2))
        self.insertLineH(insert_name, insert_marker, "%sreturn ret;" % self.getIndentation(2))
        self.insertLineH(insert_name, insert_marker, "%s}" % self.getIndentation(1))
        self.insertLineH(insert_name, insert_marker, "%s};" % self.getIndentation(0))
        self.insertLineH(insert_name, insert_marker, "")



    def addClassConstructor(self, insert_name, insert_marker, name):
        self.insertLineH(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(2), self.CODE_CLASS_CONSTRACTOR, name))
        self.insertLineH(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(2), self.CODE_CLASS_DISTRACTOR, name))
        self.insertLineH(insert_name, insert_marker, "" )

        self.insertLineCpp(insert_name, insert_marker, "%s_%s" % (self.CODE_CLASS_CONSTRACTOR, name))
        self.insertLineCpp(insert_name, insert_marker, "%s_%s" % (self.CODE_CLASS_DISTRACTOR, name))

    def addClassPublicMembers(self, insert_name, insert_marker, name):
        self.insertLineH(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(2), self.CODE_CLASS_PUBLIC_VARS_INSERT, name))
        self.insertLineH(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(2), self.CODE_CLASS_PUBLIC_VARS_END_INSERT, name))
        self.insertLineH(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(2), self.CODE_CLASS_PUBLIC_FUNC_INSERT, name))

        self.insertLineCpp(insert_name, insert_marker, "%s_%s" % (self.CODE_CLASS_PUBLIC_FUNC_INSERT, name))

    def addClassPrivateMembers(self, insert_name, insert_marker, name):
        self.insertLineH(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(2), self.CODE_CLASS_PRIVATE_FUNC_INSERT, name))
        self.insertLineH(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(2), self.CODE_CLASS_PRIVATE_VARS_INSERT, name))

        self.insertLineCpp(insert_name, insert_marker, "%s_%s" % (self.CODE_CLASS_PRIVATE_FUNC_INSERT, name))

    def addClassInitMethod(self, insert_name, insert_marker, name):
        self.insertLineH(insert_name, insert_marker, "%sbool init();" % self.getIndentation(2))

        self.insertLineCpp(insert_name, insert_marker, "bool %s::init()" % (name))
        self.insertLineCpp(insert_name, insert_marker, "{")
        self.insertLineCpp(insert_name, insert_marker, "%sif (getBuffRemainingBytes() < get_initial_size()) {" % (self.getIndentation(1)))
        self.insertLineCpp(insert_name, insert_marker, '%sTLVF_LOG(ERROR) << "Not enough available space on buffer. Class init failed";' %  self.getIndentation(2) )
        self.insertLineCpp(insert_name, insert_marker, "%sreturn false;" % self.getIndentation(2))
        self.insertLineCpp(insert_name, insert_marker, "%s}" % self.getIndentation(1) )
        self.insertLineCpp(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(1), self.CODE_CLASS_INIT_FUNC_INSERT, name))
        self.insertLineCpp(insert_name, insert_marker, "%sif (m_%s__) { class_swap(); }" % (self.getIndentation(1), self.MEMBER_PARSE))
        self.insertLineCpp(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(1), self.CODE_CLASS_INIT_FUNC_SWAP_INSERT, name))
        self.insertLineCpp(insert_name, insert_marker, "%sreturn true;" % (self.getIndentation(1)))
        self.insertLineCpp(insert_name, insert_marker, "}")
        self.insertLineCpp(insert_name, insert_marker, "")

    def addClassFinalizeMethod(self, insert_name, insert_marker, name, obj_meta):
        self.insertLineH(insert_name, insert_marker, "%sbool finalize() override;" % self.getIndentation(2))

        self.insertLineCpp(insert_name, insert_marker, "bool %s::finalize()" % (name))
        self.insertLineCpp(insert_name, insert_marker, "{")
        self.insertLineCpp(insert_name, insert_marker, "%sif (m_parse__) {" % (self.getIndentation(1)))
        self.insertLineCpp(insert_name, insert_marker, '%sTLVF_LOG(DEBUG) << "finalize() called but m_parse__ is set";' %  self.getIndentation(2) )
        self.insertLineCpp(insert_name, insert_marker, "%sreturn true;" % self.getIndentation(2))
        self.insertLineCpp(insert_name, insert_marker, "%s}" % self.getIndentation(1) )
        self.insertLineCpp(insert_name, insert_marker, "%sif (m_finalized__) {" % (self.getIndentation(1)))
        self.insertLineCpp(insert_name, insert_marker, '%sTLVF_LOG(DEBUG) << "finalize() called for already finalized class";' %  self.getIndentation(2) )
        self.insertLineCpp(insert_name, insert_marker, "%sreturn true;" % self.getIndentation(2))
        self.insertLineCpp(insert_name, insert_marker, "%s}" % self.getIndentation(1) )
        self.insertLineCpp(insert_name, insert_marker, "%sif (!isPostInitSucceeded()) {" % (self.getIndentation(1)))
        self.insertLineCpp(insert_name, insert_marker, '%sTLVF_LOG(ERROR) << "post init check failed";' %  self.getIndentation(2) )
        self.insertLineCpp(insert_name, insert_marker, "%sreturn false;" % self.getIndentation(2))
        self.insertLineCpp(insert_name, insert_marker, "%s}" % self.getIndentation(1) )
        self.insertLineCpp(insert_name, insert_marker, "%sif (m_inner__) {" % (self.getIndentation(1)))
        self.insertLineCpp(insert_name, insert_marker, "%sif (!m_inner__->finalize()) {" % self.getIndentation(2))
        self.insertLineCpp(insert_name, insert_marker, '%sTLVF_LOG(ERROR) << "m_inner__->finalize() failed";' %  self.getIndentation(3) )
        self.insertLineCpp(insert_name, insert_marker, "%sreturn false;" % self.getIndentation(3))
        self.insertLineCpp(insert_name, insert_marker, "%s}" % self.getIndentation(2) )
        self.insertLineCpp(insert_name, insert_marker, "%sauto tailroom = m_inner__->getMessageBuffLength() - m_inner__->getMessageLength();" % self.getIndentation(2))
        self.insertLineCpp(insert_name, insert_marker, "%sm_buff_ptr__ -= tailroom;" % (self.getIndentation(2)))
        if obj_meta.is_tlv_class:
            self.insertLineCpp(insert_name, insert_marker, "%s*m_length -= tailroom;" % (self.getIndentation(2)))
        self.insertLineCpp(insert_name, insert_marker, "%s}" % self.getIndentation(1) )
        self.insertLineCpp(insert_name, insert_marker, "%sclass_swap();" % (self.getIndentation(1)))
        self.insertLineCpp(insert_name, insert_marker, "%sm_finalized__ = true;" % (self.getIndentation(1)))
        self.insertLineCpp(insert_name, insert_marker, "%sreturn true;" % (self.getIndentation(1)))
        self.insertLineCpp(insert_name, insert_marker, "}")
        self.insertLineCpp(insert_name, insert_marker, "")

    def addClassSwapMethod(self, insert_name, insert_marker, name):
        self.insertLineH(insert_name, insert_marker, "%svoid class_swap() override;" % self.getIndentation(2))

        self.insertLineCpp(insert_name, insert_marker, "void %s::class_swap()" % (name))
        self.insertLineCpp(insert_name, insert_marker, "{")
        self.insertLineCpp(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(1), self.CODE_CLASS_SWAP_FUNC_INSERT, name))
        self.insertLineCpp(insert_name, insert_marker, "}")
        self.insertLineCpp(insert_name, insert_marker, "")
       
    def addClassSizeMethod(self, insert_name, insert_marker, name):
        self.insertLineH(insert_name, insert_marker, "%sstatic size_t get_initial_size();" % self.getIndentation(2))

        self.insertLineCpp(insert_name, insert_marker, "size_t %s::get_initial_size()" % (name))
        self.insertLineCpp(insert_name, insert_marker, "{")
        self.insertLineCpp(insert_name, insert_marker, "%ssize_t class_size = 0;" % (self.getIndentation(1)))
        self.insertLineCpp(insert_name, insert_marker, "%s%s_%s" % (self.getIndentation(1), self.CODE_CLASS_SIZE_FUNC_INSERT, name))
        self.insertLineCpp(insert_name, insert_marker, "%sreturn class_size;" % (self.getIndentation(1)))
        self.insertLineCpp(insert_name, insert_marker, "}")
        self.insertLineCpp(insert_name, insert_marker, "")

    def closeObject(self, obj_meta):
        if obj_meta.type == MetaData.TYPE_CLASS: # add class constractor
            # constractor 1
            self.insertLineH(obj_meta.name, self.CODE_CLASS_CONSTRACTOR, "%s(uint8_t* buff, size_t buff_len, bool parse = false);" % (obj_meta.name))
            self.insertLineCpp(obj_meta.name, self.CODE_CLASS_CONSTRACTOR, ("%s::%s(uint8_t* buff, size_t buff_len, bool parse) :" % (obj_meta.name, obj_meta.name)))
            self.insertLineCpp(obj_meta.name, self.CODE_CLASS_CONSTRACTOR, ("%sBaseClass(buff, buff_len, parse) {" % self.getIndentation(1) ))
            self.insertLineCpp(obj_meta.name, self.CODE_CLASS_CONSTRACTOR, "%sm_init_succeeded = init();" % self.getIndentation(1))
            self.insertLineCpp(obj_meta.name, self.CODE_CLASS_CONSTRACTOR, "}")

            # constractor 2
            self.insertLineH(obj_meta.name, self.CODE_CLASS_CONSTRACTOR, "explicit %s(std::shared_ptr<BaseClass> base, bool parse = false);" % (obj_meta.name))
            self.insertLineCpp(obj_meta.name, self.CODE_CLASS_CONSTRACTOR, ("%s::%s(std::shared_ptr<BaseClass> base, bool parse) :" % (obj_meta.name, obj_meta.name)))
            self.insertLineCpp(obj_meta.name, self.CODE_CLASS_CONSTRACTOR, "BaseClass(base->getBuffPtr(), base->getBuffRemainingBytes(), parse){" )
            self.insertLineCpp(obj_meta.name, self.CODE_CLASS_CONSTRACTOR, "%sm_init_succeeded = init();" % self.getIndentation(1))
            self.insertLineCpp(obj_meta.name, self.CODE_CLASS_CONSTRACTOR, "}")

            # distractor
            self.insertLineH(obj_meta.name, self.CODE_CLASS_DISTRACTOR,  "~%s();" % obj_meta.name)
            self.insertLineCpp(obj_meta.name, self.CODE_CLASS_DISTRACTOR,  "%s::~%s() {" % (obj_meta.name, obj_meta.name) )
            self.insertLineCpp(obj_meta.name, self.CODE_CLASS_DISTRACTOR, "}")

        self.is_root_obj = False

    def getIndentation(self, level):
        ident = ""
        for i in range(level):
            ident += self.CODE_INDENTATION
        return ident

    def mkdir_p(self, path):
        if not os.path.exists(path):
            os.makedirs(path)

    def generateCmake(self):
        logConsole("Generating Cmake files...")
        
        code = [
            'cmake_minimum_required(VERSION 2.8)',
            '',
            'set (CMAKE_CXX_STANDARD 11)',
            'set(CXXFLAGS "${CMAKE_C_FLAGS} -Werror")',
            'set(PROJET_NAME tlvf)',
            'project(${PROJET_NAME})',
            'message("${BoldWhite}Preparing ${BoldGreen}${PROJET_NAME} PKG_REV=${PKG_REV} ${BoldWhite} for target ${BoldGreen}${TARGET_PLATFORM}${BoldWhite} ${ColourReset}")',
            '',
            'set(MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})',
            ''
        ]
        
        h_dir = []
        for f in self.output_directories_h:
            if not f in h_dir: h_dir.append(f)
        cpp_dir = []
        for f in self.output_directories_cpp:
            if not f in cpp_dir: cpp_dir.append(f)

        code.append('include_directories(${MODULE_PATH}/include)')
        code.append('')

        code.append('file(GLOB_RECURSE tlvf_sources')
        for f in h_dir:
            code.append('  ${MODULE_PATH}/%s/*.h' % f )
        for f in cpp_dir:
            code.append('  ${MODULE_PATH}/%s/*.cpp' % f )
        code.append(')')
        code.append('')
        
        code += [
            'add_library(${PROJET_NAME} ${tlvf_sources})', 
            'target_link_libraries(${PROJET_NAME} dl)',
            '', 
            '# set_target_properties(${PROJET_NAME} PROPERTIES VERSION "%s" SOVERSION "%s")' % (self.CMAKE_PROPERTIES_VERSION, self.CMAKE_SO_VERSION) ,
            'set_target_properties(${PROJET_NAME} PROPERTIES LINKER_LANGUAGE CXX)',
            'set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -s")',
            'set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")',
            'set(CMAKE_CXX_STANDARD 11)', 
            'add_custom_command(TARGET tlvf POST_BUILD',
            '  COMMAND ${CMAKE_COMMAND} -E make_directory ${TLVF_PATH_LIB}',
            '  COMMAND ${CMAKE_COMMAND} -E copy ${TLVF_PATH_BUILD}/lib${PROJET_NAME}.so ${TLVF_PATH_LIB}/)'
        ]

        self.mkdir_p(self.conf_output_path)
        file_path = os.path.join(self.conf_output_path, "CMakeLists.txt")
        f= open(file_path,"w+")
        for line in code: f.write(line + "\n")
        f.close() 
        logConsole("Done\n")

    def compareFile(self, file_path, code_lines):
        if not os.path.isfile(file_path):
            return False
        f = open(file_path, 'r')

        for line in code_lines:
            if not self.conf_debug_keep_source_marker:
                if line.find("//~") != -1: continue
            tempLine = f.readline()
            if tempLine is None or line+"\n" != tempLine:
                f.close()
                return False
        
        eof = f.read()
        f.close()
        
        if eof is not None:
            return False
        return True

    def writeFile(self, code_lines, file_suffix):
        if file_suffix == ".h":
            file_path = os.path.join(self.conf_output_path_include, self.yaml_path)
            p = os.path.relpath(self.conf_output_path_include, self.conf_output_path)
            p = os.path.join(p, self.yaml_path)
            self.output_directories_h.append(p)
        else:
            if len(code_lines) == 0: return
            file_path = os.path.join(self.conf_output_path_src, self.yaml_path)
            p = os.path.relpath(self.conf_output_path_src, self.conf_output_path)
            p = os.path.join(p, self.yaml_path)
            self.output_directories_cpp.append(p)
        self.mkdir_p(file_path)
        
        file_path = os.path.join(file_path, self.yaml_fname + file_suffix)
        self.logger.debug("writing source file: %s" % file_path)
        self.generated_file_list.append(file_path)

        lines = [self.AUTO_GENERATED_MESSAGE]
        # write source_license_header
        if self.conf_source_license_header:
            license_file_path = os.path.join(self.src_path, self.conf_source_license_header)
            try: fl = open(license_file_path, 'r')
            except: self.abort("can't open file %s" % license_file_path)
            for line in fl:
                lines.append(line.rstrip())
            fl.close()
            lines.append("")
        
        # write include header protection start
        if file_suffix == ".h":
            tmp_list = []
            for inc_name in self.include_list:
                if (not (inc_name in tmp_list)) and (not (inc_name in self.local_include_list)):
                    tmp_list.append(inc_name)
                    self.insertLineH("",self.CODE_INCLUDE_INSERT, '#include %s' % inc_name)
            for inc_name in self.declared_include_list:
                if (not (inc_name in tmp_list)):
                    tmp_list.append(inc_name)
                    self.insertLineH("",self.CODE_INCLUDE_INSERT, '#include %s' % inc_name)

        lines.extend(code_lines)
        
        # write code
        if not self.compareFile(file_path, lines):
            f = open(file_path, "w+")
            for line in lines:
                if not self.conf_debug_keep_source_marker:
                    if line.find("//~") != -1: continue
                f.write(line + "\n")
            f.close()


    def logObject(self, obj, name=""):    
        self.logger.debug("=======%s=========" % name)
        pp = pprint.PrettyPrinter(indent=4)
        self.logger.debug( pp.pformat(obj) )
        self.logger.debug("================")

    def loadAllYamlFilesToDB(self):
        logConsole("Loading Yaml files...")
        for fname in self.yaml_file_list:
            yaml_fname = os.path.basename( fname )
            prefix = os.path.commonprefix([os.path.dirname(fname), self.yaml_root_path])
            yaml_path = os.path.relpath(os.path.dirname(fname), prefix)
            yaml_inst = self.loadYaml(fname)
            self.dumpYaml(yaml_inst)
            self.loadYamlToDB(yaml_fname, yaml_inst, yaml_path)
        logConsole("Done\n")
    
    def loadYamlToDB(self, yaml_fname, yaml_inst, yaml_path):
        yaml_fname = yaml_fname.replace(".yaml","")
        first_item=True
        for key, value in yaml_inst.items():
            if key.startswith(MetaData.META_PREFIX):
                pass
            elif first_item:
                first_item=False
            self.db[(yaml_fname, key)] = value
            self.db_yaml_paths[(yaml_fname)] = yaml_path

            try:
                if type(value) is OrderedDict and value[MetaData.KEY_ENUM_STORAGE]:
                    self.db_enum_storage_type[(yaml_fname, key)] = value
            except: pass

        if self.conf_debug_dump_db:
            self.logger.debug("DB:")
            for (file, key), value in self.db.items():
                self.logger.debug("file=%s, key=%s\n%s" % (file, key, value))
                
    def initLogger(self):
        if self.conf_log_level == "DEBUG": self.conf_log_level = logging.DEBUG
        if self.conf_log_level == "WARNING": self.conf_log_level = logging.WARNING
        if self.conf_log_level == "ERROR": self.conf_log_level = logging.ERROR
        if self.conf_log_level == "OFF": return
        
        file_path = os.path.join(self.conf_output_path, self.conf_log_file)
        self.mkdir_p(os.path.dirname(file_path))

        self.conf_log_format = logging.Formatter(fmt=self.conf_log_format, datefmt="") #datefmt=log_date_format --> log_date_format: '%d/%m/%Y %I:%M:%S %p'

        logger_fh = logging.FileHandler(filename=file_path, mode='w')
        logger_fh.setFormatter(self.conf_log_format) 
        logger_fh.setLevel(self.conf_log_level)

        logger_ch = logging.StreamHandler()
        logger_ch.setFormatter(self.conf_log_format)
        logger_ch.setLevel(logging.WARNING)

        self.logger = logging.getLogger('root')
        self.logger.addHandler(logger_fh)
        self.logger.addHandler(logger_ch)

        self.logger.setLevel(self.conf_log_level)
        
    def loadConf(self):
        logConsole("Loading configuration file...\n")
        yaml_conf = self.loadYaml(self.yaml_conf_name)
        if yaml_conf == None: self.abort("can't read conf file")
        
        try: self.conf_include_yaml_path = yaml_conf["include_yaml_path"]
        except: self.conf_include_yaml_path = None

        try: self.conf_source_license_header = yaml_conf["source_license_header"]
        except: self.conf_source_license_header = None

        try: self.conf_log_file = yaml_conf["debug"]["log_file"]
        except: self.conf_log_file = ""
        try: self.conf_log_format = yaml_conf["debug"]["log_format"]
        except: self.conf_log_format = ""
        try: self.conf_log_level = yaml_conf["debug"]["log_level"]
        except: self.conf_log_level = ""
        try: self.conf_debug_dump_db = yaml_conf["debug"]["dump_db"]
        except: self.conf_debug_dump_db = False
        try: self.conf_debug_dump_include_files = yaml_conf["debug"]["dump_include_files"]
        except: self.conf_debug_dump_include_files = False
        try: self.conf_debug_dump_yaml = yaml_conf["debug"]["dump_yaml"]
        except: self.conf_debug_dump_yaml = False
        try: self.conf_debug_dump_stack = yaml_conf["debug"]["dump_stack"]
        except: self.conf_debug_dump_stack = False
        try: self.conf_debug_keep_source_marker = yaml_conf["debug"]["keep_source_marker"]
        except: self.conf_debug_keep_source_marker = False
        try: self.conf_output_path_include = os.path.abspath( os.environ["TLVF_PATH_INCLUDE"] )
        except:
            logConsole("env TLVF_PATH_INCLUDE not set, using {}\n".format(self.conf_output_path + '/include'))
            self.conf_output_path_include = self.conf_output_path + '/include'
        try: self.conf_output_path_src = os.path.abspath( os.environ["TLVF_PATH_SRC"] )
        except:
            logConsole("env TLVF_PATH_SRC not set, using {}\n".format(self.conf_output_path + '/src'))
            self.conf_output_path_src = self.conf_output_path + '/src'
        logConsole("Done\n")

    def loadYamlFileNames(self):
        if self.conf_include_yaml_path:
            for path in self.conf_include_yaml_path:
                path = os.path.join(self.yaml_root_path, path)
                if not os.path.isdir(path):
                    if path.endswith(".yaml"):
                        self.yaml_file_list.append(os.path.abspath(path))
                    else:
                        self.logger.error("path={}".format(path))
                        self.abort("Error in include_yaml_path, include line: %s" % path)
                else:
                    for (dirpath, dirnames, filenames) in os.walk(path):
                        for fname in filenames:
                            file_path = os.path.join(dirpath, fname)
                            if file_path.endswith(".yaml") and os.path.isfile(file_path):
                                self.yaml_file_list.append(os.path.abspath(file_path))
        
        if len(self.yaml_file_list) == 0:
            self.abort("yaml file list is empty!")
        
        if self.conf_debug_dump_include_files:
            self.logger.debug("yaml file list:")
            for f in self.yaml_file_list:
                self.logger.debug("%s" % f)

    def loadYaml(self, fname):
        if not os.path.isfile(fname):
            raise Exception("can't open file: %s" % fname)
        with open(fname, 'r') as stream:
            try:
                yaml_inst = yaml.load(stream, Loader=OrderedDictYAMLLoader)
            except yaml.YAMLError as exc:
                raise Exception("  %s" % str(exc))
            return yaml_inst
        raise Exception("Unexpected end of loadYaml")

    def dumpYaml(self, yaml_inst, force=False):
        if force or self.conf_debug_dump_yaml:
            pp = pprint.PrettyPrinter(indent=4)
            self.logger.debug( pp.pformat(yaml_inst) )

    def abort(self, msg=""):
        if self.conf_debug_dump_stack:
            stack_msg = "\n=========Traceback===========\n"
            tr = traceback.format_stack()
            tr = tr[:-1]
            for l in tr: stack_msg += l
            stack_msg += "=============================\n"
            self.logger.error(stack_msg)

        msg =  ("\n================================\n" + 
                msg + "\n" +
                "================================\n")
        self.logger.error(msg)
        
        sys.exit(1)

def test(conf, output, print_dependencies, print_outputs):
    code_c = r'''
#include <stdio.h>
#include "test.h"
int main() {
  for(int i=0;i<5;i++) {
    printf("%s: Testing %d\n",MESSAGE, i);
  }
  return 0;
}
'''
    code_h = r'''
#include <stdio.h>
#ifndef __CODE_H__
#define __CODE_H__
#define MESSAGE "GOT"
#endif
'''
    if print_dependencies:
        dependencies = set([os.path.abspath(conf)])
        sys.stdout.write(";".join(dependencies))
        return 0

    path_c = output + os.path.sep + "test.cpp" if output else "./test.cpp"
    path_h = output + os.path.sep + "test.h" if output else "./test.h"
    if print_outputs:
        outputs = [os.path.abspath(path_c), os.path.abspath(path_h)]
        sys.stdout.write(";".join(outputs))
        return 0

    with open(path_c, 'w') as fout:
        fout.write(code_c)
    with open(path_h, 'w') as fout:
        fout.write(code_h)

def main():
    parser = argparse.ArgumentParser(
        description = 'TLV Factory YAML->C++ class code generator',
        formatter_class = argparse.ArgumentDefaultsHelpFormatter
    )
    parser.add_argument('src_path', help="sources folder path")
    parser.add_argument('yaml_path', help="yamls folder path")
    parser.add_argument('out_path', help="output folder path")
    parser.add_argument('-c', '--conf', default=os.path.dirname(os.path.realpath(__file__)) + '/tlvf_conf.yaml', help="path to yaml config file")
    parser.add_argument('--test', action='store_true', help='test')
    parser.add_argument('--print-dependencies', action='store_true', help='test print dependancies')
    parser.add_argument('--print-outputs', action='store_true', help='test print dependancies')
    args = parser.parse_args()

    if args.test:
        test(args.conf, args.output, args.print_dependencies, args.print_outputs)
    else:
        tlvf = TlvF(args.src_path, args.yaml_path, args.out_path, args.conf, args.print_dependencies, args.print_outputs)

if __name__ == '__main__':
    main()
