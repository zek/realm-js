////////////////////////////////////////////////////////////////////////////
//
// Copyright 2017 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#pragma once

#include <realm/util/logger.hpp>

#include "sync/sync_manager.hpp"
#include "event_loop_dispatcher.hpp"

namespace realm {
namespace js {

template <typename JSEngine>
class JSLogger : public realm::util::RootLogger {
    using GlobalContextType = typename JSEngine::GlobalContext;
    using ValueType = typename JSEngine::Value;
    using ObjectType = typename JSEngine::Object;
    using FunctionType = typename JSEngine::Function;
public:
    JSLogger(Protected<GlobalContextType> ctx, Protected<FunctionType> callback)
        : m_ctx(ctx)
        , m_callback(callback)
        , m_log_dispatcher(std::bind(&JSLogger<JSEngine>::do_log_in_js, this,std::placeholders::_1, std::placeholders::_2))
    {
    }

protected:
    void do_log(realm::util::Logger::Level level, std::string message) override final
    {
        m_log_dispatcher(level, std::move(message));
    }

private:
    void do_log_in_js(realm::util::Logger::Level level, std::string message)
    {
        HANDLESCOPE
        ValueType arguments[2];
        arguments[0] = Value<JSEngine>::from_number(m_ctx, static_cast<int>(level));
        arguments[1] = Value<JSEngine>::from_string(m_ctx, message);
        Function<JSEngine>::callback(m_ctx, m_callback, ObjectType(), 2, arguments);
    }

    Protected<GlobalContextType> m_ctx;
    Protected<FunctionType> m_callback;
    EventLoopDispatcher<void(realm::util::Logger::Level, std::string)> m_log_dispatcher;
};

template <typename JSEngine>
class JSLoggerFactory : public SyncLoggerFactory {
    using GlobalContextType = typename JSEngine::GlobalContext;
    using ContextType = typename JSEngine::Context;
    using FunctionType = typename JSEngine::Function;
public:
    JSLoggerFactory(ContextType ctx, FunctionType callback)
        : m_ctx(Context<JSEngine>::get_global_context(ctx))
        , m_callback(ctx, callback)
    {
    }

    virtual std::unique_ptr<util::Logger> make_logger(util::Logger::Level level)
    {
        auto* logger = new JSLogger<JSEngine>(m_ctx, m_callback);
        logger->set_level_threshold(level);

        return std::unique_ptr<util::Logger>(logger);
    }
private:
    Protected<GlobalContextType> m_ctx;
    Protected<FunctionType> m_callback;
};

} // js
} // realm