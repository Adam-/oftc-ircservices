#include "stdinc.h"

dlink_list global_client_list;
dlink_list global_server_list;

client_t *
make_client()
{
  client_t *client = MyMalloc(sizeof(client_t));

  return client;
}

server_t *
make_server()
{
  server_t *server = MyMalloc(sizeof(server_t));
 
  return server;
}

/* find_person()
 *
 * inputs - pointer to name
 * output - return client pointer
 * side effects - find person by (nick)name
 */
client_t *
find_person(const client_t *source, const char *name)
{
  client_t *target = NULL;

  if(IsDigit(*name) && IsServer(source->from))
    target = hash_find_id(name);
  else
    target = find_client(name);

  return(target && IsClient(target)) ? target : NULL;
}

/*
 * dead_link_on_write - report a write error if not already dead,
 *      mark it as dead then exit it
 */
void
dead_link_on_write(client_t *client, int ierrno)
{
  dlink_node *ptr;

  if (IsDefunct(client->server))
    return;

  dbuf_clear(&client->server->buf_recvq);
  dbuf_clear(&client->server->buf_sendq);
}
