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
 * Authored by: Marco Trevisan (Treviño) <mail@3v1n0.net>
 *              Neil Jagdish Patel <neil.patel@canonical.com>
 */

#include "Nux/Nux.h"
#include "Nux/HLayout.h"
#include "NuxCore/Logger.h"

#include "PanelIndicatorsView.h"

#include <UnityCore/Variant.h>
#include <glib.h>

namespace
{
nux::logging::Logger logger("unity.indicators");
}

namespace unity
{

PanelIndicatorsView::PanelIndicatorsView()
: View(NUX_TRACKER_LOCATION)
, layout_(NULL)
{
  LOG_DEBUG(logger) << "Indicators View Added: ";
  layout_ = new nux::HLayout("", NUX_TRACKER_LOCATION);

  SetCompositionLayout(layout_);
}

PanelIndicatorsView::~PanelIndicatorsView()
{
  for (auto conn : indicators_connections_)
    conn.disconnect();
}

void
PanelIndicatorsView::AddIndicator(indicator::Indicator::Ptr const& indicator)
{
  LOG_DEBUG(logger) << "IndicatorAdded: " << indicator->name();
  indicators_.push_back(indicator);

  auto entry_added_conn = indicator->on_entry_added.connect(sigc::mem_fun(this, &PanelIndicatorsView::OnEntryAdded));
  indicators_connections_.push_back(entry_added_conn);

  auto entry_removed_conn = indicator->on_entry_removed.connect(sigc::mem_fun(this, &PanelIndicatorsView::OnEntryRemoved));
  indicators_connections_.push_back(entry_removed_conn);
}

long
PanelIndicatorsView::ProcessEvent(nux::IEvent& ievent, long TraverseInfo, long ProcessEventInfo)
{
  long ret = TraverseInfo;

  if (layout_)
    ret = layout_->ProcessEvent(ievent, ret, ProcessEventInfo);
  return ret;
}

void
PanelIndicatorsView::Draw(nux::GraphicsEngine& GfxContext, bool force_draw)
{
}

void
PanelIndicatorsView::QueueDraw()
{
  nux::View::QueueDraw();
  for (auto i = entries_.begin(), end = entries_.end(); i != end; ++i)
  {
    i->second->QueueDraw();
  }
}

bool
PanelIndicatorsView::ActivateEntry(std::string const& entry_id)
{
  auto entry = entries_.find(entry_id);

  if (entry != entries_.end() && entry->second->IsEntryValid())
  {
    LOG_DEBUG(logger) << "Activating: " << entry_id;
    entry->second->Activate();
    return true;
  }

  return false;
}

bool
PanelIndicatorsView::ActivateIfSensitive()
{
  for (auto i = entries_.begin(), end = entries_.end(); i != end; ++i)
  {
    PanelIndicatorEntryView* view = i->second;
    if (view->IsSensitive())
    {
      view->Activate();
      return true;
    }
  }
  return false;
}

void
PanelIndicatorsView::GetGeometryForSync(indicator::EntryLocationMap& locations)
{
  for (auto i = entries_.begin(), end = entries_.end(); i != end; ++i)
  {
    i->second->GetGeometryForSync(locations);
  }
}

bool
PanelIndicatorsView::OnPointerMoved(int x, int y)
{
  for (auto i = entries_.begin(), end = entries_.end(); i != end; ++i)
  {
    PanelIndicatorEntryView* view = i->second;

    nux::Geometry geo = view->GetAbsoluteGeometry();
    if (geo.IsPointInside(x, y))
    {
      view->OnMouseDown(x, y, 0, 0);
      return true;
    }
  }

  return false;
}

void
PanelIndicatorsView::DrawContent(nux::GraphicsEngine& GfxContext, bool force_draw)
{
  GfxContext.PushClippingRectangle(GetGeometry());
  if (layout_)
    layout_->ProcessDraw(GfxContext, force_draw);
  GfxContext.PopClippingRectangle();
}

void
PanelIndicatorsView::OnEntryAdded(indicator::Entry::Ptr const& entry)
{
  auto view = new PanelIndicatorEntryView(entry);
  view->refreshed.connect(sigc::mem_fun(this, &PanelIndicatorsView::OnEntryRefreshed));

  int indicator_pos = nux::NUX_LAYOUT_BEGIN;

  if (entry->priority() > -1)
  {
    for (nux::Area* &area : layout_->GetChildren())
    {
      auto en = dynamic_cast<PanelIndicatorEntryView*>(area);

      if (en)
      {
        if (en && entry->priority() <= en->GetEntryPriority())
          break;

        indicator_pos++;
      }
    }
  }

  nux::LayoutPosition pos = (nux::LayoutPosition) indicator_pos;
  layout_->AddView(view, 0, nux::eCenter, nux::eFull, 1.0, pos);
  layout_->SetContentDistribution(nux::eStackRight);
  entries_[entry->id()] = view;

  AddChild(view);
  QueueRelayout();
  QueueDraw();

  on_indicator_updated.emit(view);
}

void
PanelIndicatorsView::OnEntryRefreshed(PanelIndicatorEntryView* view)
{
  QueueRelayout();
  QueueDraw();

  on_indicator_updated.emit(view);
}

void
PanelIndicatorsView::OnEntryRemoved(std::string const& entry_id)
{
  PanelIndicatorEntryView* view = entries_[entry_id];
  on_indicator_updated.emit(view);
  layout_->RemoveChildObject(view);
  entries_.erase(entry_id);

  QueueRelayout();
  QueueDraw();
}

const gchar* PanelIndicatorsView::GetName()
{
  return "IndicatorsView";
}

const gchar*
PanelIndicatorsView::GetChildsName()
{
  return "entries";
}

void
PanelIndicatorsView::AddProperties(GVariantBuilder* builder)
{
  variant::BuilderWrapper(builder).add(GetGeometry());
}

} // namespace unity
