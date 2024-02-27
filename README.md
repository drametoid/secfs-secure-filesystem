# secure-filesystem
Building an encrypted fileserver for learnings on Secure System Design

# How to use:
## Init File System
./fileserver keyfile_name

## login with user, e.g. user1
./fileserver user1_keyfile

# Features

## User features:
`cd <directory>` -  The user will provide the directory to move to. It should accept `.` and `..` as current and parent directories respectively and support changing multiple directories at once (cd ../../dir1/dir2). cd / should take you to the current user’s root directory. If a directory doesn't exist, the user should stay in the current directory.  
`pwd` - Print the current working directory. Each user should have /personal and /shared base directories.  
`ls` -  List the files and directories in the current directory separated by a new line. You need to show the directories `.` and `..` as well. To differentiate between a file and a directory, the output should look as follows:  
d -> .  
d -> ..  
d -> directory1  
f -> file1  
`cat <filename>` - Display the actual (decrypted) contents of the file. If the file doesn't exist, print "<filename> doesn't exist".  
`share <filename> <username>` -  Share the file with the target user which should appear under the `/shared` directory of the target user. The files are shared only with read permission. The shared directory must be read-only. If the file doesn't exist, print "File <filename> doesn't exist". If the user doesn't exist, print "User <username> doesn't exist". The first check will be on the file.  
`mkdir <directory_name>` - Create a new directory. If a directory with this name exists, print "Directory already exists".  
`mkfile <filename> <contents>` - Create a new file with the contents. The contents will be printable ASCII characters. If a file with <filename> exists, it should replace the contents. If the file was previously shared, the target user should see the new contents of the file.  
`exit` - Terminate the program.  

## Admin specific features:
Admin should have access to read the entire file system with all user features.  
`adduser <username>`  - This command should create a keyfile called username_keyfile on the host which will be used by the user to access the filesystem. If a user with this name already exists, print "User <username> already exists".  