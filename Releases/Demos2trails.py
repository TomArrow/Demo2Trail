
import subprocess
import os

for filename in os.listdir('demos'):
    if filename.endswith(".dm_26"):
        subprocess.Popen([r"demo2trail.exe", "demos/" + filename, "strafetrails/" + filename ])
    
