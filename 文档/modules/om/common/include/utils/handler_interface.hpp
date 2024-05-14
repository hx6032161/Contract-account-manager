/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Create a common interface for processing messages, alarms, events, etc;
 * @Author:Hen.You
 * @Date:
 *****************************************************************************/
#ifndef __HANDLER_INTERFACE_HPP__
#define __HANDLER_INTERFACE_HPP__

#include <string>

namespace common
{
// 创建一个通用的 消息、告警、事件等等 处理接口；
// 调用Handler对象的
// OnHandle接口完成，消息、告警、事件的处理动作，该函数缺省实现：先调用PreDo，在PreDo成功的情况下才执行DoAction动作；最后无论如何都会执行PostDo
// 缺省情况，接口使用者需要在DoAction接口中实现真正的处理动作；根据需要在PreDo中完成一些准备动作，如资源申请，消息解析、参数check等动作。
// 在PostDo中完成一些清理动作，如消息响应，资源释放、数据持久化，性能打点相关动作
class HanderItf
{
public:
    virtual int OnHandle(); // 返回值：0 表示成功； 其他失败

    // 获取被Handle对象的标识信息；例如：消息处理handler，则返回对应的消息ID；
    // 告警事件处理Handler，则返回告警事件的告警ID等等
    virtual std::string GetHandleObjId() { return ""; }

protected:
    // 缺省do nothing，返回值： 0表示成功，其他失败。事情处理前，准备工作，如资源申请，参数解析、check等；
    virtual int PreDo() { return 0; }

    virtual int DoAction() = 0; // 返回值： 0表示成功，其他失败

    // 缺省do nothing，返回值为0表示成功，其他失败。
    //     事情处理完成后，善后清理工作，如消息响应，资源释放、数据持久化，性能打点相关动作等；
    virtual int PostDo() { return 0; }
};

} // namespace common

#endif
