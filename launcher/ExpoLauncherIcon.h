// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright (C) 2012 Canonical Ltd
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
 * Authored by: Marco Trevisan <marco.trevisan@canonical.com>
 */

#ifndef EXPO_LAUNCHER_ICON_H
#define EXPO_LAUNCHER_ICON_H

#include "SimpleLauncherIcon.h"

namespace unity
{
namespace launcher
{

class ExpoLauncherIcon : public SimpleLauncherIcon
{
public:
  ExpoLauncherIcon();
  void Stick(bool save);

protected:
  void ActivateLauncherIcon(ActionArg arg);
  std::string GetName() const;
  std::string GetRemoteUri();
};

}
}

#endif // EXPO_LAUNCHER_ICON_H
