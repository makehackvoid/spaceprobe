# SpaceProbe
## Like the 10th time

Ship it.

## OpenRC Service Definition

The `controller/spaceprobed-run.sh` script is an OpenRC service definition.

To use it, copy it to the `openrc` `init.d` directory (on Alpine this is
`/etc/init.d/`) and ensure that it is owned by root and executable.

```sh
cp controller/spaceprobed-run.sh /etc/init.d/spaceprobe
chmod +x /etc/init.d/spaceprobed
```

```ls
-rwxr-xr-x    1 root     root           444 Dec  9 19:37 spaceprobed
```

Then add the service:
```sh
rc-update add spaceprobed
```

You will then be able to use the standard `openrc` commands to manage the
service.

```sh
rc-service spaceprobed [status, stop, start]
```
