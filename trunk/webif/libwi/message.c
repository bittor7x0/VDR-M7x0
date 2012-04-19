/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#include <u/libu.h>

#include "message.h"

const char *messageCssClass[] = {"messageAlert", "messageInfo"};

void initMessage(message_t *message) {
    message->type = MT_INFO;
    message->title = NULL;
    message->msg = NULL;
    message->requieresHtmlEncoding = false;
}

void freeMessage(message_t *message) {
    free(message->title);
    free(message->msg);
    initMessage(message);
}

void initMessageList(messageList_t *messages) {
    messages->length = 0;
    messages->entry = NULL;
}

void freeMessageList(messageList_t *messages) {
    message_t *message;
    int i;
    for (i = 0, message = messages->entry; i < messages->length; i++, message++) {
        freeMessage(message);
    }
    free(messages->entry);
    initMessageList(messages);
}

message_t *newMessage(messageType_t type, const char * title, const char *msg, ...) {
    va_list ap;
    message_t *message = malloc(sizeof (message_t));
    initMessage(message);
    message->type = type;
    message->title = (title) ? strdup(title) : NULL;
    if (msg) {
        va_start(ap, msg);
        vasprintf(&message->msg, msg, ap);
        va_end(ap);
    }
    return message;
outOfMemory:
    exit(EXIT_FAILURE);
}

message_t *addMessage(messageList_t* messages, messageType_t type, const char * title, const char *msg, ...) {
    va_list ap;
    messages->entry = realloc(messages->entry, (messages->length + 1) * sizeof (message_t));
    crit_goto_if(messages->entry == NULL, outOfMemory);
    message_t *message = messages->entry + messages->length;
    messages->length++;
    initMessage(message);
    message->type = type;
    message->title = (title) ? strdup(title) : NULL;
    if (msg) {
        va_start(ap, msg);
        vasprintf(&message->msg, msg, ap);
        va_end(ap);
    }
    return message;
outOfMemory:
    exit(EXIT_FAILURE);
}


