# ChadStat :: An I3 status bar

![screenshot.png](screenshot.png)

## Modules
- mpd
- caffeine
- http status code checker
- battery
- volume
- maildir watch
- network
- datetime

## Usage

    # ChadStat follows the suckless (https://suckless.org/philosophy) method for configuration management.
    # Make a copy of the standard config file and edit to customize.
    # Recompile everytime after making changes.
    cp config.h.def config.h

    # Build
    make

    # Add config to i3 config file and restart.
    bar {
        ...
        status_command /path/to/chadstat/chadstat
        ...
    }
