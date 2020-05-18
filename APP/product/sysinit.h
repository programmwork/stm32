/*
 * sysinit.h
 *
 *  Created on: 2016Äê3ÔÂ7ÈÕ
 *      Author: lenovo
 */

#ifndef  SOURCE_PRODUCT_SYSINIT_H_
#define  SOURCE_PRODUCT_SYSINIT_H_

#include "main.h"

void sysinit(void);

void pamsave(s_defdata_t *p_defdata_t);
unsigned char pam_init(s_defdata_t *p_defdata_t);
void tempdata_init(s_tempdata_t *p_tempdata_t);

void save_sys_cfg(bcm_info_t *p_bcm_info);
void read_sys_cfg(bcm_info_t *p_bcm_info);
unsigned char sys_cfg_init(void);
void startupprint(void);
    

#endif
