from time import time
import json
import urllib.request
from pprint import pprint

#Get username, password, last_update from file
infoFile = "demograbber.json"
try:
    info = json.load(open(infoFile))
except Exception:
    print ("Could not load info from demograbber.json")
    quit()
        
for key in info:
    if (key == "username"):
        username = info[key]
    elif (key == "password"):
        password = info[key]
    elif (key == "last_update"):
        last_update = info[key]

#Get json list of demos since last_update from stat_api website    
url = 'http://162.248.89.208/stats/update.php'
values = {'type'     : 'race_demos',
          'username' : username,
          'password' : password,
          'last_update' : last_update }
data = urllib.parse.urlencode(values).encode("utf-8")
req = urllib.request.Request(url, data)
response = urllib.request.urlopen(req)
page = response.read()
try:
    json_obj = json.loads(page)
except Exception:
    print ("Error")
    quit()
if not (json_obj):
    print ("No new demos available")
    quit()

#Parse json and download each demo
hostname = "http://162.248.89.208/races/"
racenames = ["siege", "jka", "qw", "cpm", "q3", "pjk", "wsw", "rjq3", "rjcpm", "swoop", "jetpack", "speed", "sp"]
extension = ".dm_26"

count = 0
for key in json_obj:
    count += 1

    playername = key[0]
    mapname = key[1]
    mapname = mapname.replace("/", "") 
    coursename = mapname.split('(', 1)[-1]
    coursename = coursename.rstrip(')')
    style = racenames[key[2]]
    
    url = (hostname + playername + "/" + playername + "-" + mapname + "-" + style + extension)
    url = url.replace(" ", "") # Use html escape instead
    filename = coursename + "-" + style + extension
    try:
        urllib.request.urlretrieve(url, "demos/" + filename) # Make this multithreaded or simultaneous or something
    except Exception:
        print (filename + " could not be saved [" + str(count) + "]")
        continue
    print (filename + " saved [" + str(count) + "]")

#Update last_update
newInfo = {}
newInfo['username'] = username
newInfo['password'] = password
newInfo['last_update'] = str(int(time()))

with open(infoFile, 'w') as outfile:
    json.dump(newInfo, outfile) 
