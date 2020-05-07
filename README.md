# i3status

## Deps
- libasound2-dev
- libcurl4-openssl-dev

## Install
```
make
./statusline
```

Change in ``~/.config/i3/config``:
```
bar {
        status_command /path/to/i3status/statusline
	...
}
```

## Configuration
Edit config in ``config.h`` and recompile
