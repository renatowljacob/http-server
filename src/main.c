// TODO: Turn main into a arg parse function and transfer its contents to
// another function

#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "base_arena.h"
#include "base_core.h"
#include "base_message.h"
#include "base_string.h"

#define DEFAULT_ERR_LEN 300
#define DEFAULT_PORT    8080
#define DEFAULT_S_ADDR  INADDR_ANY

#define MAX_REQUESTS      32
#define MAX_MESSAGE_BYTES MB(2)

enum ERROR
{
    SOCKET_CONN_ERR = 1,
    SOCKET_BIND_ERR,
    SOCKET_LISTEN_ERR,
};

struct Fildes
{
    i32 handles[2];
    i32 index;
};

struct ProgramState
{
    struct Fildes *fildes;
    i16 exit_code;
};

struct RequestState
{
    Arena *arena;
    u16 status_code;
};

struct ErrorState
{
    Arena *arena;
    MessageList *msg_list;
    u32 index;
};

void fildes_cleanup(struct Fildes *fildes);

int
main(void)
{
    struct ProgramState state_program = { 0 };
    struct RequestState state_request = { 0 };
    struct ErrorState state_error = { .index = 1 };

    Arena arena_request = arena_new(.size = MB(2));
    state_request.arena = &arena_request;

    Arena arena_msgs = arena_new();
    state_error.arena = &arena_msgs;

    // Dummy Head
    state_error.msg_list->head =
      arena_alloc(state_error.arena, sizeof(MessageNode));
    state_error.msg_list->head->list.next = &state_error.msg_list->head->list;
    state_error.msg_list->head->message = nil_string;
    state_error.msg_list->tail = state_error.msg_list->head;

    struct Fildes fildes = { 0 };
    state_program.fildes = &fildes;

    state_program.fildes->handles[state_program.fildes->index] =
      socket(AF_INET, SOCK_STREAM, 0);

    // Consider using a handler for getting the file descriptor
    if (state_program.fildes->handles[state_program.fildes->index])
    {
        MessageNode *msg =
          message_new(state_error.arena, state_error.index++, errno);
        fprintf(
          stderr,
          "%s\n",
          message_alloc(
            DEFAULT_MESSAGE(state_error.arena, msg, msg->status_code)
          )
        );
        state_program.exit_code = (i16)msg->status_code;

        goto cleanup;
    }

    i32 opt = 1;
    setsockopt(
      state_program.fildes->handles[state_program.fildes->index],
      SOL_SOCKET,
      SO_REUSEADDR,
      &opt,
      sizeof(opt)
    );

    struct sockaddr_in saddr = {
        .sin_family = AF_INET,
        .sin_port = htons(DEFAULT_PORT),
        .sin_addr = { .s_addr = htonl(DEFAULT_S_ADDR) },
    };
    socklen_t saddr_size = sizeof(saddr);

    // Bind socket to some address
    if (bind(
          state_program.fildes->handles[state_program.fildes->index],
          (struct sockaddr *)&saddr,
          saddr_size
        ) == -1)
    {
        MessageNode *msg =
          message_new(state_error.arena, state_error.index++, errno);
        fprintf(
          stderr,
          "%s\n",
          message_alloc(
            DEFAULT_MESSAGE(state_error.arena, msg, msg->status_code)
          )
        );
        state_program.exit_code = (i16)msg->status_code;

        goto cleanup;
    }

    if (listen(state_program.fildes->handles[state_program.fildes->index], 0) ==
        -1)
    {
        MessageNode *msg =
          message_new(state_error.arena, state_error.index++, errno);
        fprintf(
          stderr,
          "%s\n",
          message_alloc(
            DEFAULT_MESSAGE(state_error.arena, msg, msg->status_code)
          )
        );
        state_program.exit_code = (i16)msg->status_code;

        goto cleanup;
    }
    printf("Listening on port %d.\n", DEFAULT_PORT);

    state_program.fildes->index++;
    while (true)
    {
        arena_reset(state_request.arena);

        state_program.fildes->handles[state_program.fildes->index] = accept(
          state_program.fildes->handles[0],
          (struct sockaddr *)&saddr,
          &saddr_size
        );
        if (state_program.fildes->handles[state_program.fildes->index] == -1)
        {
            MessageNode *msg =
              message_new(state_error.arena, state_error.index++, errno);
            fprintf(
              stderr,
              "%s\n",
              message_alloc(
                DEFAULT_MESSAGE(state_error.arena, msg, msg->status_code)
              )
            );
            slist_append(&msg->list, &state_error.msg_list->tail->list);

            continue;
        }

        String request = {
            .value = arena_alloc(state_request.arena, MAX_MESSAGE_BYTES + 1)
        };

        ssize_t request_size = read(
          state_program.fildes->handles[state_program.fildes->index],
          (void *)request.value,
          MAX_MESSAGE_BYTES
        );
        if (request_size < 0)
        {
            MessageNode *msg =
              message_new(state_error.arena, state_error.index++, errno);
            fprintf(
              stderr,
              "%s\n",
              message_alloc(
                DEFAULT_MESSAGE(state_error.arena, msg, msg->status_code)
              )
            );
            slist_append(&msg->list, &state_error.msg_list->tail->list);

            continue;
        }
        request.len = (size_t)request_size;

        // Handle request here

        close(state_program.fildes->handles[state_program.fildes->index]);
    }

cleanup:
    fildes_cleanup(state_program.fildes);
    arena_free(state_request.arena);
    arena_free(state_error.arena);
}

void
fildes_cleanup(struct Fildes *fildes)
{
    while (fildes->index >= 0)
    {
        close(fildes->handles[fildes->index--]);
    }
}
