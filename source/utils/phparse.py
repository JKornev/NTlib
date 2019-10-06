import re
import sys
import os

#TODO:
# - Clean output dir
# - Parse include files for .DEF

c_headers_source_dir   = sys.argv[1]
c_headers_output_dir   = sys.argv[2]
c_common_templates_dir = sys.argv[3]
c_ignore_files  = ["phnt.h", "phnt_ntdef.h", "phnt_windows.h", "ntzwapi.h"]
c_include_files = ["ntcompatibility.h", "ntstatus.h", "ntcommon.h"]
c_common_header_template = "ntexp.h"
c_project_name = "ntexp"
c_include_order_source = "phnt.h"

c_regex_syscall = re.compile(r"^NTSYSCALLAPI\s+(.*)\s+NTAPI\s+Nt(\w+)\s*\(\s*((.*\s*)*?)(\);)", re.MULTILINE)
c_regex_sysapi  = re.compile(r"^NTSYSAPI\s+(.*)\s+NTAPI\s+(\w+)\s*\(\s*((.*\s*)*?)(\);)", re.MULTILINE)
c_regex_include = re.compile(r"#include\s+<(.*)>")
c_ignore_includes = ["pshpack4.h", "poppack.h"]

def main():
    headers = get_files_with_ext(c_headers_source_dir, ".h")
    headers = remove_from_list(headers, c_ignore_files)
    
    rebuild_headers(headers)
    build_common_header(headers)
    copy_includes()

def rebuild_headers(headers):
    print("Rebuilding headers")

    for h in headers:
        print("  Processing " + h)
        source = read_file(os.path.join(c_headers_source_dir, h))
        transformed = transform_header(source)
        write_file(os.path.join(c_headers_output_dir, h), transformed)
        print("  Done")

    print("Done")

def build_common_header(headers):
    print("Rebuilding common header")

    include = ""

    for header in c_include_files:
        include += build_include(header)

    # Add includes according respecting an order
    included = []
    order = build_order()
    for header in order:
        if header in headers:
            included.append(header)
            include += build_include(header)

    # Add includes that isn't ordered
    for header in headers:
        # Already added
        if header in included:
            continue
        include += build_include(header)

    common = read_file(os.path.join(c_common_templates_dir, c_common_header_template))
    output = re.sub(r"\/\/INCLUDE\:ZONE", include.strip(), common)
    write_file(os.path.join(c_headers_output_dir, c_common_header_template), output)
    print("Done")

def build_include(header):
    print("  Included: " + header)
    return "#include <" + c_project_name + r"\\" + header + ">\n"

def build_order():
    source = read_file(os.path.join(c_headers_source_dir, c_include_order_source))
    return c_regex_include.findall(source)

def copy_includes():
    print("Coping include headers")
    for h in c_include_files:
        output = read_file(os.path.join(c_common_templates_dir, h))
        write_file(os.path.join(c_headers_output_dir, h), output)
        print("  Copied: " + h)
    print("Done")

def transform_header(source):
    # Syscalls to macro
    output = c_regex_syscall.sub(lambda s: make_syscall_macro(s.groups()), source)
    # Native API to macro
    output = c_regex_sysapi.sub(lambda s: make_sysapi_macro(s.groups()), output)
    # Includes normalization
    output = c_regex_include.sub(lambda s: remake_include(s.groups()), output)
    # Macro renaming
    output = re.sub("PHNT_MODE_KERNEL", "NTLIB_KERNEL_MODE", output)
    output = re.sub("PHNT_MODE_USER",   "NTLIB_USER_MODE",   output)
    output = re.sub("PHNT_MODE",        "NTLIB_CPU_MODE",    output)
    output = re.sub("PHNT_WIN2K",       "NTLIB_WIN_2K",      output)
    output = re.sub("PHNT_WS03",        "NTLIB_WIN_XP",      output)
    output = re.sub("PHNT_VISTA",       "NTLIB_WIN_VISTA",   output)
    output = re.sub("PHNT_WIN7",        "NTLIB_WIN_7",       output)
    output = re.sub("PHNT_WIN8",        "NTLIB_WIN_8",       output)
    output = re.sub("PHNT_WINBLUE",     "NTLIB_WIN_8_1",     output)
    output = re.sub("PHNT_THRESHOLD",   "NTLIB_WIN_10_TH1",  output)
    output = re.sub("PHNT_THRESHOLD2",  "NTLIB_WIN_10_TH2",  output)
    output = re.sub("PHNT_REDSTONE",    "NTLIB_WIN_10_RS1",  output)
    output = re.sub("PHNT_REDSTONE2",   "NTLIB_WIN_10_RS2",  output)
    output = re.sub("PHNT_REDSTONE3",   "NTLIB_WIN_10_RS3",  output)
    output = re.sub("PHNT_REDSTONE4",   "NTLIB_WIN_10_RS4",  output)
    output = re.sub("PHNT_REDSTONE 5",   "NTLIB_WIN_10_RS5",  output)
    output = re.sub("PHNT_19H1",        "NTLIB_WIN_10_19H1", output)
    output = re.sub("PHNT_VERSION",     "NTLIB_WIN_VERSION", output)
    return output
    
def make_syscall_macro(groups):
    retval = groups[0].strip()
    fnname = groups[1].strip()
    args   = groups[2].strip()

    assert(len(retval) < 32)
    assert(len(fnname) < 64)
    assert(len(args) < 1024)

    allowed  = ["NTSTATUS", "BOOLEAN", "ULONG", "PVOID", "VOID"]
    noreturn = False

    if (not retval in allowed):
        raise Exception("Unknown syscall: " + retval + " " + fnname + "(...)")
    
    if retval == "VOID":
        noreturn = True

    return build_sysapi_str("NATIVE", retval, fnname, "Nt", args, noreturn)

def make_sysapi_macro(groups):
    retval = groups[0].strip()
    fnname = groups[1].strip()
    args   = groups[2].strip()

    assert(len(retval) < 32)
    assert(len(fnname) < 64)
    assert(len(args) < 1024)

    noreturn = False

    if retval == "VOID":
        noreturn = True

    return build_sysapi_str("NTDLL", retval, fnname, None, args, noreturn)

def remake_include(groups):
    header = groups[0]
    if header in c_ignore_includes:
        return "#include <" + header + ">"

    return "#include \"" + header + "\""

def build_sysapi_str(prefix, retval, fnname, comment, args, noreturn):
    macro = prefix + ("_API_VOID" if noreturn else "_API") 
    macro += "(" + ("" if noreturn else retval + ", ") + ("/*" + comment + "*/" if comment else "") + fnname + ", (\n"
    macro += "    " + args + "\n"
    macro += "))"
    return macro

def get_files_with_ext(dir, ext):
    files = []
    for file in os.listdir(dir):
        if file.endswith(ext) and os.path.isfile(os.path.join(dir, file)):
            files.append(file)
    return files

def remove_from_list(source, what):
    return [x for x in source if x not in what]

def read_file(file):
    return open(file, "r").read()

def write_file(file, txt):
    open(file, "w+").write(txt)

if __name__== "__main__":
    main()
