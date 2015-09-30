// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright (C) 2013-2015 Canonical Ltd
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
 * Authored by: William Hua <william.hua@canonical.com>
 *              Marco Trevisan <marco.trevisan@canonical.com>
 */

#include "GnomeKeyGrabberImpl.h"

#include <NuxCore/Logger.h>

namespace unity
{
namespace key
{
DECLARE_LOGGER(logger, "unity.key.gnome.grabber");

// Private implementation
namespace shell
{
std::string const DBUS_NAME = "org.gnome.Shell";
std::string const DBUS_INTERFACE = "org.gnome.Shell";
std::string const DBUS_OBJECT_PATH = "/org/gnome/Shell";
std::string const INTROSPECTION_XML =
R"(<node>
  <interface name='org.gnome.Shell'>
    <method name='GrabAccelerators'>
      <arg type='a(su)' direction='in' name='accelerators'/>
      <arg type='au' direction='out' name='actions'/>
    </method>
    <method name='GrabAccelerator'>
      <arg type='s' direction='in' name='accelerator'/>
      <arg type='u' direction='in' name='flags'/>
      <arg type='u' direction='out' name='action'/>
    </method>
    <method name='UngrabAccelerator'>
      <arg type='u' direction='in' name='action'/>
      <arg type='b' direction='out' name='success'/>
    </method>
    <signal name='AcceleratorActivated'>
      <arg type='u' name='action'/>
      <arg type='u' name='device'/>
      <arg type='u' name='timestamp'/>
    </signal>
  </interface>
</node>)";
}

namespace testing
{
std::string const DBUS_NAME = "com.canonical.Unity.Test.GnomeKeyGrabber";
}

GnomeGrabber::Impl::Impl(bool test_mode)
  : screen_(screen)
  , shell_server_(test_mode ? testing::DBUS_NAME : shell::DBUS_NAME)
  , current_action_id_(0)
{
  shell_server_.AddObjects(shell::INTROSPECTION_XML, shell::DBUS_OBJECT_PATH);
  shell_object_ = shell_server_.GetObject(shell::DBUS_INTERFACE);
  shell_object_->SetMethodsCallsHandlerFull(sigc::mem_fun(this, &Impl::OnShellMethodCall));
}

GnomeGrabber::Impl::~Impl()
{
  for (auto& action : actions_)
    screen_->removeAction(&action);
}

uint32_t GnomeGrabber::Impl::NextActionID()
{
  return ++current_action_id_;
}

bool GnomeGrabber::Impl::AddAction(CompAction const& action, uint32_t& action_id)
{
  LOG_DEBUG(logger) << "AddAction (\"" << action.keyToString() << "\") = " << action_id;

  if (action.key().toString().empty())
  {
    LOG_WARN(logger) << "Trying to grab a disabled action, we skip it";
    return false;
  }

  auto it = std::find(actions_.begin(), actions_.end(), action);
  if (it != actions_.end())
  {
    action_id = actions_ids_[it - actions_.begin()];
    LOG_DEBUG(logger) << "Key binding \"" << action.keyToString() << "\" is already grabbed, reusing id " << action_id;
    return true;
  }

  if (screen_->addAction(const_cast<CompAction*>(&action)))
  {
    actions_ids_.push_back(action_id);
    actions_.push_back(action);
    return true;
  }

  LOG_ERROR(logger) << "Impossible to grab action \"" << action.keyToString() << "\"";
  return false;
}

uint32_t GnomeGrabber::Impl::AddAction(CompAction const& action)
{
  auto action_id = NextActionID();
  return AddAction(action, action_id) ? action_id : 0;
}

bool GnomeGrabber::Impl::RemoveAction(CompAction const& action)
{
  auto it = std::find(actions_.begin(), actions_.end(), action);

  if (it != actions_.end())
    return RemoveAction(static_cast<size_t>(it - actions_.begin()));

  return false;
}

bool GnomeGrabber::Impl::RemoveAction(uint32_t action_id)
{
  if (!action_id)
    return false;

  auto it = std::find(actions_ids_.begin(), actions_ids_.end(), action_id);

  if (it != actions_ids_.end())
    return RemoveAction(static_cast<size_t>(it - actions_ids_.begin()));

  return false;
}

bool GnomeGrabber::Impl::RemoveAction(size_t index)
{
  if (!index || index >= actions_.size())
    return false;

  CompAction* action = &(actions_[index]);
  LOG_DEBUG(logger) << "RemoveAction (\"" << action->keyToString() << "\")";

  screen_->removeAction(action);
  actions_.erase(actions_.begin() + index);
  actions_ids_.erase(actions_ids_.begin() + index);

  return true;
}

GVariant* GnomeGrabber::Impl::OnShellMethodCall(std::string const& method, GVariant* parameters, std::string const& sender, std::string const&)
{
  LOG_DEBUG(logger) << "Called method '" << method << "'";

  if (method == "GrabAccelerators")
  {
    if (g_variant_is_of_type(parameters, G_VARIANT_TYPE("(a(su))")))
    {
      GVariant* variant;
      GVariantBuilder builder;
      GVariantIter* iterator;
      gchar const* accelerator;
      guint flags;

      g_variant_builder_init(&builder, G_VARIANT_TYPE("au"));
      g_variant_get(parameters, "(a(su))", &iterator);

      while (g_variant_iter_next(iterator, "(&su)", &accelerator, &flags))
        g_variant_builder_add(&builder, "u", GrabDBusAccelerator(sender, accelerator, flags));

      g_variant_iter_free(iterator);
      variant = g_variant_builder_end(&builder);
      return g_variant_new_tuple(&variant, 1);
    }
    else
    {
      LOG_WARN(logger) << "Expected arguments of type (a(su))";
    }
  }
  else if (method == "GrabAccelerator")
  {
    if (g_variant_is_of_type(parameters, G_VARIANT_TYPE("(su)")))
    {
      gchar const* accelerator;
      guint flags;
      g_variant_get(parameters, "(&su)", &accelerator, &flags);

      if (uint32_t action_id = GrabDBusAccelerator(sender, accelerator, flags))
        return g_variant_new("(u)", action_id);
    }
    else
    {
      LOG_WARN(logger) << "Expected arguments of type (su)";
    }
  }
  else if (method == "UngrabAccelerator")
  {
    uint32_t action_id;
    g_variant_get(parameters, "(u)", &action_id);
    return g_variant_new("(b)", UnGrabDBusAccelerator(sender, action_id));
  }

  return nullptr;
}

uint32_t GnomeGrabber::Impl::GrabDBusAccelerator(std::string const& sender, std::string const& accelerator, uint32_t flags)
{
  CompAction action;
  action.keyFromString(accelerator);
  uint32_t action_id = NextActionID();

  LOG_DEBUG(logger) << "GrabDBusAccelerator \"" << accelerator << "\"";

  if (action.key().toString().empty())
  {
    auto prefixed = "XF86" + accelerator;
    LOG_DEBUG(logger) << "Can't grab \"" << accelerator << "\", trying \"" << prefixed << "\"";
    action.keyFromString(prefixed);
  }

  if (!IsActionPostponed(action))
  {
    action.setState(CompAction::StateInitKey);
    action.setInitiate([this, sender, action_id](CompAction* action, CompAction::State state, CompOption::Vector& options) {
      LOG_DEBUG(logger) << "pressed \"" << action->keyToString() << "\"";
      ActivateAction(*action, sender, action_id, 0, options[7].value().i());
      return true;
    });
  }
  else
  {
    action.setState(CompAction::StateInitKey | CompAction::StateTermKey);
    action.setTerminate([this, sender, action_id](CompAction* action, CompAction::State state, CompOption::Vector& options) {
      auto key = action->keyToString();

      LOG_DEBUG(logger) << "released \"" << key << "\"";

      if (state & CompAction::StateTermTapped)
      {
        LOG_DEBUG(logger) << "tapped \"" << key << "\"";
        ActivateAction(*action, sender, action_id, 0, options[7].value().i());
        return true;
      }

      return false;
    });
  }

  if (AddAction(action, action_id))
  {
    if (action_id == current_action_id_)
    {
      auto& owner_actions = actions_by_dest_[sender];
      bool empty_actions = owner_actions.actions.empty();
      owner_actions.actions.push_back(action_id);

      if (empty_actions)
      {
        owner_actions.watcher = std::make_shared<glib::DBusNameWatcher>(sender);
        owner_actions.watcher->vanished.connect([this] (std::string const& name) {
          auto it = actions_by_dest_.find(name);
          if (it != actions_by_dest_.end())
          {
            for (auto action_id : it->second.actions)
              RemoveAction(action_id);

            actions_by_dest_.erase(it);
          }
        });
      }
    }

    return action_id;
  }

  return 0;
}

bool GnomeGrabber::Impl::UnGrabDBusAccelerator(std::string const& sender, uint32_t action_id)
{
  auto it = actions_by_dest_.find(sender);
  if (it != actions_by_dest_.end())
  {
    auto& actions = it->second.actions;
    actions.erase(std::remove(actions.begin(), actions.end(), action_id), actions.end());

    if (actions.empty())
      actions_by_dest_.erase(it);
  }

  return RemoveAction(action_id);
}

void GnomeGrabber::Impl::ActivateAction(CompAction const& action, std::string const& dest, uint32_t action_id, uint32_t device, uint32_t timestamp) const
{
  LOG_DEBUG(logger) << "ActivateAction (" << action_id << " \"" << action.keyToString() << "\")";
  shell_object_->EmitSignal("AcceleratorActivated", g_variant_new("(uuu)", action_id, device, timestamp), dest);
}

bool GnomeGrabber::Impl::IsActionPostponed(CompAction const& action) const
{
  int keycode = action.key().keycode();
  return keycode == 0 || modHandler->keycodeToModifiers(keycode) != 0;
}

// Public implementation

GnomeGrabber::GnomeGrabber()
  : impl_(new Impl())
{}

GnomeGrabber::GnomeGrabber(TestMode const& dummy)
  : impl_(new Impl(true))
{}

GnomeGrabber::~GnomeGrabber()
{}

CompAction::Vector& GnomeGrabber::GetActions()
{
  return impl_->actions_;
}

uint32_t GnomeGrabber::AddAction(CompAction const& action)
{
  return impl_->AddAction(action);
}

bool GnomeGrabber::RemoveAction(CompAction const& action)
{
  return impl_->RemoveAction(action);
}

bool GnomeGrabber::RemoveAction(uint32_t action_id)
{
  return impl_->RemoveAction(action_id);
}

} // namespace key
} // namespace unity
