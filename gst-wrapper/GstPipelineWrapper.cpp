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

#include "GstPipelineWrapper.h"

GstPipelineWrapper::GstPipelineWrapper(void)
    : is_verbose_(false)
{
    EnsureGstreamerInitialization();
}

GstPipelineWrapper::~GstPipelineWrapper(void)
{
    FreePipeline();
}

void GstPipelineWrapper::InitializePipelineWithString(std::string pipelineString)
{
    /// Init pipeline using string
    GError* err = 0;
    pipeline_ = gst_parse_launch(pipelineString.c_str(), &err);

    /// Get bus and add message parser
    bus_ = gst_pipeline_get_bus(GST_PIPELINE(pipeline_));
    gst_bus_add_watch(bus_, (GstBusFunc)GstPipelineWrapper::GstMessageParser, this);

    /// Run the main loop to receive messages from bus
    g_main_loop_thread_ = boost::thread(&GstPipelineWrapper::RunningMainLoop, this);
}

GstElement* GstPipelineWrapper::GetElementByName(std::string element_name)
{
    return gst_bin_get_by_name(GST_BIN(pipeline_), element_name.c_str());
}

bool GstPipelineWrapper::SetPipelineState(GstState state)
{
    GstStateChangeReturn ret = gst_element_set_state(pipeline_, state);

    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state");
        gst_object_unref(pipeline_);
        return false;
    }
    return true;
}

GstState GstPipelineWrapper::GetPipelineState()
{
    GstState current_state, pending_state;
    GstStateChangeReturn ret = gst_element_get_state(pipeline_, &current_state, &pending_state, GST_CLOCK_TIME_NONE);
    return current_state;
}

void GstPipelineWrapper::RunningMainLoop()
{
    GMainLoop* loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);
}

bool GstPipelineWrapper::GstMessageParser(GstBus* bus, GstMessage* msg, GstPipelineWrapper* pipeline)
{
    if (!pipeline->get_is_verbose())
        return true;

    if (msg != NULL) {
        GError* err = 0;
        gchar* debug_info = 0;

        switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug_info);
            g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
            g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            break;

        case GST_MESSAGE_WARNING:
            gst_message_parse_warning(msg, &err, &debug_info);
            g_printerr("Warning received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
            g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            break;

        case GST_MESSAGE_INFO:
            gst_message_parse_info(msg, &err, &debug_info);
            g_printerr("Info received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
            g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
            g_clear_error(&err);
            g_free(debug_info);
            break;

        case GST_MESSAGE_EOS:
            g_print("End-Of-Stream reached.\n");
            break;

        case GST_MESSAGE_STATE_CHANGED:
            GstState old_state, new_state;
            gst_message_parse_state_changed(msg, &old_state, &new_state, 0);
            g_print("Element %s changed state from %s to %s.\n", GST_OBJECT_NAME(msg->src), gst_element_state_get_name(old_state), gst_element_state_get_name(new_state));
            break;

        case GST_MESSAGE_QOS:
            break;

        case GST_MESSAGE_STREAM_STATUS:

            GstStreamStatusType stream_status_type;
            GstElement* owner;
            const gchar* stream_status_type_string;
            gst_message_parse_stream_status(msg, &stream_status_type, &owner);

            switch (stream_status_type) {
            case GST_STREAM_STATUS_TYPE_CREATE:
                stream_status_type_string = "CREATE";
                break;
            case GST_STREAM_STATUS_TYPE_ENTER:
                stream_status_type_string = "ENTER";
                break;
            case GST_STREAM_STATUS_TYPE_LEAVE:
                stream_status_type_string = "LEAVE";
                break;
            case GST_STREAM_STATUS_TYPE_DESTROY:
                stream_status_type_string = "DESTROY";
                break;

            case GST_STREAM_STATUS_TYPE_START:
                stream_status_type_string = "START";
                break;
            case GST_STREAM_STATUS_TYPE_PAUSE:
                stream_status_type_string = "PAUSE";
                break;
            case GST_STREAM_STATUS_TYPE_STOP:
                stream_status_type_string = "STOP";
                break;
            }

            g_printerr("STREAM STATUS received from element %s: %s\n", GST_OBJECT_NAME(owner), stream_status_type_string);
            //g_free (stream_status_type_string);
            break;

        default:
            g_printerr("Unparsed message received of type: %s\n", gst_message_type_get_name(GST_MESSAGE_TYPE(msg)));
            break;
        }
    }
    return true;
}

void GstPipelineWrapper::EnsureGstreamerInitialization()
{
    if (!gst_is_initialized()) {
        //		int 	argc = 0;
        //		char** 	argv = NULL;

        int argc = 0;
        char** argv = new char*[2];

        argv[argc++] = "--gst-version";
        argv[argc++] = "--gst-debug-level=1";

        gst_init(&argc, &argv);

        delete argv;

        // get and display GST version
        {
            guint major = 0;
            guint minor = 0;
            guint micro = 0;
            guint nano = 0;

            gst_version(&major, &minor, &micro, &nano);

            std::cout << "GStreamer V" << major << "." << minor << "." << micro << "." << nano << std::endl;
        }
    }
}

void GstPipelineWrapper::FreePipeline()
{
    gst_element_set_state(pipeline_, GST_STATE_NULL);
    gst_object_unref(bus_);
    gst_object_unref(pipeline_);
}

void GstPipelineWrapper::set_is_verbose(bool is_verbose) { is_verbose = is_verbose; };
bool GstPipelineWrapper::get_is_verbose() const { return is_verbose_; }
