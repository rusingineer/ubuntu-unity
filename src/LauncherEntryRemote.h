// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright (C) 2011 Canonical Ltd
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
 * Authored by: Mikkel Kamstrup Erlandsen <mikkel.kamstrup@canonical.com>
 */

#ifndef LAUNCHER_ENTRY_REMOTE_H
#define LAUNCHER_ENTRY_REMOTE_H

#include <Nux/Nux.h>
#include <glib.h>
#include <sigc++/sigc++.h>

/**
 * Instances of this class mirrors the remote metadata for a laucnher entry
 * exposed by an application via the com.canonical.Unity.LauncherEntry DBus API.
 *
 * You do not create instances of LauncherEntryRemote yourself. Instead they
 * are created and managed dynamically by a LauncherEntryRemoteModel.
 */
class LauncherEntryRemote : public nux::InitiallyUnownedObject, public sigc::trackable
{

  NUX_DECLARE_OBJECT_TYPE (LauncherEntryRemote, nux::InitiallyUnownedObject);

public:
    const gchar* Emblem();
    gint64 Count();
    gdouble Progress();

    gboolean EmblemVisible();
    gboolean CountVisible();
    gboolean ProgressVisible();

    sigc::signal<void> emblem_changed;
    sigc::signal<void> count_changed;
    sigc::signal<void> progress_changed;

    sigc::signal<void> emblem_visible_changed;
    sigc::signal<void> count_visible_changed;
    sigc::signal<void> progress_visible_changed;

private:

    LauncherEntryRemote();
    ~LauncherEntryRemote();

    gchar  *_emblem;
    gint64  _count;
    gdouble _progress;

    gboolean _emblem_visible;
    gboolean _count_visible;
    gboolean _progress_visible;

    void SetEmblem (const gchar *emblem);
    void SetCount (gint64 count);
    void SetProgress (gdouble progress);

    void SetEmblemVisible (gboolean visible);
    void SetCountVisible (gboolean visible);
    void SetProgressVisible (gboolean visible);

    friend class LauncherEntryRemoteModel;
};

#endif // LAUNCHER_ENTRY_REMOTE_H
