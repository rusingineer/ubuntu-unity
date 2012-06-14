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
* Authored by: Jason Smith <jason.smith@canonical.com>
*/

#include <Nux/Nux.h>
#include <Nux/BaseWindow.h>
#include <NuxGraphics/GraphicsEngine.h>
#include <Nux/TextureArea.h>

#include "LauncherDragWindow.h"

namespace unity
{
namespace launcher
{

NUX_IMPLEMENT_OBJECT_TYPE(LauncherDragWindow);

LauncherDragWindow::LauncherDragWindow(nux::ObjectPtr<nux::IOpenGLBaseTexture> icon)
  : nux::BaseWindow("")
  , _icon(icon)
{
  SetBaseSize(_icon->GetWidth(), _icon->GetHeight());
}

LauncherDragWindow::~LauncherDragWindow()
{
  if (on_anim_completed.connected())
    on_anim_completed.disconnect();
}

bool LauncherDragWindow::Animating()
{
  return bool(animation_timer_);
}

void LauncherDragWindow::SetAnimationTarget(int x, int y)
{
  _animation_target = nux::Point2(x, y);
}

void LauncherDragWindow::StartAnimation()
{
  if (animation_timer_)
    return;

  animation_timer_.reset(new glib::Timeout(15));
  animation_timer_->Run(sigc::mem_fun(this, &LauncherDragWindow::OnAnimationTimeout));
}

bool LauncherDragWindow::OnAnimationTimeout()
{
  nux::Geometry const& geo = GetGeometry();

  int half_size = geo.width / 2;

  int target_x = static_cast<int>(_animation_target.x) - half_size;
  int target_y = static_cast<int>(_animation_target.y) - half_size;

  int x_delta = static_cast<int>(static_cast<float>(target_x - geo.x) * .3f);
  if (std::abs(x_delta) < 5)
    x_delta = (x_delta >= 0) ? std::min(5, target_x - geo.x) : std::max(-5, target_x - geo.x);

  int y_delta = static_cast<int>(static_cast<float>(target_y - geo.y) * .3f);
  if (std::abs(y_delta) < 5)
    y_delta = (y_delta >= 0) ? std::min(5, target_y - geo.y) : std::max(-5, target_y - geo.y);

  SetBaseXY(geo.x + x_delta, geo.y + y_delta);

  nux::Geometry const& new_geo = GetGeometry();

  if (new_geo.x == target_x && new_geo.y == target_y)
  {
    anim_completed.emit();
    animation_timer_.reset();

    return false;
  }

  return true;
}

void
LauncherDragWindow::DrawContent(nux::GraphicsEngine& GfxContext, bool force_draw)
{
  nux::Geometry geo = GetGeometry();
  geo.SetX(0);
  geo.SetY(0);

  GfxContext.PushClippingRectangle(geo);

  nux::TexCoordXForm texxform;
  texxform.FlipVCoord(true);

  GfxContext.QRP_1Tex(0,
                      0,
                      _icon->GetWidth(),
                      _icon->GetHeight(),
                      _icon,
                      texxform,
                      nux::color::White);

  GfxContext.PopClippingRectangle();
}

} // namespace launcher
} // namespace unity