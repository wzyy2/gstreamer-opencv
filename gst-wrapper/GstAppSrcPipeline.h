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

#include "GstPipelineWrapper.h"
#include <gst/app/gstappsrc.h>

class GstAppSrcPipeline : public GstPipelineWrapper {
public:
    GstAppSrcPipeline();
    ~GstAppSrcPipeline(void);

    void Initialize(std::string pipelineString);

    bool GetNeedsData() const;
    bool SendFrame(unsigned char* buffer, unsigned int bufferSize);
    bool SendBUF(GstBuffer* buffer);

private:
    boost::mutex pushBufferFlagMutex;

    // Callbacks for various signals
    static void NeedDataCallback(GstAppSrc* appsrc, guint length, gpointer user_data);
    static void EnoughDataCallback(GstAppSrc* appsrc, gpointer user_data);
    static gboolean SeekDataCallback(GstAppSrc* appsrc, guint64 offset, gpointer user_data);

    static bool ParseMessageCallback(GstBus* bus, GstMessage* msg, GstAppSrcPipeline* multicaster);

    static void DestroyCallback(GstAppSrcPipeline* multicaster);

    GstElement* appsrc;

    void FreePipeline();

    bool needsData;
};
