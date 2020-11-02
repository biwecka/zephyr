# MCUmgr on Windows
## Install "newtmgr"
-   Download `apache-mynewt-newtmgr-bin-windows-x.y.z.tgz` from
    [here](https://apache.lauf-forum.at/mynewt/apache-mynewt-1.8.0/).
-   Extract it to `C:\`
-   Add the extracted directory to the environment variable "PATH".

## Usage
When you successfully flashed the bootloader and this sample application
you can execute:  
`newtmgr --conntype serial --connstring "COM6" image list`  
or
`newtmgr --conntype serial --connstring "COM6" echo hello`  

Both commands should work.  
**Note**: When you are currently connected to the "Shell" these
commands (might) not work. Exit the "Shell" connection before using
"newtmgr".

