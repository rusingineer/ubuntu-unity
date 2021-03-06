// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright (C) 2010-12 Canonical Ltd
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
 * Authored by: Andrea Azzarone <andrea.azzarone@canonical.com>
 */

#ifndef UNITYSHELL_DEVICES_SETTINGS_H
#define UNITYSHELL_DEVICES_SETTINGS_H

#include <boost/noncopyable.hpp>
#include <memory>
#include <string>

#include <sigc++/signal.h>
#include <sigc++/trackable.h>

namespace unity
{
namespace launcher
{

class DevicesSettings : boost::noncopyable, public sigc::trackable
{
public:
  typedef std::shared_ptr<DevicesSettings> Ptr;

  virtual ~DevicesSettings() {};

  virtual bool IsABlacklistedDevice(std::string const& uuid) const = 0;
  virtual void TryToBlacklist(std::string const& uuid) = 0;
  virtual void TryToUnblacklist(std::string const& uuid) = 0;

  sigc::signal<void> changed;
};

}
}

#endif
