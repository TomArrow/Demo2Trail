import sys
import re
from html import escape

#logFileName = sys.argv[1] #drag and drop

inputName = "test" #demo filename w/o extension goes here

logFileName = inputName + ".log"
logFile = open(logFileName,'r') #try ?
output = "<html><head><title>" + logFileName + "</title><style type='text/css'>body,td,th {background-color: #666699;font-family: Courier New, Courier, mono;font-size: 11pt;}.c0 {color: #000000;text-shadow: 1px 1px 1px #FFFFFF} .c1 {color: #FF0000;text-shadow: 1px 1px 1px #000000} .c2 {color: #00FF00;text-shadow: 1px 1px 1px #000000} .c3 {color: #FFFF00} .c4 {color: #0000FF;text-shadow: 1px 1px 1px #000000} .c5 {color: #00FFFF} .c6 {color: #FF00FF;text-shadow: 1px 1px 1px #000000} .c7 {color: #FFFFFF;text-shadow: 1px 1px 1px #000000} .c8 {color: #FF8C00;text-shadow: 1px 1px 1px #000000} .c9 {color: #C0C0C0} .shadow {text-shadow: 1px 1px 1px #000000} .padding {padding-left: 50px;}</style></head><body><pre>\n"

for line in logFile:
    replaced = re.sub(r"(\^([0-9]))+", r"</span><span class='shadow c\2'>", escape(line)) #regex this for all colors, #line by line if want change formatting based on teamsay/say/tell/print
    output = output + "<span class='padding shadow c7'>" + replaced + "</span>"

output = output + "</pre></body></html>"

htmlFileName = inputName + ".html" #logfilename - extension + html ?
htmlFile = open(htmlFileName, "w")
htmlFile.write(output)

#get py2exe working
#get drag and drop working
#format colors better, add shadows or border for readability?
#better background color?