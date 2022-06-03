// Prototype, not accurate
struct database {
	char *path;
	char *log_path;
	void *contents;
	FILE *log;
}; // size 32
        struct xcinfocache {
                char *cachefile;
//		struct database db;
		char *db;
		struct {
			short verbose;
			short nocache;
		} xcrun_config;
                char *developer_dir;
                BOOL was_cltools;
                char *path_of_xcodebuild;
                char *sdkroot;
                unsigned long *unknown_7;
                char *toolchains;
                char *developer_usr_path;
                char *external_toolchain_dir;
                char *xcode_overriding_sdks_dir;
                char *built_products_dirs;
                char *unknown_13;
                char *toolchain_dirs;
                char *sysroot_dirs;
        }; // size 128
