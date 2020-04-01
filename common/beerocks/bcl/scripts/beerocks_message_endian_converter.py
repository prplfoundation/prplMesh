#!/usr/bin/env python2

# SPDX-License-Identifier: BSD-2-Clause-Patent
#
#
#
# Copyright (c) 2019 Intel Corporation
#
#
#
# This code is subject to the terms of the BSD+Patent license.
#
# See LICENSE file for more details.
#
#

import sys
import os
import copy
import tempfile
import filecmp
import shutil


class MessageEndianConverter():
    def __init__(self):

        cwd = os.getcwd()
        print(cwd)
        if cwd.endswith("build"):
            self.PATH_PREFIX = "../"
        else:
            self.PATH_PREFIX = "./"
        self.CONTROL_HEADER_FILE = self.PATH_PREFIX +\
            "include/beerocks/bcl/beerocks_message_control.h"
        self.CONTROL_STRUCTS_FILE = self.PATH_PREFIX +\
            "include/beerocks/bcl/beerocks_message_structs.h"
        self.NET_STRUCTS_FILE = self.PATH_PREFIX + "include/beerocks/bcl/network/net_struct.h"
        self.OUT_HEADER_FILE = self.PATH_PREFIX +\
            "include/beerocks/bcl/beerocks_message_endian_converter.h"
        self.OUT_CPP_FILE = self.PATH_PREFIX + "source/beerocks_message_endian_converter.cpp"
        self.INTEL_HEADER = self.PATH_PREFIX + "intel_header.txt"
        self.debug = 0
        self.control_header_lines = ""
        self.control_structs_lines = ""
        self.action_op = []
        self.structs_dic = {}

    def readArgs(self, args):
        if "-d" in args:
            self.debug = 1
            print("Debug level 1 enabled")
        elif "-dd" in args:
            self.debug = 2
            print("Debug level 2 enabled")
        return True

    def run(self, args):
        if not self.readArgs(args):
            return
        if not self.readControlHeaderFile():
            return
        if not self.readControlStructsFile():
            return
        if not self.loadActionOp():
            return
        if not self.loadStructs(self.control_header_lines):
            return
        if not self.loadStructs(self.control_structs_lines):
            return
        if not self.writeHeaderFile():
            return
        if not self.writeCPPFile():
            return

    def writeLine(self, fd, line):
        fd.write(line)
        fd.write('\n')

    def writeHeaderFile(self):
        fd = tempfile.NamedTemporaryFile(mode='w+t', delete=False)

        fd2 = open(self.INTEL_HEADER, "rt")
        t = fd2.read()
        fd.write(t)
        self.writeLine(fd, '\n')
        fd2.close()
        t = ""

        self.writeLine(fd, '#ifndef _BEEROCKS_MESSAGE_ENDIAN_CONVERTER_H_')
        self.writeLine(fd, '#define _BEEROCKS_MESSAGE_ENDIAN_CONVERTER_H_')
        self.writeLine(fd, '')
        self.writeLine(fd, '#include "beerocks_message_control.h"')
        self.writeLine(fd, '#include "network/swap.h"')
        self.writeLine(fd, '')
        self.writeLine(fd, 'namespace beerocks {')
        self.writeLine(fd, 'namespace message  {')
        self.writeLine(fd, '')

        self.writeLine(fd, 'class endian_converter')
        self.writeLine(fd, '{')
        self.writeLine(fd, '    public:')
        self.writeLine(
            fd, '        static void swap_message(uint8_t* buffer, bool only_header=false);')
        self.writeLine(fd, '};')
        self.writeLine(fd, '')

        self.writeLine(fd, '} // namespace message')
        self.writeLine(fd, '} // namespace beerocks')
        self.writeLine(fd, '')

        self.writeLine(fd, '#endif // _BEEROCKS_MESSAGE_ENDIAN_CONVERTER_H_')

        temp_file_name = fd.name
        fd.close()

        # Check if anything changed since the last build
        if not os.path.isfile(self.OUT_HEADER_FILE) or\
                not filecmp.cmp(temp_file_name, self.OUT_HEADER_FILE, shallow=False):
            print("Updating %s..." % self.OUT_HEADER_FILE)
            shutil.copyfile(temp_file_name, self.OUT_HEADER_FILE)

        os.remove(temp_file_name)
        return True

    def writeCPPFile(self):
        fd = tempfile.NamedTemporaryFile(mode='w+t', delete=False)

        self.writeLine(fd, '#include <bcl/beerocks_message_endian_converter.h>')
        self.writeLine(fd, '')
        self.writeLine(fd, 'using namespace beerocks;')
        self.writeLine(fd, 'using namespace message;')
        self.writeLine(fd, '')
        self.writeLine(fd, 'void endian_converter::swap_message(uint8_t* buffer, bool only_header)')
        self.writeLine(fd, '{')
        self.writeLine(fd, '    auto header = (message::sActionHeader*)buffer;')
        self.writeLine(
            fd, '    if(header->action != ACTION_CONTROL) return; '
                '// local message, no need to convert')
        self.writeLine(fd, '')

        [lines, has_code, error] = self.convertStruct("sActionHeader")
        if error:
            self.writeLine(fd, '#error "Code generation failed!!"')
            return False
        self.writeLine(fd, '    //swap header')
        self.writeLine(fd, '    auto msg = header;')
        for line in lines:
            self.writeLine(fd, '    %s' % line)
        self.writeLine(fd, '')
        self.writeLine(fd, '    //advance buffer pointer past the header')
        self.writeLine(fd, '    buffer += sizeof(message::sActionHeader);')
        self.writeLine(fd, '')

        self.writeLine(fd, '    if (only_header) return;')
        self.writeLine(fd, '')
        self.writeLine(fd, '    switch(header->action_op)')
        self.writeLine(fd, '    {')
        for op in self.action_op:
            struct_name = "s"+op
            [lines, has_code, error] = self.convertStruct(struct_name)
            if error:
                self.writeLine(fd, '#error "Code generation failed!!"')
                return False
            self.writeLine(fd, '        case ' + op + ':')
            self.writeLine(fd, '        {')
            self.writeLine(fd, '            %sauto msg = (message::s%s*)buffer;' %
                           ("" if has_code else "//", op))
            for line in lines:
                self.writeLine(fd, '            %s' % line)
            self.writeLine(fd, '            break;')
            self.writeLine(fd, '        }')
        self.writeLine(fd, '    }')
        self.writeLine(fd, '}')

        tmep_file_name = fd.name
        fd.close()

        # Check if anything changed since the last build
        if not os.path.isfile(self.OUT_CPP_FILE) or\
                not filecmp.cmp(tmep_file_name, self.OUT_CPP_FILE, shallow=False):
            print("Updating %s..." % self.OUT_CPP_FILE)
            shutil.copyfile(tmep_file_name, self.OUT_CPP_FILE)

        os.remove(tmep_file_name)
        return True

    def convertStruct(self, struct_name):
        has_code = False
        error = False
        lines = []
        try:
            var_list = self.structs_dic[struct_name]
        except Exception:
            print("writeConverterCode Error, can't find struct=" + struct_name)
            error = True

        if not error:
            var_list = self.convertVarListToBasicVarTypes(struct_name, var_list)
            if var_list is None:
                return False
            for var in var_list:
                line = self.getSwapLine(var)
                if line is None:
                    error = True
                    break
                if line[0] != "/":
                    has_code = True
                    arr_lines = self.expandArray(line)
                    for l in arr_lines:
                        lines.append(l)
                else:
                    lines.append(line)

        return [lines, has_code, error]

    def convertVarListToBasicVarTypes(self, struct_name, var_list):
        idx = 0
        while (idx < len(var_list)):
            var = var_list[idx]
            ii = idx
            if var[0].startswith("int"):
                idx += 1
            elif var[0].startswith("uint"):
                idx += 1
            elif var[0].startswith("char"):
                idx += 1
            elif var[0].startswith("s"):
                try:
                    var_v_list = copy.deepcopy(self.structs_dic[var[0]])
                    for j in xrange(len(var_v_list)):
                        var_v_list[j][1] = var[1] + "." + var_v_list[j][1]
                    del var_list[ii]
                    for j in xrange(len(var_v_list)):
                        var_list.insert(ii, var_v_list[j])
                        ii += 1
                except Exception:
                    print("convertVarListToBasicVarTypes Error, can't find var=" +
                          str(var) + " in struct=" + struct_name)
                    return None
            else:
                print("convertVarListToBasicVarTypes Error, unknown supported var=" +
                      str(var) + " in struct=" + struct_name)
                return None
        return var_list

    def getSwapLine(self, var):
        line = None
        if var[0].startswith("char"):
            line = "// msg->%s;    //%s" % (var[1], var[0])
        else:
            i1 = var[0].find("int")
            if i1 != -1:
                if var[0][i1+3:] == "64_t":
                    line = "swap_64(msg->%s); //%s" % (var[1], var[0])
                elif var[0][i1+3:] == "32_t":
                    line = "swap_32(msg->%s); //%s" % (var[1], var[0])
                elif var[0][i1+3:] == "16_t":
                    line = "swap_16(msg->%s); //%s" % (var[1], var[0])
                elif var[0][i1+3:] == "8_t":
                    line = "// msg->%s;    //%s" % (var[1], var[0])

        if line is None:
            print("getSwapLine Error: type " + var[0] + " is not supported!")
        return line

    def expandArray(self, line):
        lines_out = []
        i1 = line.find("[")
        if i1 == -1:
            lines_out.append(line)
            return lines_out
        i2 = line.find("]")
        func = line[:i1] + "[i]" + line[i2+1:]
        loops = line[i1+1:i2]
        lines_out.append("for(int i=0;i<%s;i++){" % loops)
        lines_out.append("    %s" % func)
        lines_out.append("}")
        return lines_out

    def cleanText(self, text, fname):
        # clean multi line comment
        new_text = ""
        i0 = 0
        while True:
            i1 = text.find("/*", i0)
            if i1 == -1:
                break
            i2 = text.find("*/", i1)
            if i2 == -1:
                print("cleanText Error, can't find matching */ after position " +
                      str(i1) + " fname=" + fname)
                return None
            else:  # clear the comment
                new_text += text[i0:i1]
                i0 = i2+2

        if i0 < len(text):
            new_text += text[i0:]

        # clear empty and comments lines
        line_list = []
        i0 = 0
        while True:
            i1 = new_text.find("\n", i0)
            if i1 == -1:
                break
            line = new_text[i0:i1].strip()
            i0 = i1+1
            if line == "":
                continue
            ic = line.find("//")
            if ic == 0:
                continue
            elif ic != -1:
                line = line[0:ic]
            line_list.append(line)

        # if self.debug == 2:
        #     print "write debug file " + name
        #     with open("Output_" +  name + ".txt", "w") as text_file:
        #         for line in line_list:
        #             text_file.write(line + "\n")

        return line_list

    def readControlHeaderFile(self):
        fname = self.CONTROL_HEADER_FILE
        fd = open(fname, "rt")
        if fd is None:
            print("readControlHeaderFile Error, can't open file: " + fname)
            return False

        text1 = fd.read()
        fd.close()

        text1 = self.cleanText(text1, fname)
        if text1 is None:
            return False

        self.control_header_lines = text1
        return True

    def readControlStructsFile(self):
        fname = self.CONTROL_STRUCTS_FILE
        fd = open(fname, "rt")
        if fd is None:
            print("readControlStructsFile Error, can't open file: " + fname)
            return False

        text1 = fd.read()
        fd.close()

        text1 = self.cleanText(text1, fname)
        if text1 is None:
            return False

        fname = self.NET_STRUCTS_FILE
        fd = open(fname, "rt")
        if fd is None:
            print("readControlStructsFile Error, can't open file: " + fname)
            return False

        text2 = fd.read()
        fd.close()

        text2 = self.cleanText(text2, fname)
        if text2 is None:
            return False

        self.control_structs_lines = text1 + text2
        return True

    def loadActionOp(self):
        s1 = "enum eActionOp_CONTROL"
        s2 = "ACTION_CONTROL_"
        s3 = "ACTION_CONTROL_ENUM_END"
        state = "Init"

        for line in self.control_header_lines:

            if state == "Init":
                if line.startswith(s1):
                    state = "Fill"
            elif state == "Fill":
                if line.startswith(s3):
                    state = "ListEnd"
                    break
                elif line.startswith(s2):
                    ss = line.split("=")
                    op = ss[0].strip()
                    self.action_op.append(op)
                elif line != "":
                    print("loadActionOp Error, line " + " -> " + line)
                    return False
            else:
                print("loadActionOp Error, state ", state)
                return False

        if state != "ListEnd":
            print("loadActionOp Error in ", state)
            return False

        if self.debug == 1:
            print("self.action_op")
            for op in self.action_op:
                print(op)

        return True

    def loadStructs(self, list_lines):
        state = "FindStart"
        read_line = True
        line_idx = 0
        error = ""
        while error == "":
            if read_line:
                if line_idx >= len(list_lines):
                    break
                line = list_lines[line_idx]
                line_idx += 1
                read_line = False
            if state == "FindStart":
                read_line = True
                if line.startswith("typedef") and line.find("struct") != -1:
                    if line.endswith("{"):
                        state = "Start"
                    else:
                        state = "FindStart2"

            elif state == "FindStart2":
                read_line = True
                if line.endswith("{"):
                    state = "Start"

            elif state == "Start":
                if self.debug == 1:
                    print("** { start, line " + str(line_idx))
                struct_name = ""
                struct_list = []
                state = "FindEnd"

            elif state == "SkipEnum":
                read_line = True
                if line.find("}") != -1:
                    state = "FindEnd"

            elif state == "FindEnd":
                read_line = True
                if line.startswith("enum"):
                    state = "SkipEnum"
                elif line.find("{") != -1:
                    error = "loadStructs, Error unexpected '{' inside typedef struct at line " +\
                            str(line_idx) + " -->" + line
                elif line.find("}") != -1:
                    i1 = line.find("}")
                    i2 = line.find(";")
                    if i2 != -1:
                        struct_name = line[i1+1:i2]
                        struct_name = struct_name.strip()
                        if not struct_name.startswith("s"):
                            error = "loadStructs, Error struct name not starting with 's'" +\
                                    " at line " + str(line_idx) + " -->" + line
                        else:
                            if self.debug == 1:
                                print("** } " + struct_name + " end, line", str(line_idx))

                            self.structs_dic[struct_name] = struct_list

                            state = "FindStart"
                    else:
                        error = "loadStructs, Error struct not ending starting '} name ;'" +\
                                "at line " + str(line_idx) + " -->" + line
                else:
                    line = line.replace(";", "").strip()
                    var = line.split(" ")
                    if len(var) < 2:
                        error = "loadStructs, Error invalid var at line " + \
                            str(line_idx) + " -->" + line
                    else:
                        struct_list.append([var[0], var[1]])

        if error == "":
            if self.debug == 2:
                print(self.structs_dic)
            return True
        else:
            print(error)
            return False


if __name__ == '__main__':
    mec = MessageEndianConverter()
    print("Running message endian_converter...")
    mec.run(sys.argv[1:])
    print("Message endian_converter done.")
