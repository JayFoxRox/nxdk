__attribute__((constructor))
void initialize_nxdk_rdt(void) {
    // Set up threads for nxdk-rdt
    net_init();
    dbgd_init();
}
