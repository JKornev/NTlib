import re
import sys
import os
import datetime

# NATIVE_API(NTSTATUS, /*Nt*/CreateThread, (
ntapi     = re.compile(r"NATIVE_API\((\w+), (?:\w+), /\*Nt\*/(\w+),")
# NATIVE_API(NTSTATUS, /*Nt*/CreateThread, (
ntapivoid = re.compile(r"NATIVE_API_VOID\((?:\w+), /\*Nt\*/(\w+),")
# NTDLL_API(PVOID, RtlAllocateHeap, (
ntdll     = re.compile(r"NTDLL_API\((\w+), (?:\w+), (\w+),")
# NTDLL_API_VOID(RtlInitUnicodeString, (
ntdllvoid = re.compile(r"NTDLL_API_VOID\((?:\w+), (\w+),")
# EXPORT_API(memchr);
ntexport  = re.compile(r"EXPORT_API\((\w+)\)")

drct = sys.argv[1]
headers = [obj for obj in os.listdir(drct) if os.path.isfile(os.path.join(drct, obj)) and obj.endswith(".h")]

print("; Auto-generated definitions file (" + datetime.datetime.now().strftime("%B %d, %Y  %H:%M:%S") + ")")
print("LIBRARY ntdll.dll")
print("EXPORTS")

for h in headers:
    with open(os.path.join(drct, h), "r") as lines:
        print("; === " + h.upper() + " ===")
        for line in lines:
            res = ntapi.match(line)
            if not res == None:
                print(" Nt" + res.group(2))
                print(" Zw" + res.group(2))
            
            res = ntapivoid.match(line)
            if not res == None:
                print("  Nt" + res.group(1))
                print("  Zw" + res.group(1))

            res = ntdll.match(line)
            if not res == None:
                print("   " + res.group(2))

            res = ntdllvoid.match(line)
            if not res == None:
                print("    " + res.group(1))

            res = ntexport.match(line)
            if not res == None:
                print("     " + res.group(1) + " = ntexport_" + res.group(1))

