#define KBC_ST_REG 0x64
#define KBC_CMD_REG 0x64
#define KBC_OUT_BUF 0x60
#define OBF BIT(0)
#define IBF BIT(1)
#define AUX BIT(5)
#define KBD_INT BIT(0)
#define DIS_KBD_INTERFACE BIT(4)
#define DIS_MOUSE_INTERFACE BIT(5)
#define PAR_ERROR BIT(7)
#define TIME_ERROR BIT(6)
#define ESC 0x81

#define KBC_RCB 0X20
#define KBC_WCB 0X60
