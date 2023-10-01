/*!***************************************************************************************
\file			Debugger.cpp
\project
\author			Zacharie Hong

\par			Course: GAM300
\par			Section:
\date			25/09/2023

\brief
    This file contains a the definitions for a debugger using SpeedLogger

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#include <Precompiled.h>
#include "Debugger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/ostream_sink.h>
#include <spdlog/sinks/rotating_file_sink.h> // support for rotating file logging

Debugger::Debugger()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::trace);
    console_sink->set_pattern("[%T] [%^%l%$] %v");

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/Engine.txt", true);
    file_sink->set_level(spdlog::level::warn);
    file_sink->set_pattern("[source %s] [function %!] [line %#] [%H:%M:%S] %n:  %v");

    spdlog::sinks_init_list sink_list = { file_sink, console_sink };

    //spdlog::set_pattern("%^[%T] %n: %v%$");
    spdlog::set_pattern("[source %s] [function %!] [line %#] %v");
    consoleLogger = std::make_shared<spdlog::logger>("Bean Factory", sink_list.begin(), sink_list.end());
}

std::string Debugger::to_string(std::string msg)
{
    std::ostringstream _oss;
    auto ostream_logger = spdlog::get("gtest_logger");
    if (!ostream_logger)
    {
        auto ostream_sink = std::make_shared<spdlog::sinks::ostream_sink_st>(_oss);
        ostream_logger = std::make_shared<spdlog::logger>("gtest_logger", ostream_sink);
        ostream_logger->set_pattern(">%v<");
        ostream_logger->set_level(spdlog::level::trace);
    }
    spdlog::set_default_logger(ostream_logger);

    E_ASSERT(!ostream_logger, "Output stream Logger was not created properly");
    spdlog::trace(msg);
    std::string text = _oss.str();

    return text;
}

void Debugger::create_multiplefile(int fileSize,int fileAmount)
{
    int max_size = fileSize;
    int max_files = fileAmount;
    auto logger = spdlog::rotating_logger_mt("rotating_logger", "logs/rotating.txt",
                                max_size, max_files);

    E_ASSERT(logger.use_count() != 0, "Multiple file Logger was not created properly");
    logger->info("The purpose of this is to test the rotating file sink");
}

void Debugger::error_log()
{
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::warn);
    console_sink->set_pattern("[Error] [%^%l%$] %v");

    auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/Crashes.txt", true);
    file_sink->set_level(spdlog::level::trace);

    spdlog::sinks_init_list sink_list = { file_sink, console_sink };

    spdlog::logger logger("Error", sink_list.begin(), sink_list.end());
    logger.set_level(spdlog::level::trace);
    spdlog::set_default_logger(std::make_shared<spdlog::logger>("Error", spdlog::sinks_init_list({ console_sink, file_sink })));
}