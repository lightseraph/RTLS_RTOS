#ifndef __DWAPP_H
#define __DWAPP_H

#include "main.h"
#include "config_options.h"
#include "instance.h"
#include "kalman.h"

void DW_Init_Task(void *argument);
void DW_Main_Task(void *argument);

#endif