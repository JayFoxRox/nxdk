
#define MASK(mask, val) (((val) << (ffs(mask)-1)) & (mask))


static void xgux_wait_for_idle() {
    while(pb_busy()) {
        /* Wait for completion... */
    }
}

static void xgux_swap() {
    while (pb_finished()) {
        /* Not ready to swap yet */
    }
}
