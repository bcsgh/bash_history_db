# bash_history_db
A tool for recording bash history to a SQLite DB. 

Note: This adds about 15ms to each command.
If you run large loops over simple commands, that can be noticable.
(If anyone knows of faster (or asynchronous) way to get the command
that tripped a DEBUG trap I'd be interested ot hear about it.)

To install:

- Build `//cli:sql_log_history`
- Copy the binary to some `$CMD`
- Run `$CMD --create`
- Ammend your `.bashrc` with `trap '$CMD -- $(history 1)' DEBUG` 

This will create a SQLite db at `~/.history.sqlite3`.
If you want to place it somewher else, that can be configured via `$HISTORYDB`.

## Setup (for development)
To configure the git hooks, run `./.git_hooks/setup.sh`
