/* -------------------------------------------------------------------------
 *
 * File:        pi_socket.h
 *
 * Copyright:   2010 Delphi Technologies, Inc., All Rights Reserved
 *              Information Contained Herein Is Proprietary and Confidential
 *
 * Author:      Jacek Roman (jacek.roman(at)delphi.com)
 *
 * Description: Platform independent socket.h' functionality. 
 *
 * -------------------------------------------------------------------------*/

#ifndef PI_SOCKET_H
#define PI_SOCKET_H

#include "reuse.h"

#if defined (WIN32)
#include <Windows.h>
#else
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#endif /* WIN32 */


/* ------------------------------
 *  Public Function Definitions
 * ------------------------------ */

#define pi_socket_create(protocol_family, type, protocol)                   (int)socket(protocol_family, type, protocol)
#define pi_socket_bind(socket_fd, addr, addr_len)                           bind(socket_fd, addr, addr_len)
#define pi_socket_listen(socket_fd, backlog)                                listen(socket_fd, backlog)
#define pi_socket_accept(socket_fd, addr, addr_len)                         (int)accept(socket_fd, addr, addr_len)
#define pi_socket_connect(socket_fd, server_addr, server_addr_size)         connect(socket_fd, server_addr, server_addr_size)
#define pi_socket_recv(socket_fd, buffer, buffer_size, flags)               recv(socket_fd, buffer, buffer_size, flags)
#define pi_socket_send(socket_fd, buffer, buffer_size, flags)               send(socket_fd, buffer, buffer_size, flags)
#define pi_socket_sendto(socket_fd, buffer, buffer_size, flags, to, to_len) sendto(socket_fd, buffer, buffer_size, flags, to, to_len)

#if defined (WIN32)
#define pi_socket_close(s)                                          closesocket(s)
#define pi_socket_err()                                             WSAGetLastError()
#else
#define pi_socket_close(s)                                          close(s)
#define pi_socket_err()                                             errno
#endif /* WIN32 */

#endif /* PI_SOCKET_H */

/* -------------------------------------------------------------------------
 * Revision history:
 *
 * 09 August 2010, Jacek Roman (jacek.roman(at)delphi.com)
 *   Initial revision.
 *
 * -----------------------------------------------------------------------*/
