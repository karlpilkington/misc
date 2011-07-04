Control Port
============

Administrative control port for a running application
------------------------------------------------------
The code in controlport.c implements a control port which can be added to an
application. When initialized, it creates a UNIX domain listener which is 
serviced by a command dispatcher. It gives a running application an interactive  
administrative console. However, the commands that are provided by the control
port are entirely up to you. Typically you would have commands that reflect or
change the application state.


// vim: set tw=80 wm=2 aw syntax=asciidoc: 
