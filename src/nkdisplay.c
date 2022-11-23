#include "nkcli.h"
#include "nkdisplay.h"

// Generic display CLI command

int cmd_display(nkdisplay_t *dev, nkinfile_t *args)
{
    uint8_t buf[40];
    int rtn;
    int bright;
    if (dev->init && nk_fscan(args, "init "))
    {
        rtn = dev->init(dev);
        if (rtn)
        {
            nk_printf("Could not access\n");
        }
    }
    else if (dev->write && nk_fscan(args, "w %w", buf, sizeof(buf)))
    {
	nk_sfprintf(dev->buf, dev->width, "%s", buf);
        rtn = dev->write(dev, 0, 0, (uint8_t *)dev->buf, dev->width);
        if (rtn)
        {
            nk_printf("No ack\n");
        }
        else
        {
            nk_printf("Wrote '%s'\n", buf);
        }
    }
    else if (dev->write_raw && nk_fscan(args, "raw %e"))
    {
	uint16_t write_array[8];
	uint32_t write_len = 0;
	memset(write_array, 0, sizeof(write_array));
        while (nk_fscan(args, "%hx %e", &write_array[write_len]))
        {
            if (write_len != 7)
            {
	        ++write_len;
            }
        }
        rtn = dev->write_raw(dev, 0, 0, write_array, write_len);
        if (rtn)
        {
            nk_printf("No ack\n");
        }
        else
        {
            nk_printf("Wrote %"PRIu32" shorts\n", write_len + 1);
        }
    }
#if 0
    else if (dev->write && nk_fscan(args, "count "))
    {
        rtn = nk_tm1638_init(&tm1638);
        if (rtn) {
            nk_printf("Pin access error\n");
            return 0;
        }
        if (!tm1638_tid)
            tm1638_tid = nk_alloc_tid();
        tm1638_counter = 0;
        tm1638_count(NULL);
    }
    else if(dev->scan && nk_fscan(args, "scan "))
    {
        uint32_t val;
        rtn = nk_tm1638_keyscan(&tm1638, &val);
        if (rtn) {
            nk_printf("Pin access error\n");
        } else {
            nk_printf("%8.8"PRIx32"\n", val);
        }
    }
#endif
    else if(dev->mode && nk_fscan(args, "on %d ", &bright))
    {
        rtn = dev->mode(dev, true, false, false, bright);
        if (rtn)
        {
            nk_printf("Pin access error\n");
        }
    }
    else if(dev->mode && nk_fscan(args, "off "))
    {
        rtn = dev->mode(dev, false, false, false, 0);
        if (rtn)
        {
            nk_printf("Pin access error\n");
        }
    }
    else
    {
        nk_printf("Syntax error\n");
    }
    return 0;
}
