import sys
import os
import shutil

source = sys.argv[1]
destination = sys.argv[2]

specific = {
    'dir' : destination,
    'files' : ["ntexp.h"]
}

default = {
    'dir' : destination + "\\ntexp",
    'ext' : ".h"
}

def move_files(policy):
    if "files" in policy.keys():
        for file in policy['files']:
            shutil.move(source + "\\" + file, policy['dir'] + "\\" + file)
    elif "ext" in policy.keys():
        files = [obj for obj in os.listdir(source) if os.path.isfile(os.path.join(source, obj)) and obj.endswith(policy['ext'])]
        for file in files:
            shutil.move(source + "\\" + file, policy['dir'] + "\\" + file)
    return

move_files(specific)
move_files(default)
