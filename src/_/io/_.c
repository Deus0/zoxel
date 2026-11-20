// io: the file system, the same calls on every platform (linux, windows, android)
//  - folders: zox_mkdir, zox_mkdirs (and the folders on the way)
//  - files: zox_copy_file, zox_copy_directory, zox_executable_path (deleting a folder: delete_dir, _/pathing)
//  - use these, not mkdir or system("mkdir -p ...") with an #ifdef at each call: windows' mkdir takes no mode, and its
//    system() runs cmd, which has no mkdir -p, rm -rf or cp
#include "fun/_.c"
