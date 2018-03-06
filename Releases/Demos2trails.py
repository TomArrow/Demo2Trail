
import subprocess
import os

for filename in os.listdir('files'):
    subprocess.Popen([r"demo2trail.exe", "files/" + filename])
    
