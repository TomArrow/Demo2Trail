# Demo2Trail #

This is a stripped down version of demotrimmer by Teh, modified to create strafetrails for jaPRO.
A strafetrail is created from a .cfg file, which contains a list of coordinates (one per line).
A strafetrail is simply a history of player-movement, represented by a line.  
Since there is a constant 1 coordinate per server frame, strafetrails also represent where a player was at a certain point in time.

Usage: /demo2Trail <demo file name> <cfg file name (optional)>
If no output is specified, it will use the same name as demo file.
You can also drag a demo file into the exe, and it will treat that as the input file.

##Maintainers##
* videoP (loda)

###Credits###
Teh