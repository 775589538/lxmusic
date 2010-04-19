/*
 *      lxmusic-notify.c
 *      
 *      Copyright 2009 Jürgen Hötzel <juergen@archlinux.org>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <libnotify/notify.h>
#include "lxmusic-notify.h"    

#define LXMUSIC_NOTIFY_NOTIFICATION_SHOW( notify )  do  { GError *error = NULL; 	\
    if (!notify_notification_show (notify, &error)) { 					\
    	g_warning ("Failed to show notification: %s", error->message); 			\
	g_error_free (error); 								\
    }											\
} while (0);							

struct _LXMusicNotification
{
    NotifyNotification *notify;
};

void lxmusic_do_notify_pixbuf( LXMusicNotification lxn, GdkPixbuf* pixbuf) 
{
    notify_notification_set_icon_from_pixbuf ( lxn->notify, pixbuf );
    LXMUSIC_NOTIFY_NOTIFICATION_SHOW( lxn->notify );
    g_free(lxn);
}

void lxmusic_do_notify( LXMusicNotification lxn ) 
{
    GValue val = { 0, };
    g_value_init (&val, G_TYPE_STRING);
    g_value_set_string( &val, "lxmusic" );
    g_object_set_property( G_OBJECT(lxn->notify), "icon-name", &val );
    g_value_unset (&val);
    LXMUSIC_NOTIFY_NOTIFICATION_SHOW( lxn->notify );
    g_free(lxn);
}



LXMusicNotification lxmusic_do_notify_prepare(const gchar *artist, const gchar *title, const char *summary, GtkStatusIcon *status_icon)
{

    if (!notify_is_initted ())
	notify_init ("LXMusic");
    GString* message = g_string_new("");
    if ( (artist != NULL) && (title != NULL ) ) {	
	/* metadata available */
	g_string_append_printf(message, "<b>%s: </b><i>%s</i>", _("Artist"), artist );
	g_string_append_printf(message, "\n<b>%s: </b><i>%s</i>", _("Title"), title );
    }
    /* use filename without markup */
    else 			
	g_string_append( message, title );
    struct _LXMusicNotification *lxn = g_new ( struct _LXMusicNotification, 1);
    lxn->notify = notify_notification_new (summary, message->str, NULL, NULL);
    notify_notification_set_urgency (lxn->notify, NOTIFY_URGENCY_NORMAL);
    notify_notification_attach_to_status_icon( lxn->notify, status_icon );
    notify_notification_set_timeout (lxn->notify, NOTIFY_EXPIRES_DEFAULT);
    g_string_free( message, TRUE );
    return lxn;
}

