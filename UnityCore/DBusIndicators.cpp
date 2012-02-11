// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright (C) 2010, 2011 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Neil Jagdish Patel <neil.patel@canonical.com>
 *              Marco Trevisan (Treviño) <3v1n0@ubuntu.com>
 */

#include "DBusIndicators.h"

#include <algorithm>
#include <iostream>

#include <gio/gio.h>

#include <X11/Xlib.h>

#include "config.h"
#include "GLibSignal.h"
#include "GLibWrapper.h"
#include "Variant.h"

namespace unity
{
namespace indicator
{

namespace
{
// This anonymous namespace holds the DBus callback methods.

struct SyncData
{
  SyncData(DBusIndicators::Impl* self_)
    : self(self_)
    , cancel(g_cancellable_new())
  {
  }

  ~SyncData()
  {
    if (cancel)
    {
      g_cancellable_cancel(cancel);
      g_object_unref(cancel);
    }
  }

  void SyncComplete()
  {
    if (cancel)
    {
      g_object_unref(cancel);
    }
    cancel = NULL;
  }

  DBusIndicators::Impl* self;
  GCancellable* cancel;
};

typedef boost::shared_ptr<SyncData> SyncDataPtr;

const char* const S_NAME = "com.canonical.Unity.Panel.Service";
const char* const S_PATH = "/com/canonical/Unity/Panel/Service";
const char* const S_IFACE = "com.canonical.Unity.Panel.Service";

struct ShowEntryData
{
  GDBusProxy* proxy;
  std::string entry_id;
  guint xid;
  int x;
  int y;
  guint button;
  guint timestamp;
};

bool run_local_panel_service();
gboolean reconnect_to_service(gpointer data);
void on_proxy_ready_cb(GObject* source, GAsyncResult* res, gpointer data);
void request_sync(GDBusProxy* proxy, const char* method, GVariant* name,
                  SyncData* data);
void on_sync_ready_cb(GObject* source, GAsyncResult* res, gpointer data);

bool send_show_entry(ShowEntryData* data);
bool send_show_appmenu(ShowEntryData* data);

} // anonymous namespace


// Connects to the remote panel service (unity-panel-service) and translates
// that into something that the panel can show
class DBusIndicators::Impl
{
public:
  Impl(DBusIndicators* owner);
  ~Impl();

  void OnRemoteProxyReady(GDBusProxy* proxy);
  void Reconnect();
  void RequestSyncAll();
  void RequestSyncIndicator(std::string const& name);
  void Sync(GVariant* args, SyncData* data);
  void SyncGeometries(std::string const& name,
                      EntryLocationMap const& locations);
  void OnProxyNameOwnerChanged(GDBusProxy* proxy, GParamSpec* pspec);
  void OnProxySignalReceived(GDBusProxy* proxy,
                             char* sender_name,
                             char* signal_name_,
                             GVariant* parameters);

  virtual void OnEntryScroll(std::string const& entry_id, int delta);
  virtual void OnEntryShowMenu(std::string const& entry_id, unsigned int xid,
                               int x, int y, unsigned int button,
                               unsigned int timestamp);
  virtual void OnEntrySecondaryActivate(std::string const& entry_id,
                                        unsigned int timestamp);
  virtual void OnShowAppMenu(unsigned int xid, int x, int y,
                             unsigned int timestamp);

  std::string name() const;
  std::string owner_name() const;
  bool using_local_service() const;

  DBusIndicators* owner_;
  GDBusProxy* proxy_;
  typedef std::vector<SyncDataPtr> PendingSyncs;
  PendingSyncs pending_syncs_;

  glib::SignalManager signal_manager_;
  std::map<std::string, EntryLocationMap> cached_locations_;
};


// Public Methods
DBusIndicators::Impl::Impl(DBusIndicators* owner)
  : owner_(owner)
  , proxy_(NULL)
{
  Reconnect();
}

DBusIndicators::Impl::~Impl()
{
  if (G_IS_OBJECT(proxy_))
  {
    g_object_unref(proxy_);
  }
}

void DBusIndicators::Impl::Reconnect()
{
  g_spawn_command_line_sync("killall -9 unity-panel-service",
                            NULL, NULL, NULL, NULL);

  if (g_getenv("PANEL_USE_LOCAL_SERVICE"))
  {
    run_local_panel_service();
    g_timeout_add_seconds(1, reconnect_to_service, this);
  }
  else
  {
    // We want to grab the Panel Service object. This is async, which is fine
    reconnect_to_service(this);
  }
}

void DBusIndicators::Impl::OnRemoteProxyReady(GDBusProxy* proxy)
{
  if (proxy_)
  {
    // We've been connected before; We don't need new proxy, just continue
    // rocking with the old one.
    g_object_unref(proxy);
  }
  else
  {
    proxy_ = proxy;
    // Connect to interesting signals
    signal_manager_.Add(new glib::Signal<void, GDBusProxy*, char*, char*, GVariant*>
                        (proxy_, "g-signal", sigc::mem_fun(this, &Impl::OnProxySignalReceived)));

    signal_manager_.Add(new glib::Signal<void, GDBusProxy*, GParamSpec*>
                        (proxy_, "notify::g-name-owner", sigc::mem_fun(this, &Impl::OnProxyNameOwnerChanged)));
  }
  RequestSyncAll();
}

void DBusIndicators::Impl::RequestSyncAll()
{
  SyncDataPtr data(new SyncData(this));
  pending_syncs_.push_back(data);
  request_sync(proxy_, "Sync", NULL, data.get());
}

void DBusIndicators::Impl::RequestSyncIndicator(std::string const& name)
{
  SyncDataPtr data(new SyncData(this));
  pending_syncs_.push_back(data);
  // The ownership of this variant is taken by the g_dbus_proxy_call.
  GVariant* v_name = g_variant_new("(s)", name.c_str());
  request_sync(proxy_, "SyncOne", v_name, data.get());
}


void DBusIndicators::Impl::OnEntryShowMenu(std::string const& entry_id,
                                           unsigned int xid, int x, int y,
                                           unsigned int button,
                                           unsigned int timestamp)
{
  owner_->on_entry_show_menu.emit(entry_id, xid, x, y, button, timestamp);

  // We have to do this because on certain systems X won't have time to
  // respond to our request for XUngrabPointer and this will cause the
  // menu not to show
  auto data = new ShowEntryData();
  data->proxy = proxy_;
  data->entry_id = entry_id;
  data->xid = xid;
  data->x = x;
  data->y = y;
  data->button = button;
  data->timestamp = timestamp;

  g_idle_add_full (G_PRIORITY_DEFAULT, (GSourceFunc) send_show_entry, data, NULL);
}

void DBusIndicators::Impl::OnShowAppMenu(unsigned int xid, int x, int y,
                                         unsigned int timestamp)
{
  owner_->on_show_appmenu.emit(xid, x, y, timestamp);

  // We have to do this because on certain systems X won't have time to
  // respond to our request for XUngrabPointer and this will cause the
  // menu not to show
  auto data = new ShowEntryData();
  data->proxy = proxy_;
  data->xid = xid;
  data->x = x;
  data->y = y;
  data->timestamp = timestamp;

  g_idle_add_full (G_PRIORITY_DEFAULT, (GSourceFunc) send_show_appmenu, data, NULL);
}

void DBusIndicators::Impl::OnEntrySecondaryActivate(std::string const& entry_id,
                                                    unsigned int timestamp)
{
  g_dbus_proxy_call(proxy_, "SecondaryActivateEntry",
                    g_variant_new("(su)", entry_id.c_str(), timestamp),
                    G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL, NULL);
}

void DBusIndicators::Impl::OnEntryScroll(std::string const& entry_id, int delta)
{
  g_dbus_proxy_call(proxy_, "ScrollEntry",
                    g_variant_new("(si)", entry_id.c_str(), delta),
                    G_DBUS_CALL_FLAGS_NONE,
                    -1, NULL, NULL, NULL);
}

void DBusIndicators::Impl::Sync(GVariant* args, SyncData* data)
{
  GVariantIter* iter            = NULL;
  gchar*        name_hint       = NULL;
  gchar*        indicator_id    = NULL;
  gchar*        entry_id        = NULL;
  gchar*        label           = NULL;
  gboolean      label_sensitive = false;
  gboolean      label_visible   = false;
  guint32       image_type      = 0;
  gchar*        image_data      = NULL;
  gboolean      image_sensitive = false;
  gboolean      image_visible   = false;
  gint32        priority        = -1;

  // sanity check
  if (!args)
    return;

  std::map<Indicator::Ptr, Indicator::Entries> indicators;

  g_variant_get(args, "(a(ssssbbusbbi))", &iter);
  while (g_variant_iter_loop(iter, "(ssssbbusbbi)",
                             &indicator_id,
                             &entry_id,
                             &name_hint,
                             &label,
                             &label_sensitive,
                             &label_visible,
                             &image_type,
                             &image_data,
                             &image_sensitive,
                             &image_visible,
                             &priority))
  {
    std::string entry(entry_id);
    std::string indicator_name(indicator_id);

    Indicator::Ptr indicator = owner_->GetIndicator(indicator_name);
    if (!indicator)
    {
      indicator = owner_->AddIndicator(indicator_name);
    }

    Indicator::Entries& entries = indicators[indicator];

    // NULL entries (entry_id == "") are empty indicators.
    if (entry != "")
    {
      Entry::Ptr e = indicator->GetEntry(entry_id);

      if (!e)
      {
        e = Entry::Ptr(new Entry(entry,
                                 name_hint,
                                 label,
                                 label_sensitive,
                                 label_visible,
                                 image_type,
                                 image_data,
                                 image_sensitive,
                                 image_visible,
                                 priority));
      }
      else
      {
        e->setLabel(label, label_sensitive, label_visible);
        e->setImage(image_type, image_data, image_sensitive, image_visible);
        e->setPriority(priority);
      }

      entries.push_back(e);
    }
  }
  g_variant_iter_free(iter);

  for (auto i = indicators.begin(), end = indicators.end(); i != end; ++i)
  {
    i->first->Sync(indicators[i->first]);
  }

  // Clean up the SyncData.  NOTE: don't use find when passing in a raw
  // pointer due to explicit construction of the shared pointer.  Could write
  // a predicate, but often a for loop is easier to understand.
  data->SyncComplete();
  for (PendingSyncs::iterator i = pending_syncs_.begin(), end = pending_syncs_.end();
       i != end; ++i)
  {
    if (i->get() == data)
    {
      pending_syncs_.erase(i);
      break;
    }
  }

  // Notify listeners we have new data
  owner_->on_synced.emit();
}

void DBusIndicators::Impl::SyncGeometries(std::string const& name,
                                          EntryLocationMap const& locations)
{
  if (!proxy_)
    return;

  GVariantBuilder b;
  bool found_changed_locations = false;
  g_variant_builder_init(&b, G_VARIANT_TYPE("(a(ssiiii))"));
  g_variant_builder_open(&b, G_VARIANT_TYPE("a(ssiiii)"));
  EntryLocationMap& cached_loc = cached_locations_[name];

  // Only send to panel service the geometries of items that have changed
  for (auto i = locations.begin(), end = locations.end(); i != end; ++i)
  {
    auto rect = i->second;

    if (cached_loc[i->first] != rect)
    {
      g_variant_builder_add(&b, "(ssiiii)",
                            name.c_str(),
                            i->first.c_str(),
                            rect.x,
                            rect.y,
                            rect.width,
                            rect.height);
      found_changed_locations = true;
    }
  }

  // Inform panel service of the entries that have been removed sending invalid values
  for (auto i = cached_loc.begin(), end = cached_loc.end(); i != end; ++i)
  {
    if (locations.find(i->first) == locations.end())
    {
      g_variant_builder_add(&b, "(ssiiii)",
                            name.c_str(),
                            i->first.c_str(),
                            0,
                            0,
                            -1,
                            -1);
      found_changed_locations = true;
    }
  }

  if (!found_changed_locations)
    return;

  g_variant_builder_close(&b);
  g_dbus_proxy_call(proxy_, "SyncGeometries",
                    g_variant_builder_end(&b),
                    G_DBUS_CALL_FLAGS_NONE,
                    -1,
                    NULL,
                    NULL,
                    NULL);

  cached_loc = locations;
}

std::string DBusIndicators::Impl::name() const
{
  glib::String name;
  g_object_get(proxy_, "g-name", &name, NULL);
  return name.Str();
}

std::string DBusIndicators::Impl::owner_name() const
{
  glib::String owner_name;
  g_object_get(proxy_, "g-name-owner", &owner_name, NULL);
  return owner_name.Str();
}

bool DBusIndicators::Impl::using_local_service() const
{
  return g_getenv("PANEL_USE_LOCAL_SERVICE") != NULL;
}

void DBusIndicators::Impl::OnProxyNameOwnerChanged(GDBusProxy* proxy,
                                                   GParamSpec* pspec)
{
  char* name_owner = g_dbus_proxy_get_name_owner(proxy);

  if (name_owner == NULL)
  {
    for (auto indicator : owner_->GetIndicators())
    {
      owner_->RemoveIndicator(indicator->name());
    }

    // The panel service has stopped for some reason.  Restart it if not in
    // dev mode
    if (!g_getenv("UNITY_DEV_MODE"))
      Reconnect();
  }

  g_free(name_owner);
}

void DBusIndicators::Impl::OnProxySignalReceived(GDBusProxy* proxy,
                                                 char* sender_name,
                                                 char* signal_name_,
                                                 GVariant* parameters)
{
  std::string signal_name(signal_name_);
  if (signal_name == "EntryActivated")
  {
    glib::String entry_name;
    nux::Rect geo;
    g_variant_get (parameters, "(s(iiuu))", &entry_name, &geo.x, &geo.y, &geo.width, &geo.height);

    if (entry_name)
    {
      owner_->ActivateEntry(entry_name.Str(), geo);
    }
  }
  else if (signal_name == "EntryActivateRequest")
  {
    const char* entry_name = g_variant_get_string(g_variant_get_child_value(parameters, 0), NULL);
    if (entry_name)
    {
      owner_->on_entry_activate_request.emit(entry_name);
    }
  }
  else if (signal_name == "ReSync")
  {
    const char* id = g_variant_get_string(g_variant_get_child_value(parameters, 0), NULL);
    bool sync_one = !g_strcmp0(id, "") == 0;

    if (sync_one)
    {
      RequestSyncIndicator(id);
    }
    else
    {
      RequestSyncAll();
    }
  }
  else if (signal_name == "EntryShowNowChanged")
  {
    gchar*    id = NULL;
    gboolean  show_now;

    g_variant_get(parameters, "(sb)", &id, &show_now);
    owner_->SetEntryShowNow(id, show_now);

    g_free(id);
  }
}

DBusIndicators::DBusIndicators()
  : pimpl(new Impl(this))
{
}

DBusIndicators::~DBusIndicators()
{
  delete pimpl;
}

void DBusIndicators::SyncGeometries(std::string const& name,
                                    EntryLocationMap const& locations)
{
  pimpl->SyncGeometries(name, locations);
}

void DBusIndicators::OnEntryScroll(std::string const& entry_id, int delta)
{
  pimpl->OnEntryScroll(entry_id, delta);
}

void DBusIndicators::OnEntryShowMenu(std::string const& entry_id,
                                     unsigned int xid, int x, int y,
                                     unsigned int button, unsigned int timestamp)
{
  pimpl->OnEntryShowMenu(entry_id, xid, x, y, button, timestamp);
}

void DBusIndicators::OnEntrySecondaryActivate(std::string const& entry_id,
                                              unsigned int timestamp)
{
  pimpl->OnEntrySecondaryActivate(entry_id, timestamp);
}

void DBusIndicators::OnShowAppMenu(unsigned int xid, int x, int y,
                                   unsigned int timestamp)
{
  pimpl->OnShowAppMenu(xid, x, y, timestamp);
}

std::string DBusIndicators::name() const
{
  return pimpl->name();
}

std::string DBusIndicators::owner_name() const
{
  return pimpl->owner_name();
}

bool DBusIndicators::using_local_service() const
{
  return pimpl->using_local_service();
}


namespace
{

// Initialise DBus for the panel service, and let us know when it is
// ready.  The unused bool return is to fit with the GSourceFunc.
gboolean reconnect_to_service(gpointer data)
{
  g_dbus_proxy_new_for_bus(G_BUS_TYPE_SESSION,
                           G_DBUS_PROXY_FLAGS_DO_NOT_LOAD_PROPERTIES,
                           NULL,
                           S_NAME,
                           S_PATH,
                           S_IFACE,
                           NULL,
                           on_proxy_ready_cb,
                           data);
  return false;
}

// Make sure the proxy object exists and has a name, and if so, pass
// that on to the DBusIndicators.
void on_proxy_ready_cb(GObject* source, GAsyncResult* res, gpointer data)
{
  DBusIndicators::Impl* remote = reinterpret_cast<DBusIndicators::Impl*>(data);
  GError* error = NULL;
  GDBusProxy* proxy = g_dbus_proxy_new_for_bus_finish(res, &error);

  static bool force_tried = false;

  char* name_owner;
  if (G_IS_DBUS_PROXY(proxy) && (name_owner = g_dbus_proxy_get_name_owner(proxy)))
  {
    remote->OnRemoteProxyReady(G_DBUS_PROXY(proxy));
    g_free(name_owner);
    return;
  }
  else
  {
    if (force_tried)
    {
      g_warning("WARNING: Unable to connect to the unity-panel-service %s",
                error ? error->message : "Unknown");
      if (error)
        g_error_free(error);
    }
    else
    {
      force_tried = true;
      run_local_panel_service();
      g_timeout_add_seconds(2, reconnect_to_service, remote);
    }
  }

  g_object_unref(proxy);
}


bool run_local_panel_service()
{
  GError* error = NULL;

  // This is obviously hackish, but this part of the code is mostly hackish...
  // Let's attempt to run it from where we expect it to be
  std::string cmd = PREFIXDIR + std::string("/lib/unity/unity-panel-service");
  std::cerr << "\nWARNING: Couldn't load panel from installed services, "
            << "so trying to load panel from known location: "
            << cmd << "\n";

  g_spawn_command_line_async(cmd.c_str(), &error);
  if (error)
  {
    std::cerr << "\nWARNING: Unable to launch remote service manually: "
              << error->message << "\n";
    g_error_free(error);
    return false;
  }
  return true;
}

void request_sync(GDBusProxy* proxy, const char* method, GVariant* name, SyncData* data)
{
  g_dbus_proxy_call(proxy, method, name, G_DBUS_CALL_FLAGS_NONE,
                    -1, data->cancel, on_sync_ready_cb, data);
}

void on_sync_ready_cb(GObject* source, GAsyncResult* res, gpointer data)
{
  SyncData* sync_data = reinterpret_cast<SyncData*>(data);
  GError* error = NULL;
  GVariant* args = g_dbus_proxy_call_finish((GDBusProxy*)source, res, &error);

  if (args == NULL)
  {
    g_warning("Unable to perform Sync() on panel service: %s", error->message);
    g_error_free(error);
    return;
  }

  sync_data->self->Sync(args, sync_data);
  g_variant_unref(args);
}

bool send_show_entry(ShowEntryData* data)
{
  g_return_val_if_fail(data != NULL, FALSE);
  g_return_val_if_fail(G_IS_DBUS_PROXY(data->proxy), FALSE);

  g_dbus_proxy_call(data->proxy,
                    "ShowEntry",
                    g_variant_new("(suiiuu)",
                                  data->entry_id.c_str(),
                                  data->xid,
                                  data->x,
                                  data->y,
                                  data->button,
                                  data->timestamp),
                    G_DBUS_CALL_FLAGS_NONE,
                    -1,
                    NULL,
                    NULL,
                    NULL);
  delete data;
  return FALSE;
}

bool send_show_appmenu(ShowEntryData* data)
{
  g_return_val_if_fail(data != NULL, FALSE);
  g_return_val_if_fail(G_IS_DBUS_PROXY(data->proxy), FALSE);

  g_dbus_proxy_call(data->proxy,
                    "ShowAppMenu",
                    g_variant_new("(uiiu)",
                                  data->xid,
                                  data->x,
                                  data->y,
                                  data->timestamp),
                    G_DBUS_CALL_FLAGS_NONE,
                    -1,
                    NULL,
                    NULL,
                    NULL);
  delete data;
  return FALSE;
}

} // anonymous namespace

} // namespace indicator
} // namespace unity
