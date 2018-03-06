
import subprocess
import os

for filename in os.listdir('demos'):
    subprocess.Popen([r"demo2trail.exe", "demos/" + filename, "strafetrails/" + filename ])
    
