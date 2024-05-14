/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:
 * @Author:chuanyan
 * @Date:2022-11-21
 *****************************************************************************/
#ifndef ZLOG_EXPORT_H
#define ZLOG_EXPORT_H

#ifdef ZLOG_STATIC_DEFINE
#define ZLOG_EXPORT
#define ZLOG_NO_EXPORT
#else
#ifndef ZLOG_EXPORT
#ifdef ZLOG_IS_A_DLL
/* We are building this library */
#define ZLOG_EXPORT __attribute__((visibility("default")))
#else
/* We are using this library */
#define ZLOG_EXPORT __attribute__((visibility("default")))
#endif
#endif

#ifndef ZLOG_NO_EXPORT
#define ZLOG_NO_EXPORT __attribute__((visibility("hidden")))
#endif
#endif

#ifndef ZLOG_DEPRECATED
#define ZLOG_DEPRECATED(msg) __attribute__((__deprecated__(msg)))
#endif

#ifndef ZLOG_DEPRECATED_EXPORT
#define ZLOG_DEPRECATED_EXPORT ZLOG_EXPORT ZLOG_DEPRECATED
#endif

#ifndef ZLOG_DEPRECATED_NO_EXPORT
#define ZLOG_DEPRECATED_NO_EXPORT ZLOG_NO_EXPORT ZLOG_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#ifndef ZLOG_NO_DEPRECATED
#define ZLOG_NO_DEPRECATED(prompt) [[deprecated(prompt)]]
#endif
#endif

#endif /* ZLOG_EXPORT_H */
