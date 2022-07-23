#include <inttypes.h>
#include "nkcli.h"
#include "tunnel_fpga.h"
#include "basic_cmds.h"

const struct regfield fpga_regtable[]=
{
    { FPGAREG_ID, 0, 32, "id", "" },
    { FPGAREG_TEST, 0, 32, "test", "" },
    { FPGAREG_INT_ENABLE, 0, 32, "int_enable", "" },
        { 0x01, 0, 1, "eth_rx_int_enable", "" },
    { FPGAREG_INT_PENDING, 0, 32, "int_pending", "" },
        { 0x01, 0, 1, "eth_rx_int_pending", "" },
    { 0, 0, 0, 0, 0 }
};

void reboot()
{
    // fpga_wr(FPGAREG_CAM_I2C, (REBOOT_BIT << 16) + REBOOT_BIT); // Reboot system
}

static void pci_recv()
{
    for (;;)
    {
        uint32_t val;
        fpga_rd(FPGAREG_PCIE_FIFO_STATUS, &val);
        if (val & NE_BIT)
        {
            nk_printf("Recv: ");
            if (val & SOP_BIT) nk_printf("SOP ");
            if (val & EOP_BIT) nk_printf("EOP ");
            if (!(val & (SOP_BIT | EOP_BIT))) nk_printf("    ");
            fpga_rd(FPGAREG_PCIE_FIFO, &val);
            nk_printf("%8.8"PRIx32"\n", val);
        }
        else
        {
            break;
        }
    }
    
}

static uint32_t ucfg_rd(uint32_t addr)
{
    uint32_t val;
    fpga_wr(FPGAREG_PCIE_CTRL, addr | CONFIG_BIT);
    fpga_rd(FPGAREG_PCIE_CONFIG, &val);
    return val;
}

static void ucfg_wr(uint32_t addr, uint32_t val)
{
    fpga_wr(FPGAREG_PCIE_CONFIG, val);
    fpga_wr(FPGAREG_PCIE_CTRL, addr | CONFIG_BIT | WRITE_BIT);
}

static uint32_t lmmi_rd(uint32_t addr)
{
    uint32_t val;
    fpga_wr(FPGAREG_PCIE_CTRL, addr | LMMI_BIT);
    fpga_rd(FPGAREG_PCIE_LMMI, &val);
    return val;
}

static void lmmi_wr(uint32_t addr, uint32_t val)
{
    fpga_wr(FPGAREG_PCIE_LMMI, val);
    fpga_wr(FPGAREG_PCIE_CTRL, addr | LMMI_BIT | WRITE_BIT);
}

static void pci_send(uint32_t *data, uint32_t len)
{
    uint32_t first = SOP_BIT;
    nk_printf("Sending: ");
    while (len)
    {
        if (len == 1)
            first |= EOP_BIT;
        fpga_wr(FPGAREG_PCIE_FIFO_STATUS, first);
        first = 0;
        nk_printf(" %8.8"PRIx32"\n", *data);
        fpga_wr(FPGAREG_PCIE_FIFO, *data++);
        --len;
    }
    fpga_wr(FPGAREG_PCIE_CTRL, SEND_BIT);
    nk_printf("Done.\n");
    pci_recv();
}

static uint32_t swap(uint32_t val)
{
    return ((0x000000FF & val) << 24) | ((0x0000FF00 & val) << 8) | ((0x00FF0000 & val) >> 8) | ((0xFF000000 & val) >> 24);
}

static uint32_t header_1(uint32_t fmt, uint32_t type, uint32_t len)
{
    return ((fmt & 7) << 29) | ((type & 31) << 24) | len;
}

static uint32_t header_2(uint32_t requester_id, uint32_t tag, uint32_t last_be, uint32_t first_be)
{
    return (requester_id << 16) | (tag << 8) | (last_be << 4) | first_be;
}

uint8_t tag;

static void mem_wr(uint32_t addr, uint32_t data)
{
    uint32_t packet[4];
    packet[0] = swap(header_1(2, 0, 1));
    packet[1] = swap(header_2(0, tag++, 0, 0x0F));
    packet[2] = swap(addr);
    packet[3] = data;
    pci_send(packet, 4);
}

static void mem_rd(uint32_t addr)
{
    uint32_t packet[3];
    packet[0] = swap(header_1(0, 0, 1));
    packet[1] = swap(header_2(0, tag++, 0, 0x0F));
    packet[2] = swap(addr);
    pci_send(packet, 3);
}

static uint32_t bfd(uint32_t b, uint32_t f, uint32_t d)
{
    return ((b << 8) | (d << 3) | f) << 16;
}

static void conf0_wr(uint32_t b, uint32_t f, uint32_t d, uint32_t addr, uint32_t data)
{
    uint32_t packet[4];
    packet[0] = swap(header_1(2, 4, 1));
    packet[1] = swap(header_2(0, tag++, 0, 0x0F));
    packet[2] = swap(bfd(b, f, d) | addr);
    packet[3] = data;
    pci_send(packet, 4);
}

static void conf0_rd(uint32_t b, uint32_t f, uint32_t d, uint32_t addr)
{
    uint32_t packet[3];
    packet[0] = swap(header_1(0, 4, 1));
    packet[1] = swap(header_2(0, tag++, 0, 0x0F));
    packet[2] = swap(bfd(b, f, d) | addr);
    pci_send(packet, 3);
}

static void conf1_wr(uint32_t b, uint32_t f, uint32_t d, uint32_t addr, uint32_t data)
{
    uint32_t packet[4];
    packet[0] = swap(header_1(2, 5, 1));
    packet[1] = swap(header_2(0, tag++, 0, 0x0F));
    packet[2] = swap(bfd(b, f, d) | addr);
    packet[3] = data;
    pci_send(packet, 4);
}

static void conf1_rd(uint32_t b, uint32_t f, uint32_t d, uint32_t addr)
{
    uint32_t packet[3];
    packet[0] = swap(header_1(0, 5, 1));
    packet[1] = swap(header_2(0, tag++, 0, 0x0F));
    packet[2] = swap(bfd(b, f, d) | addr);
    pci_send(packet, 3);
}

static int cmd_pci(nkinfile_t *args)
{
    uint32_t addr;
    uint32_t val;
    uint32_t bus;
    uint32_t func;
    uint32_t dev;
    if (nk_fscan(args, " "))
    {
        fpga_rd(FPGAREG_PCIE_STATUS, &val);

        if (val & PL_LINK_UP_BIT) nk_printf("PL up.\n");
        else nk_printf("PL down!\n");

        if (val & DL_LINK_UP_BIT) nk_printf("DL up.\n");
        else nk_printf("DL down!\n");

        if (val & TL_LINK_UP_BIT) nk_printf("TL up.\n");
        else nk_printf("TL down!\n");

        nk_printf("Recv count = %ld\n", (val >> SOP_COUNT_SHIFT) & ((1 << SOP_COUNT_WIDTH) - 1));
        nk_printf("Send count = %ld\n", (val >> SEND_COUNT_SHIFT) & ((1 << SEND_COUNT_WIDTH) - 1));

        pci_recv();
    }
    else if (nk_fscan(args, "ucfg rd %"PRIx32" ", &addr))
    {
        nk_printf("UCFG addr = %"PRIx32" read = %"PRIx32"\n", addr, ucfg_rd(addr));
    }
    else if (nk_fscan(args, "ucfg wr %"PRIx32" %"PRIx32" ", &addr, &val))
    {
        ucfg_wr(addr, val);
        nk_printf("UCFB addr = %"PRIx32" write = %"PRIx32"\n", addr, val);
    }
    else if (nk_fscan(args, "lmmi rd %"PRIx32" ", &addr))
    {
        nk_printf("LMMI addr = %"PRIx32" read = %"PRIx32"\n", addr, lmmi_rd(addr));
    }
    else if (nk_fscan(args, "lmmi wr %"PRIx32" %"PRIx32" ", &addr, &val))
    {
        lmmi_wr(addr, val);
        nk_printf("LMMI addr = %"PRIx32" write = %"PRIx32"\n", addr, val);
    }
    else if (nk_fscan(args, "mem rd %"PRIx32" ", &addr))
    {
        nk_printf("MEM addr = %"PRIx32" read\n", addr);
        mem_rd(addr);
    }
    else if (nk_fscan(args, "mem wr %"PRIx32" %"PRIx32" ", &addr, &val))
    {
        nk_printf("MEM addr = %"PRIx32" write = %"PRIx32"\n", addr, val);
        mem_wr(addr, val);
    }
    else if (nk_fscan(args, "conf0 wr %"PRIx32":%"PRIx32".%"PRIx32" %"PRIx32" %"PRIx32" ",  &bus, &dev, &func, &addr, &val))
    {
        nk_printf("Conf0 %"PRIx32":%"PRIx32".%"PRIx32" addr = %"PRIx32" write = %"PRIx32"\n", bus, dev, func, addr, val);
        conf0_wr(bus, func, dev, addr, val);
    }
    else if (nk_fscan(args, "conf0 rd %"PRIx32":%"PRIx32".%"PRIx32" %"PRIx32" ", &bus, &dev, &func, &addr))
    {
        nk_printf("Conf0 %"PRIx32":%"PRIx32".%"PRIx32" addr = %"PRIx32" read\n", bus, dev, func, addr);
        conf0_rd(bus, func, dev, addr);
    }
    else if (nk_fscan(args, "conf1 wr %"PRIx32":%"PRIx32".%"PRIx32" %"PRIx32" %"PRIx32" ",  &bus, &dev, &func, &addr, &val))
    {
        nk_printf("Conf1 %"PRIx32":%"PRIx32".%"PRIx32" addr = %"PRIx32" write = %"PRIx32"\n", bus, dev, func, addr, val);
        conf1_wr(bus, func, dev, addr, val);
    }
    else if (nk_fscan(args, "conf1 rd %x:%x.%x %x ", &bus, &dev, &func, &addr))
    {
        nk_printf("Conf1 %"PRIx32":%"PRIx32".%"PRIx32" addr = %"PRIx32" read\n", bus, dev, func, addr);
        conf1_rd(bus, func, dev, addr);
    }
    else
    {
        nk_printf("Syntax error\n");
    }
    return 0;
}

COMMAND(cmd_pci,
    ">pci                       PCI command\n"
    "-pci                       PCI status, receive any pending\n"
    "-pci ucfg                  Show local config space\n"
    "-pci ucfg wr aa xxxx       Write to config register\n"
    "-pci lmmi                  Show LMMI space\n"
    "-pci lmmi wr aa xxxx       Write to LMMI register\n"
    "-pci mem wr aa xx xx ...   Memory write transaction\n"
    "-pci mem rd aa             Memory read transaction\n"
    "-pci conf0 wr aa xx        Config write transaction\n"
    "-pci conf0 rd aa           Config read transaction\n"
    "-pci conf1 wr aa xx        Config write transaction\n"
    "-pci conf1 rd aa           Config read transaction\n"
)
