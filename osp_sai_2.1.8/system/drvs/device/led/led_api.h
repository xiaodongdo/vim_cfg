#ifndef __LED_API_H__
#define __LED_API_H__

#include "led_drv.h"


enum led_api_mode_e
{
    LED_MODE_1_RXLNK_BIACT,       /* One bit, rxlink + biactivity */
    LED_MODE_1_FORCE_ON,          /* One bit, force on */
    LED_MODE_1_FORCE_OFF,         /* One bit, force off */
    LED_MODE_1_BIACT,             /* One bit, biactivity */
    
    LED_MODE_2_RXLNK_BIACT,       /* Two bits, first: rxlink; second: biactivity */
    LED_MODE_2_OFF_RXLNKBIACT,    /* Two bits, first: off; second: rxlink + biactivity */
    LED_MODE_2_RXLNKBIACT_OFF,    /* Two bits, first: rxlink + biactivity; second: off */
    LED_MODE_2_FORCE_OFF,         /* Two bits, all force off */
    LED_MODE_2_OFF_BIACT,         /* Two bits, first: off; second: biactivity */
    LED_MODE_2_BIACT_OFF,         /* Two bits, first: biactivity; second: off */
    LED_MODE_2_OFF_ON,            /* Two bits, first: force off; second: force on */
    LED_MODE_2_ON_OFF,            /* Two bits, first: force on; second: force off */

    LED_MODE_2_TXLNK_RXLNKBIACT,  /* Two bits, first: txlink(rxlink+fault); second: rxlink+biactivity */
    LED_MODE_2_TXLNK_BIACT,       /* Two bits, first: txlink(rxlink+fault); second: biactivity */
    LED_MODE_2_TXLNK_OFF,         /* Two bits, first: txlink(rxlink+fault); second: force off */
    LED_MODE_2_RXLNKBIACT_TXLNK,  /* Two bits, first: rxlink+biactivity; second: txlink(rxlink+fault) */
    LED_MODE_2_BIACT_TXLNK,       /* Two bits, first: biactivity; second: txlink(rxlink+fault) */
    LED_MODE_2_OFF_TXLNK,         /* Two bits, first: force off; second: txlink(rxlink+fault) */
    
    LED_MAX_LED_MODE,
};
typedef enum led_api_mode_e led_api_mode_t;

struct gb_mac_led_api_para_s
{
    uint16 port_id;  /* High 8-bit indicates chip id, low 8-bit indicate logic port id */
    uint8  lchip;    /* local chip id */
    uint8  ctl_id;   /* GG slice id*/
    led_api_mode_t mode;
    uint8  fixed;    /* 1 means the para is fixed, 0 means the para can be changed */
};
typedef struct gb_mac_led_api_para_s mac_led_api_para_t;

struct mac_led_info_s
{
    uint8 table_num;        /* table num */
    uint8 slice0_mac_num;   /* mac numbers belong to slice0 */
    uint8 mac_num;          /* mac numbers belong to slice0 and slice1 */
    uint8 polarity;
    mac_led_api_para_t** mac_led_api_para;
};
typedef struct mac_led_info_s mac_led_info_t;

int32 mac_led_init();
int32 led_api_op(glb_led_type_t type, glb_led_stat_t led_stat);
int32 led_init(led_info_t* p_led_info);
int32 led_cfg_port_mode(mac_led_api_para_t* port_led);
int32 mac_led_info_register(mac_led_info_t* p_mac_led_info);
int32 led_mgt_port_led();
int32 led_linkup_process(uint8 lchip, uint16 gport);

#endif
