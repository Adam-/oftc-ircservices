/*
 *  oftc-ircservices: an extensible and flexible IRC Services package
 *  akill.c - akill related functions
 *
 *  Copyright (C) 2006 Stuart Walsh and the OFTC Coding department
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 *  USA
 *
 *  $Id: dbm.c 1260 2007-12-07 08:53:17Z swalsh $
 */

#include "stdinc.h"

static int
akill_check_mask(struct Client *client, const char *mask)
{
  struct irc_ssaddr addr;
  struct split_nuh_item nuh;
  char name[NICKLEN];
  char user[USERLEN + 1];
  char host[HOSTLEN + 1];
  int type, bits, found = 0;

  DupString(nuh.nuhmask, mask);
  nuh.nickptr  = name;
  nuh.userptr  = user;
  nuh.hostptr  = host;

  nuh.nicksize = sizeof(name);
  nuh.usersize = sizeof(user);
  nuh.hostsize = sizeof(host);

  split_nuh(&nuh);

  type = parse_netmask(host, &addr, &bits);

  if(match(name, client->name) && match(user, client->username))
  {
    switch(type)
    {
      case HM_HOST:
        if(match(host, client->host))
          found = 1;
        break;
      case HM_IPV4:
        if (client->aftype == AF_INET)
          if (match_ipv4(&client->ip, &addr, bits))
            found = 1;
        break;
#ifdef IPV6
      case HM_IPV6:
        if (client->aftype == AF_INET6)
          if (match_ipv6(&client->ip, &addr, bits))
            found = 1;
        break;
#endif
    }
  }
  MyFree(nuh.nuhmask);
  return found;
}

int
akill_check_client(struct Service *service, struct Client *client)
{
  result_set_t *results;
  int error;
  int i;

  results = db_execute(GET_AKILLS, 0, &error);
  if(results == NULL && error != 0)
  {
    ilog(L_CRIT, "akill_check_client: database error %d", error);
    return FALSE;
  }
  else if(results == NULL)
    return FALSE;

  if(results->row_count == 0)
    return FALSE;

  for(i = 0; i < results->row_count; i++)
  {
    if(akill_check_mask(client, results->rows[i].cols[2]))
    {
      char *setter = db_get_nickname_from_id(atoi(results->rows[i].cols[1]));

//      send_akill(service, setter, sban);
      MyFree(setter);

      return TRUE;
    }
  }
  return FALSE;
}
