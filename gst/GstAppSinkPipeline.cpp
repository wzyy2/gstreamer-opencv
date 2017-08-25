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

#include <boost/thread/lock_guard.hpp>

#include "GstAppSinkPipeline.h"
#include "GstreamerPipelines.h"

GstAppSinkPipeline::GstAppSinkPipeline()
    : retrievedBuffer(0)
    , currentBuffer(0)
    , is_streaming_(true)
{

}

GstAppSinkPipeline::~GstAppSinkPipeline()
{

}

void GstAppSinkPipeline::Initialize(std::string pipelineString)
{
    GstPipelineWrapper::InitializePipelineWithString(pipelineString);

    // setup appsink
    appsink = GstPipelineWrapper::GetElementByName(APPSINK_NAME);
    GstAppSinkCallbacks appsinkCallbacks;
    appsinkCallbacks.new_preroll = &GstAppSinkPipeline::NewPrerollCallback;
    appsinkCallbacks.new_sample = &GstAppSinkPipeline::NewSampleCallback;
    appsinkCallbacks.eos = &GstAppSinkPipeline::EndOfStreamCallback;

    gst_app_sink_set_drop(GST_APP_SINK(appsink), true);
    gst_app_sink_set_max_buffers(GST_APP_SINK(appsink), 1);
    //gst_app_sink_set_emit_signals	(GST_APP_SINK(appsink), true);
    gst_app_sink_set_callbacks(GST_APP_SINK(appsink), &appsinkCallbacks, this, (GDestroyNotify)GstAppSinkPipeline::DestroyCallback);
}

void GstAppSinkPipeline::EndOfStreamCallback(GstAppSink* appsink, gpointer user_data)
{
    
}

GstFlowReturn GstAppSinkPipeline::NewPrerollCallback(GstAppSink* appsink, gpointer user_data)
{
    GstSample* sample = gst_app_sink_pull_preroll(appsink);
    gst_sample_unref(sample);
    return GST_FLOW_OK;
}

GstFlowReturn GstAppSinkPipeline::NewSampleCallback(GstAppSink* appsink, gpointer user_data)
{
    ((GstAppSinkPipeline*)user_data)->ReceiveNewSample();
    return GST_FLOW_OK;
}

void GstAppSinkPipeline::DestroyCallback(gpointer user_data)
{
    std::cout << "DESTROY" << std::endl;
}

void GstAppSinkPipeline::set_is_streaming(bool is_streaming)
{
    boost::lock_guard<boost::mutex> guard(bufferMutex);
    is_streaming_ = is_streaming;

    if (is_streaming_)
        return;
  
    if (currentBuffer)
        gst_sample_unref(currentBuffer);
    currentBuffer = 0;
}

void GstAppSinkPipeline::ReceiveNewSample()
{
    if (!is_streaming_)
        return;

    GstSample* sample = gst_app_sink_pull_sample(GST_APP_SINK(appsink));

    if (sample) {
        boost::lock_guard<boost::mutex> guard(bufferMutex);
        if (currentBuffer != 0) // release if not empty
        {
            gst_sample_unref(currentBuffer);
        }

        currentBuffer = sample;
    }
}

void GstAppSinkPipeline::ReleaseFrameBuffer()
{
    boost::lock_guard<boost::mutex> guard(bufferMutex);
    if (retrievedBuffer)
        gst_sample_unref(retrievedBuffer);
    retrievedBuffer = 0;
}

bool GstAppSinkPipeline::GetIsNewFrameAvailable()
{
    boost::lock_guard<boost::mutex> guard(bufferMutex);
    return ((retrievedBuffer == 0) && (currentBuffer != 0));
}

bool GstAppSinkPipeline::GetLatestFrameBuffer(void** frameBuffer)
{
    bool retrieving = false;

    boost::lock_guard<boost::mutex> guard(bufferMutex);
    if (retrievedBuffer == 0) {
        if (currentBuffer != 0) {
            retrievedBuffer = currentBuffer;
            currentBuffer = 0;
            retrieving = true;
        }
    }

    if (retrieving) {

        GstBuffer* buffer;
        GstMapInfo map;

        buffer = gst_sample_get_buffer(retrievedBuffer);

        if (buffer) {
            gst_buffer_map(buffer, &map, GST_MAP_READ);
            (*frameBuffer) = map.data;
            gst_buffer_unmap(buffer, &map);

        } else
            return false;
    }

    return true;
}

bool GstAppSinkPipeline::GetLatestSample(GstSample** sample)
{
    boost::lock_guard<boost::mutex> guard(bufferMutex);
    if (retrievedBuffer == 0) {
        if (currentBuffer != 0) {
            retrievedBuffer = currentBuffer;
            currentBuffer = 0;
            (*sample) = retrievedBuffer;
        }
    } else {
        (*sample) = NULL;
        return false;  
    }

    return true;
}
