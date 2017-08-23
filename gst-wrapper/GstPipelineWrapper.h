/*
 * Copyright (c) 2014 Samsung Electronics Corporation.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *    http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <string>

#include <boost/thread.hpp>
#include <boost/thread/lock_guard.hpp>
#include <gst/gst.h>

class GstPipelineWrapper {
public:
    GstPipelineWrapper(void);
    ~GstPipelineWrapper(void);

    void InitializePipelineWithString(std::string pipelineString);
    GstElement* GetElementByName(std::string element_name);

    void set_is_verbose(bool is_verbose);
    bool get_is_verbose() const;

    bool SetPipelineState(GstState state);
    GstState GetPipelineState();

private:
    bool is_verbose_; // default: false

    GstElement* pipeline_;
    GstBus* bus_;

    boost::thread g_main_loop_thread_;
    void RunningMainLoop();

    void FreePipeline();

    static bool GstMessageParser(GstBus* bus, GstMessage* msg, GstPipelineWrapper* pipeline);
    static void EnsureGstreamerInitialization();
};
