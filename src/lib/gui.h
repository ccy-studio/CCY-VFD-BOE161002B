/*
 * @Description: 
 * @Blog: saisaiwa.com
 * @Author: ccy
 * @Date: 2024-06-03 09:21:08
 * @LastEditTime: 2024-06-03 16:01:48
 */
#ifndef __VFD_GUI_
#define __VFD_GUI_

#include "pt6315.h"

/**
 * BOE VFD16 1002B
 * 10 GR
 * 16 P
 */

// VFDλ��
#define VFD_DIG_LEN 10

#define PWM_ARR 1000  // ����ֵ
#define PWM_CCR 65   // �Ƚ�ֵ

/**
 * ��ʼ��
 */
void vfd_gui_init();

/**
 * ֹͣ�ر���ʾ����˿��ֹͣ����
 */
void vfd_gui_stop();

/**
 * ���VFD��Ļ��ʾ,ѭ��ˢ�����ʹ��vfd_gui_set_text��������Ҫʹ������
 */
void vfd_gui_clear();

/**
 * ��ʾһ�����֣���0λ��ʼ��
 * (�Զ���ո�����ʾ������ÿ�β��õ���clear��ֹ��������)
 * @param colon �Ƿ���ʾð��
 */
void vfd_gui_set_text(char* string, const u8 point,const u8 icon);

/**
 * ��ָ����DIGλ����buf��
 */
void vfd_gui_set_dig(u8 dig, u32 buf);

/**
 * �������ȵȼ� 1~7
 */
void vfd_gui_set_blk_level(size_t level);

/**
 * acg����
 */
void vfd_gui_acg_update();

/**
 * ��Ļ��������
 */
void vfd_gui_display_protect_exec();

long map(long x, long in_min, long in_max, long out_min, long out_max);
#endif