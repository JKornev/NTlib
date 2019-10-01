import re
import sys

# NATIVE_API(NTSTATUS, /*Nt*/CreateThread, (
ntapi  = re.compile(r"NATIVE_API\((\w+), /\*Nt\*/(\w+),")
# NTDLL_API(PVOID, RtlAllocateHeap, (
ntdll  = re.compile(r"NTDLL_API\((\w+), (\w+),")
# NTDLL_API_VOID(RtlInitUnicodeString, (
ntvoid = re.compile(r"NTDLL_API_VOID\((\w+),")

with open(sys.argv[1], "r") as lines:

    print("LIBRARY ntdll.dll")
    print("EXPORTS")
    
    for line in lines:

        res = ntapi.match(line)
        if not res == None:
            print(" Nt" + res.group(2))
            print(" Zw" + res.group(2))

        res = ntdll.match(line)
        if not res == None:
            print("  " + res.group(2))

        res = ntvoid.match(line)
        if not res == None:
            print("   " + res.group(1))

