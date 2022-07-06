#pragma once

#include <string>
#include <iostream>

#define PIC_BUFFER_SIZE (1024*1024*3) //3M

void InitUdp();

bool SendUdp(void* data, int dataLen);

void CloseUdp();
