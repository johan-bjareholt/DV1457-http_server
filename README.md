DV1457-http-server
==================

A simple HTTP 1.0 server written in C for the course DV1457 Programming in a Unix Environment at Blekine Institure of Technology.

#### Features

- Support for thread and fork
- Support for running as a daemon
- Jail support
- CLF log format to either syslog or file


#### ToDo

- Handle ctrl+c
- Kill all forked processes and threads instantly on shutdown
- Fix syslog in jail (pass flag to openlog to instantly open)
- Fix log to file in thread/fork (it creates the file but doesn't write anything)
- Segfaults during heavy parallell load when using threads
