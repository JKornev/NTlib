import re
import sys
import os

# TODO:
# 1. Parse .h files
# 2. Find native functions according to a template
# 3. Replace it using our templates
# 4. Replace WIN macro with our ones
# 5. Save a new .h file
# 6. Generate a main .h file
# 7. Make includes for phnt files + our custom files
# 8. Profit

c_headers_source_dir = "phnt-master"
c_headers_output_dir = "output"
c_ignore_files = ["phnt.h", "phnt_ntdef.h", "phnt_windows.h", "ntzwapi.h"]
c_include_files = ["ntstatus.h", "ntcommon.h"]
c_common_header_template = "ntexp.h"

c_regex_syscall = re.compile(r"^NTSYSCALLAPI\s+(.*)\s+NTAPI\s+Nt(\w+)\s*\(\s*((.*\s*)*?)(\);)", re.MULTILINE)
c_regex_sysapi  = re.compile(r"^NTSYSAPI\s+(.*)\s+NTAPI\s+(\w+)\s*\(\s*((.*\s*)*?)(\);)", re.MULTILINE)

def main():
    headers = get_files_with_ext(c_headers_source_dir, ".h")
    headers = remove_from_list(headers, c_ignore_files)
    
    rebuild_headers(headers)
    build_common_header(headers)

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
    for h in c_include_files + headers:
        include += "#include \"" + h + "\"\n"
        print("  Included: " + h)

    common = read_file(c_common_header_template)
    output = re.sub(r"\/\/INCLUDE\:ZONE", include, common)
    write_file(os.path.join(c_headers_output_dir, c_common_header_template), output)
    print("Done")

def transform_header(source):
    # Syscalls to macro
    output = c_regex_syscall.sub(lambda s: make_sysapi_macro(s.groups(), "NATIVE", True), source)
    # Native API to macro
    output = c_regex_sysapi.sub(lambda s: make_sysapi_macro(s.groups(), "NTDLL", False), output)
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
    output = re.sub("PHNT_REDSTONE5",   "NTLIB_WIN_10_RS5",  output)
    output = re.sub("PHNT_19H1",        "NTLIB_WIN_10_TH1",  output)
    output = re.sub("PHNT_VERSION",     "NTLIB_WIN_VERSION", output)
    return output
    
def make_sysapi_macro(groups, prefix, check_type):
    retval = groups[0].strip()
    fnname = groups[1].strip()
    args   = groups[2].strip()

    assert(len(retval) < 32)
    assert(len(fnname) < 64)
    assert(len(args) < 1024)

    allowed  = ["NTSTATUS", "BOOLEAN", "ULONG", "PVOID", "VOID"]
    unknown  = False
    noreturn = False
    macro    = ""

    if (check_type and not retval in allowed):
        print("Warning! Unknown syscall: " + retval + " " + fnname + "(...)")
        unknown = True
    elif retval == "VOID":
        noreturn = True

    macro += ("/*UNKNOWN SYSAPI DEFINITION:\n////" if unknown else "")
    macro += prefix + ("_API_VOID" if noreturn else "_API") + "(" + retval + ", /*Nt*/" + fnname + ", (\n"
    macro += ("////" if unknown else "") + "    " + args + "\n"
    macro += ("////" if unknown else "") + ")"
    #print("    Parsed: " + retval + " " + fnname + "(...)")
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
