import re
import sys
import os
import datetime

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
            res = ntexport.match(line)
            if not res == None:
                print("     " + res.group(1) + " = ntexport_" + res.group(1))

