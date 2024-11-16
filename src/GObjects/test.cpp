#include <iostream>
extern "C" {
#include <gupnp-dlna-2.0/libgupnp-dlna/gupnp-dlna.h>
#include "my_dlna_information.h"  // Ensure this header is in your include path
#include <gst/gst.h>
#include <gst/pbutils/pbutils.h>
}
int main(int argc, char *argv[]) {
 gst_init(&argc, &argv);

    GError *error = NULL;
    const gchar *uri = "file:///home/fabreezy/projects/upnptvserver/src/GObjects/test.mp4"; // Replace with your media file URI

    // Initialize the discoverer
    GstDiscoverer *discoverer = gst_discoverer_new(5 * GST_SECOND, &error);
    if (!discoverer) {
        g_printerr("Error creating discoverer instance: %s\n", error->message);
        g_clear_error(&error);
        return -1;
    }

    // Discover media info
    GstDiscovererInfo *info = gst_discoverer_discover_uri(discoverer, uri, &error);
    if (!info) {
        g_printerr("Error discovering URI: %s\n", error->message);
        g_clear_error(&error);
        g_object_unref(discoverer);
        return -1;
    }
    

    // Get and print media properties
    GstDiscovererResult result = gst_discoverer_info_get_result(info);
    if (result == GST_DISCOVERER_OK) {
        gint64 duration = gst_discoverer_info_get_duration(info);
        const gchar *mime_type = gst_discoverer_info_get_uri(info);
        g_print("Duration: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS(duration));
        g_print("MIME type: %s\n", mime_type);
    } else {
        g_printerr("Failed to get media information\n");
    }

    // Cleanup
    gst_discoverer_info_unref(info);
    g_object_unref(discoverer);
    gst_deinit();

    return 0;
}
