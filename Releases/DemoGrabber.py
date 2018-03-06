import json
import urllib.request
from pprint import pprint

data = json.load(open('test.json')) # Get this with cURL

hostname = "http://162.248.89.208/races/"
racenames = ["siege", "jka", "qw", "cpm", "q3", "pjk", "wsw", "rjq3", "rjcpm", "swoop", "jetpack", "speed", "sp"]
extension = ".dm_26"

count = 0
for key in data:
    count += 1
        
    username = key[0]
    mapname = key[1]
    mapname = mapname.replace("/", "")
    
    style = racenames[key[2]]
    url = (hostname + username + "/" + username + "-" + mapname + "-" + style + extension)
    url = url.replace(" ", "") # Use html escape instead!

    coursename = mapname.split('(', 1)[-1]
    coursename = coursename.rstrip(')')
    
    filename = coursename + "-" + style + extension
    try:
        urllib.request.urlretrieve(url, "demos/" + filename) # Make this multithreaded or simultaneous or something
    except Exception:
        print (filename + " could not be saved [" + str(count) + "]")
        continue
    # Don't crash if this fails!

    print (filename + " saved [" + str(count) + "]")
    
