**Make sure to configure your `NXDK_DIR` environment variable!**

To use this, run it like this:

```
reset && make -C $NXDK_DIR/tools/fp20compiler && ./run.sh
```

To test a specific folder:

```
reset && make -C $NXDK_DIR/tools/fp20compiler && ./run.sh TS1.0
```
