#ifndef BACKEND_H
#define BACKEND_H

#include "tegra.h"
#include "util.h"

#define trigger_vulnerability GLUE(BACKEND, trigger_vulnerability)
int trigger_vulnerability(tegra_handle*, uint16_t);

#endif