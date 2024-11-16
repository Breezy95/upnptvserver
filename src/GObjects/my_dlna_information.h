#ifndef MY_DLNA_INFORMATION_H
#define MY_DLNA_INFORMATION_H

#include <glib-object.h>
#include <gupnp-dlna-2.0/libgupnp-dlna/gupnp-dlna.h>

G_BEGIN_DECLS

#define MY_TYPE_DLNA_INFORMATION (my_dlna_information_get_type())
#define MY_DLNA_INFORMATION(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), MY_TYPE_DLNA_INFORMATION, MyDLNAInformation))
#define MY_DLNA_INFORMATION_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), MY_TYPE_DLNA_INFORMATION, MyDLNAInformationClass))
#define MY_IS_DLNA_INFORMATION(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), MY_TYPE_DLNA_INFORMATION))
#define MY_IS_DLNA_INFORMATION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), MY_TYPE_DLNA_INFORMATION))

typedef struct _MyDLNAInformation MyDLNAInformation;
typedef struct _MyDLNAInformationClass MyDLNAInformationClass;

struct _MyDLNAInformation {
    GUPnPDLNAInformation parent_instance;
    // Add any additional fields here
};

struct _MyDLNAInformationClass {
    GUPnPDLNAInformationClass parent_class;

    // Implement the virtual methods
    gchar* (*get_value)(MyDLNAInformation *self, const gchar *key);
    void (*set_value)(MyDLNAInformation *self, const gchar *key, const gchar *value);
    gchar* (*get_title)(MyDLNAInformation *self);
    gchar* (*get_creator)(MyDLNAInformation *self);
    gchar* (*get_class)(MyDLNAInformation *self);
    gchar* (*get_protocol_info)(MyDLNAInformation *self);
    gchar* (*get_metadata)(MyDLNAInformation *self);
};

GType my_dlna_information_get_type(void);
MyDLNAInformation* my_dlna_information_new(void);

G_END_DECLS

#endif // MY_DLNA_INFORMATION_H
