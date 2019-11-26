/*  os.c
 *  Part of xfce4-cpugraph-plugin
 *
 *  Copyright (c) Alexander Nordfelth <alex.nordfelth@telia.com>
 *  Copyright (c) gatopeich <gatoguan-os@yahoo.com>
 *  Copyright (c) 2007-2008 Angelo Arrifano <miknix@gmail.com>
 *  Copyright (c) 2007-2008 Lidiriel <lidiriel@coriolys.org>
 *  Copyright (c) 2010 Florian Rivoal <frivoal@gmail.com>
 *  Copyright (c) 2010 Peter Tribble <peter.tribble@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "os.h"

#include <stdio.h>
#include <string.h>

#define NV_SMI "nvidia-smi --query-gpu=utilization.gpu --format=csv"
#define NV_SMI_QUERY_HEADER "utilization.gpu [%]"
#define NV_SMI_QUERY_HEADER_LEN 19
#define QUERYMAXLEN 256


guint
detect_cpu_number ()
{
    FILE *fp;
    gchar query_string[QUERYMAXLEN];
    guint gpu_count = 0;

    fp = popen(NV_SMI, "r");
    if (fp == NULL)
        return 0;


    if ((fgets(query_string, sizeof(query_string), fp) != NULL) &&	
	    (strncmp (query_string, NV_SMI_QUERY_HEADER, NV_SMI_QUERY_HEADER_LEN) == 0))
    {

        while (fgets(query_string, sizeof(query_string), fp) != NULL) 
            gpu_count++;
    }

    pclose(fp);

    return gpu_count;
}

gboolean
read_cpu_data_gpu (CpuData *data, guint nb_cpu)
{
    FILE *fp;
    gchar query_string[QUERYMAXLEN];
    guint line = 0;
    guint utilization;

    fp = popen(NV_SMI, "r");
    if (fp == NULL)
    return FALSE;

    if ((fgets(query_string, sizeof(query_string), fp) != NULL) &&	
	    (strncmp (query_string, NV_SMI_QUERY_HEADER, NV_SMI_QUERY_HEADER_LEN) == 0))
    {

        while ((fgets(query_string, sizeof(query_string), fp) != NULL) &&
                (line < nb_cpu))
        {
            
            if (sscanf (query_string, "%u %%", &utilization) < 1)
            {
                pclose(fp);
                return FALSE;
            }
                
            data[line].load = utilization;
            line++;
        }

    }

    pclose(fp);

    return TRUE;
}

