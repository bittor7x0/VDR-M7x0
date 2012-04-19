/**
 * This source-code is licensed under GPL v2.
 * See ../../LICENSE for details
 * 
 * @author: atinar <atinar1@hotmail.com>
 * 
 */

#ifndef WI_MESSAGE_H
#define	WI_MESSAGE_H

#include <stdbool.h>

#ifdef	__cplusplus
extern "C" {
#endif

    typedef enum messageType_e {
        MT_ALERT,
        MT_INFO
    } messageType_t;

    extern const char *messageCssClass[]; //indexed by messageType_t

    typedef struct message_s {
        messageType_t type;
        char *title;
        char *msg;
        bool requieresHtmlEncoding;
    } message_t;

    typedef struct messageList_s {
        int length;
        message_t *entry;
    } messageList_t;

    void initMessage(message_t *message);
    void freeMessage(message_t *message);
    void initMessageList(messageList_t *messages);
    void freeMessageList(messageList_t *messages);
    message_t *newMessage(messageType_t type, const char *title, const char *msg, ...)
    __attribute__((format(printf, 3, 4)));
    message_t *addMessage(messageList_t* messages, messageType_t type, const char *title, const char *msg, ...);

#ifdef	__cplusplus
}
#endif

#endif

