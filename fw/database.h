// Calibration database

struct cal {
    unsigned short port;
    unsigned char ip_addr[4];
    unsigned char mac_addr[6];
};

// Database instances
extern struct cal cal;
extern const struct cal cal_defaults;

// Database metadata
extern const struct type tyCAL;

void database_init();
