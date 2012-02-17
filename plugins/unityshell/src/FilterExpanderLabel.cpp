// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright 2011 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the applicable version of the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of both the GNU Lesser General Public
 * License version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Gordon Allott <gord.allott@canonical.com>
 *
 */

#include "DashStyle.h"
#include "FilterBasicButton.h"
#include "FilterExpanderLabel.h"

namespace
{

const float EXPAND_DEFAULT_ICON_OPACITY = 1.0f;

// right_hand_contents_
const int RIGHT_HAND_CONTENTS_HEIGHT = 33;

// layout_
const int LAYOUT_LEFT_PADDING = 3;
const int LAYOUT_RIGHT_PADDING = 1;

// top_bar_layout_
const int TOP_BAR_LAYOUT_LEFT_PADDING = 2;
const int TOP_BAR_LAYOUT_RIGHT_PADDING = 0;
const int TOP_BAR_LAYOUT_WIDTH_ADDER = 19;

// expander_layout_
const int EXPANDER_LAYOUT_SPACE_BETWEEN_CHILDREN = 8;

// highlight
const int HIGHLIGHT_HEIGHT = 34;
const int HIGHLIGHT_WIDTH_SUBTRACTOR = 5;

class ExpanderView : public nux::View
{
public:
  ExpanderView(NUX_FILE_LINE_DECL)
   : nux::View(NUX_FILE_LINE_PARAM)
  {
    SetAcceptKeyNavFocusOnMouseDown(false);
    SetAcceptKeyNavFocusOnMouseEnter(true);
  }

protected:
  void Draw(nux::GraphicsEngine& graphics_engine, bool force_draw)
  {};

  void DrawContent(nux::GraphicsEngine& graphics_engine, bool force_draw)
  {
    if (GetLayout())
      GetLayout()->ProcessDraw(graphics_engine, force_draw);
  }

  bool AcceptKeyNavFocus()
  {
    return true;
  }

  nux::Area* FindAreaUnderMouse(const nux::Point& mouse_position, nux::NuxEventType event_type)
  {
    bool mouse_inside = TestMousePointerInclusionFilterMouseWheel(mouse_position, event_type);

    if (mouse_inside == false)
      return nullptr;

    return this;
  }
};

}

namespace unity
{
namespace dash
{

NUX_IMPLEMENT_OBJECT_TYPE(FilterExpanderLabel);

FilterExpanderLabel::FilterExpanderLabel(std::string const& label, NUX_FILE_LINE_DECL)
  : FilterWidget(NUX_FILE_LINE_PARAM)
  , expanded(true)
  , layout_(nullptr)
  , top_bar_layout_(nullptr)
  , expander_view_(nullptr)
  , expander_layout_(nullptr)
  , right_hand_contents_(nullptr)
  , cairo_label_(nullptr)
  , raw_label_(label)
  , label_("<span size='larger' weight='bold'>" + label + "</span>")
{
  expanded.changed.connect(sigc::mem_fun(this, &FilterExpanderLabel::DoExpandChange));
  BuildLayout();
}

FilterExpanderLabel::~FilterExpanderLabel()
{
}

void FilterExpanderLabel::SetLabel(std::string const& label)
{
  raw_label_ = label;

  label_ = "<span size='larger' weight='bold'>";
  label_ += raw_label_;
  label_ += "</span>";
  cairo_label_->SetText(label_.c_str());
}

void FilterExpanderLabel::SetRightHandView(nux::View* view)
{
  view->SetMinimumHeight(RIGHT_HAND_CONTENTS_HEIGHT);
  view->SetMaximumHeight(RIGHT_HAND_CONTENTS_HEIGHT);

  right_hand_contents_ = view;
  top_bar_layout_->AddView(right_hand_contents_, 0, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
}

void FilterExpanderLabel::SetContents(nux::Layout* contents)
{
  // Since the contents is initially unowned, we don't want to Adopt it, just assign.
  contents_ = contents;

  layout_->AddLayout(contents_.GetPointer(), 1, nux::MINOR_POSITION_LEFT, nux::MINOR_SIZE_FULL);
  top_bar_layout_->SetTopAndBottomPadding(0);

  QueueDraw();
}

void FilterExpanderLabel::BuildLayout()
{
  layout_ = new nux::VLayout(NUX_TRACKER_LOCATION);
  layout_->SetLeftAndRightPadding(LAYOUT_LEFT_PADDING, LAYOUT_RIGHT_PADDING);

  top_bar_layout_ = new nux::HLayout(NUX_TRACKER_LOCATION);
  top_bar_layout_->SetLeftAndRightPadding(TOP_BAR_LAYOUT_LEFT_PADDING, TOP_BAR_LAYOUT_RIGHT_PADDING);

  expander_layout_ = new nux::HLayout(NUX_TRACKER_LOCATION);
  expander_layout_->SetSpaceBetweenChildren(EXPANDER_LAYOUT_SPACE_BETWEEN_CHILDREN);

  expander_view_ = new ExpanderView(NUX_TRACKER_LOCATION);
  expander_view_->SetLayout(expander_layout_);
  top_bar_layout_->AddView(expander_view_, 0);

  cairo_label_ = new nux::StaticText(label_.c_str(), NUX_TRACKER_LOCATION);
  cairo_label_->SetFontName("Ubuntu 10");
  cairo_label_->SetTextColor(nux::color::White);
  cairo_label_->SetAcceptKeyNavFocusOnMouseDown(false);

  nux::BaseTexture* arrow;
  arrow = dash::Style::Instance().GetGroupUnexpandIcon();
  expand_icon_ = new IconTexture(arrow,
                                 arrow->GetWidth(),
                                 arrow->GetHeight());
  expand_icon_->SetOpacity(EXPAND_DEFAULT_ICON_OPACITY);
  expand_icon_->SetMinimumSize(arrow->GetWidth(), arrow->GetHeight());
  expand_icon_->SetVisible(true);
  arrow_layout_  = new nux::VLayout();
  arrow_top_space_ = new nux::SpaceLayout(2, 2, 11, 11);
  arrow_bottom_space_ = new nux::SpaceLayout(2, 2, 9, 9);
  arrow_layout_->AddView(arrow_top_space_, 0, nux::MINOR_POSITION_CENTER);
  arrow_layout_->AddView(expand_icon_, 0, nux::MINOR_POSITION_CENTER);
  arrow_layout_->AddView(arrow_bottom_space_, 0, nux::MINOR_POSITION_CENTER);

  expander_layout_->AddView(cairo_label_, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
  expander_layout_->AddView(arrow_layout_, 0, nux::MINOR_POSITION_CENTER);
  top_bar_layout_->AddSpace(1, 1);

  top_bar_layout_->SetMaximumWidth((Style::Instance().GetTileWidth() - 12) * 2 + TOP_BAR_LAYOUT_WIDTH_ADDER);

  layout_->AddLayout(top_bar_layout_, 0, nux::MINOR_POSITION_LEFT);
  layout_->SetVerticalInternalMargin(0);

  SetLayout(layout_);

  // Lambda functions
  auto mouse_expand = [&](int x, int y, unsigned long b, unsigned long k)
  {
    expanded = !expanded;
  };

  auto key_redraw = [&](nux::Area*, bool, nux::KeyNavDirection)
  {
    QueueDraw();
  };

  auto key_expand = [&](nux::Area*)
  {
    expanded = !expanded;
  };

  // Signals
  expander_view_->mouse_click.connect(mouse_expand);
  expander_view_->key_nav_focus_change.connect(key_redraw);
  expander_view_->key_nav_focus_activate.connect(key_expand);
  cairo_label_->mouse_click.connect(mouse_expand);
  expand_icon_->mouse_click.connect(mouse_expand);
  key_nav_focus_change.connect([&](nux::Area* area, bool has_focus, nux::KeyNavDirection direction)
  {
    if(has_focus)
      nux::GetWindowCompositor().SetKeyFocusArea(expander_view_);
  });

  QueueRelayout();
  NeedRedraw();
}

void FilterExpanderLabel::DoExpandChange(bool change)
{
  dash::Style& style = dash::Style::Instance();
  if (expanded)
    expand_icon_->SetTexture(style.GetGroupUnexpandIcon());
  else
    expand_icon_->SetTexture(style.GetGroupExpandIcon());

  if (change and contents_ and !contents_->IsChildOf(layout_))
  {
    layout_->AddLayout(contents_.GetPointer(), 1, nux::MINOR_POSITION_LEFT, nux::MINOR_SIZE_FULL);
    top_bar_layout_->SetTopAndBottomPadding(0);
  }
  else if (!change and contents_ and contents_->IsChildOf(layout_))
  {
    layout_->RemoveChildObject(contents_.GetPointer());
    top_bar_layout_->SetTopAndBottomPadding(0, 10);
  }

  layout_->ComputeContentSize();
  QueueDraw();
}

bool FilterExpanderLabel::ShouldBeHighlighted()
{
  return ((expander_view_ && expander_view_->HasKeyFocus()));
}

void FilterExpanderLabel::Draw(nux::GraphicsEngine& GfxContext, bool force_draw)
{
  nux::Geometry const& base = GetGeometry();

  GfxContext.PushClippingRectangle(base);
  nux::GetPainter().PaintBackground(GfxContext, base);

  if (ShouldBeHighlighted())
  {
    nux::Geometry geo(top_bar_layout_->GetGeometry());
    geo.x = base.x;
    geo.height = HIGHLIGHT_HEIGHT;
    geo.width = base.width - HIGHLIGHT_WIDTH_SUBTRACTOR;;

    if (!highlight_layer_)
      highlight_layer_.reset(dash::Style::Instance().FocusOverlay(geo.width, geo.height));

    highlight_layer_->SetGeometry(geo);
    highlight_layer_->Renderlayer(GfxContext);
  }

  GfxContext.PopClippingRectangle();
}

void FilterExpanderLabel::DrawContent(nux::GraphicsEngine& GfxContext, bool force_draw)
{
  GfxContext.PushClippingRectangle(GetGeometry());

  if (ShouldBeHighlighted() && highlight_layer_ && !IsFullRedraw())
  {
    nux::GetPainter().PushLayer(GfxContext, highlight_layer_->GetGeometry(), highlight_layer_.get());
  }

  GetLayout()->ProcessDraw(GfxContext, force_draw);
  GfxContext.PopClippingRectangle();
}

//
// Key navigation
//
bool FilterExpanderLabel::AcceptKeyNavFocus()
{
  return true;
}

} // namespace dash
} // namespace unity
