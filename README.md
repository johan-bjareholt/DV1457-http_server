DV1457-http-server
==================

A simple HTTP 1.0 server written in C for the course DV1457 Programming in a Unix Environment at Blekine Institure of Technology.

#### Features

- Support for thread and fork
- Support for running as a daemon
- Jail support
- CLF log format to either syslog or file


#### ToDo

- Test check_students script
- Where should the config file be placed?
- Handle ctrl+c
- Jail only runs as root?
- Kill all forked processes and threads on shutdown
