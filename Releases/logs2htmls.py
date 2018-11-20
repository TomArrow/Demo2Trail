
import subprocess
import os

for filename in os.listdir('logs'):
    if filename.endswith(".log"):
        #subprocess.Popen([r"log2html.py", "logs/" + filename, "html_logs/" + filename ])
        subprocess.call(['python.exe', 'log2html.py', "logs/" + filename])
    
