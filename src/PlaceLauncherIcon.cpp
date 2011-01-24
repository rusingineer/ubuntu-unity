// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright (C) 2010 Canonical Ltd
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
 */

#include "PlaceLauncherIcon.h"

#include "ubus-server.h"
#include "UBusMessages.h"

PlaceLauncherIcon::PlaceLauncherIcon (Launcher *launcher, PlaceEntry *entry)
: SimpleLauncherIcon(launcher),
  _entry (entry)
{
  gchar *escape;

  escape = g_markup_escape_text (entry->GetName (), -1);

  SetTooltipText (escape);
  SetIconName (entry->GetIcon ());
  SetQuirk (QUIRK_VISIBLE, true);
  SetQuirk (QUIRK_RUNNING, false);
  SetIconType (TYPE_PLACE); 

  g_free (escape);
}

PlaceLauncherIcon::~PlaceLauncherIcon()
{

}

nux::Color 
PlaceLauncherIcon::BackgroundColor ()
{
  return nux::Color (0xFF333333);
}

nux::Color 
PlaceLauncherIcon::GlowColor ()
{
  return nux::Color (0xFF333333);
}

void
PlaceLauncherIcon::OnMouseClick (int button)
{
  SimpleLauncherIcon::OnMouseClick (button);

  if (button == 1)
  {
    ubus_server_send_message (ubus_server_get_default (),
                              UBUS_PLACE_ENTRY_ACTIVATE_REQUEST,
                              g_variant_new ("(sus)",
                                             _entry->GetId (),
                                             0,
                                             ""));
  }
}

void
PlaceLauncherIcon::UpdatePlaceIcon ()
{

}
