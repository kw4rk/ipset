/* Copyright 2008 Jozsef Kadlecsik (kadlec@blackhole.kfki.hu)
 *
 * This program is free software; you can redistribute it and/or modify   
 * it under the terms of the GNU General Public License as published by   
 * the Free Software Foundation; either version 2 of the License, or      
 * (at your option) any later version.                                    
 *                                                                         
 * This program is distributed in the hope that it will be useful,        
 * but WITHOUT ANY WARRANTY; without even the implied warranty of         
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          
 * GNU General Public License for more details.                           
 *                                                                         
 * You should have received a copy of the GNU General Public License      
 * along with this program; if not, write to the Free Software            
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <linux/netfilter_ipv4/ip_set_setlist.h>
#include "ipset.h"

/* Initialize the create. */
static void
create_init(void *data)
{
	struct ip_set_req_setlist_create *mydata = data;
	
	mydata->size = 8;
}

/* Function which parses command options; returns true if it ate an option */
static int
create_parse(int c, char *argv[], void *data, unsigned *flags)
{
	struct ip_set_req_setlist_create *mydata = data;
	unsigned int size;
	
	switch (c) {
	case '1':
		if (string_to_number(optarg, 1, 255, &size))
			exit_error(PARAMETER_PROBLEM,
				   "Invalid size '%s specified: must be "
				   "between 1-255", optarg);
		mydata->size = size;
		break;
	default:
		return 0;
	}
	return 1;
}

/* Final check; exit if not ok. */
static void
create_final(void *data, unsigned int flags)
{
}

/* Create commandline options */
static const struct option create_opts[] = {
	{.name = "size",	.has_arg = required_argument,	.val = '1'},
	{NULL},
};

static void check_setname(const char *name)
{
	if (strlen(name) > IP_SET_MAXNAMELEN - 1)
		exit_error(PARAMETER_PROBLEM,
			"Setname %s is longer than %d characters.",
			name, IP_SET_MAXNAMELEN - 1);
}

/* Add, del, test parser */
static ip_set_ip_t
adt_parser(unsigned cmd, const char *optarg, void *data)
{
	struct ip_set_req_setlist *mydata = data;
	char *saved = ipset_strdup(optarg);
	char *ptr, *tmp = saved;
	
	DP("setlist: %p %p", optarg, data);

	ptr = strsep(&tmp, ",");
	check_setname(ptr);
	strcpy(mydata->name, ptr);
	
	if (!tmp) {
		mydata->before = 0;
		mydata->ref[0] = '\0';
		return 1;
	}
	
	ptr = strsep(&tmp, ",");
	
	if (tmp == NULL || !(strcmp(ptr, "before") == 0 || strcmp(ptr, "after") == 0))
		exit_error(PARAMETER_PROBLEM,
			"Syntax error, you must specify elements as setname,[before|after],setname");

	check_setname(tmp);
	strcpy(mydata->ref, tmp);
	mydata->before = !strcmp(ptr, "before");

	free(saved);

	return 1;	
}

/*
 * Print and save
 */

static void
initheader(struct set *set, const void *data)
{
	const struct ip_set_req_setlist_create *header = data;
	struct ip_set_setlist *map = set->settype->header;
		
	memset(map, 0, sizeof(struct ip_set_setlist));
	map->size = header->size;
}

static void
printheader(struct set *set, unsigned options)
{
	struct ip_set_setlist *mysetdata = set->settype->header;

	printf(" size: %u\n", mysetdata->size);
}

static void
printips_sorted(struct set *set, void *data, size_t len, unsigned options)
{
	struct ip_set_setlist *mysetdata = set->settype->header;
	int i;
	ip_set_id_t id;
	struct set *elem;

	for (i = 0; i < mysetdata->size; i++ ) {
		id = *((ip_set_id_t *)data + i);
		if (id == IP_SET_INVALID_ID)
			return;
		elem = set_find_byid(id);
		printf("%s\n", elem->name);
	}
}

static void
saveheader(struct set *set, unsigned options)
{
	struct ip_set_setlist *mysetdata = set->settype->header;

	printf("-N %s %s --size %u\n",
	       set->name, set->settype->typename,
	       mysetdata->size);
}

static void
saveips(struct set *set, void *data, size_t len, unsigned options)
{
	struct ip_set_setlist *mysetdata = set->settype->header;
	int i;
	ip_set_id_t id;
	struct set *elem;

	for (i = 0; i < mysetdata->size; i++ ) {
		id = *((ip_set_id_t *)data + i);
		if (id == IP_SET_INVALID_ID)
			return;
		elem = set_find_byid(id);
		printf("-A %s %s\n", set->name, elem->name);
	}
}

static void usage(void)
{
	printf
	    ("-N set setlist --size size\n"
	     "-A set setname[,before|after,setname]\n"
	     "-D set setname\n"
	     "-T set setname\n");
}

static struct settype settype_setlist = {
	.typename = SETTYPE_NAME,
	.protocol_version = IP_SET_PROTOCOL_VERSION,

	/* Create */
	.create_size = sizeof(struct ip_set_req_setlist_create),
	.create_init = &create_init,
	.create_parse = &create_parse,
	.create_final = &create_final,
	.create_opts = create_opts,

	/* Add/del/test */
	.adt_size = sizeof(struct ip_set_req_setlist),
	.adt_parser = &adt_parser,

	/* Printing */
	.header_size = sizeof(struct ip_set_setlist),
	.initheader = &initheader,
	.printheader = &printheader,
	.printips = &printips_sorted,	/* We only have sorted version */
	.printips_sorted = &printips_sorted,
	.saveheader = &saveheader,
	.saveips = &saveips,
	
	.usage = &usage,
};

void _init(void)
{
	settype_register(&settype_setlist);

}
