/*
 * Copyright 2010 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 3 along with this program.  If not, see
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Gordon Allott <gord.allott@canonical.com>
 *
 */
#include <gtk/gtk.h>

#include "Nux/Nux.h"
#include "Nux/VLayout.h"
#include "Nux/WindowThread.h"
#include "NuxGraphics/GraphicsEngine.h"
#include <Nux/Layout.h>
#include <NuxCore/Logger.h>
#include <UnityCore/Variant.h>
#include <UnityCore/Preview.h>
#include <UnityCore/ApplicationPreview.h>
#include <UnityCore/MoviePreview.h>
#include <UnityCore/MusicPreview.h>
#include <UnityCore/SeriesPreview.h>
#include <unity-protocol.h>
#include "PreviewFactory.h"

#include "unity-shared/FontSettings.h"
#include "unity-shared/UnitySettings.h"
#include "unity-shared/PreviewStyle.h"
#include "unity-shared/DashStyle.h"

#include "Preview.h"
#include "PreviewContainer.h"


#define WIDTH 972
#define HEIGHT 452

using namespace unity;
using namespace unity::dash;

class DummyView : public nux::View
{
public:
  DummyView(nux::View* view)
  : View(NUX_TRACKER_LOCATION)
  {
    nux::ROPConfig rop;
    rop.Blend = true;
    rop.SrcBlend = GL_ONE;
    rop.DstBlend = GL_ONE_MINUS_SRC_ALPHA;
    bg_layer_.reset(new nux::ColorLayer(nux::Color(81, 26, 48), true, rop));

    nux::Layout* layout = new nux::VLayout();
    layout->SetPadding(16);
    layout->AddView(view, 1, nux::MINOR_POSITION_CENTER);
    SetLayout(layout);
  }

protected:
  virtual void Draw(nux::GraphicsEngine& gfx_engine, bool force_draw)
  {
    nux::Geometry const& base = GetGeometry();

    gfx_engine.PushClippingRectangle(base);
    nux::GetPainter().PaintBackground(gfx_engine, base);

    unsigned int alpha, src, dest = 0;
    gfx_engine.GetRenderStates().GetBlend(alpha, src, dest);
    gfx_engine.GetRenderStates().SetBlend(true, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    bg_layer_->SetGeometry(GetGeometry());
    nux::GetPainter().RenderSinglePaintLayer(gfx_engine, GetGeometry(), bg_layer_.get());

    gfx_engine.GetRenderStates().SetBlend(alpha, src, dest);

    gfx_engine.PopClippingRectangle();
  }

  virtual void DrawContent(nux::GraphicsEngine& gfx_engine, bool force_draw)
  {
    nux::Geometry const& base = GetGeometry();
    gfx_engine.PushClippingRectangle(base);

    if (!IsFullRedraw())
      nux::GetPainter().PushLayer(gfx_engine, GetGeometry(), bg_layer_.get());

    if (GetCompositionLayout())
      GetCompositionLayout()->ProcessDraw(gfx_engine, force_draw);

    if (!IsFullRedraw())
      nux::GetPainter().PopBackground();

    gfx_engine.PopClippingRectangle();
  }

   typedef std::unique_ptr<nux::AbstractPaintLayer> LayerPtr;
  LayerPtr bg_layer_;
};

class TestRunner
{
public:
  TestRunner ();
  ~TestRunner ();

  static void InitWindowThread (nux::NThread* thread, void* InitData);
  void Init ();
  void NavRight();
  void NavLeft();

  previews::PreviewContainer::Ptr container_;
  nux::Layout *layout_;
  int nav_iter;
};

TestRunner::TestRunner ()
{
  nav_iter = 0;
}

TestRunner::~TestRunner ()
{
}

void TestRunner::Init ()
{
  container_ = new previews::PreviewContainer(NUX_TRACKER_LOCATION);
  container_->navigate_right.connect(sigc::mem_fun(this, &TestRunner::NavRight));
  container_->navigate_left.connect(sigc::mem_fun(this, &TestRunner::NavLeft));

  DummyView* dummyView = new DummyView(container_.GetPointer());
  layout_ = new nux::VLayout(NUX_TRACKER_LOCATION);
  layout_->AddView(dummyView, 1, nux::MINOR_POSITION_CENTER, nux::MINOR_SIZE_FULL);
  nux::GetWindowThread()->SetLayout (layout_);


  std::stringstream app_name;
  app_name << "Title " << nav_iter;

  const char* subtitle = "The Beatles, 1986";
  const char* description = "";

 // creates a generic preview object
  glib::Object<GIcon> image(g_icon_new_for_string("./Beatles.png", NULL));
  glib::Object<GIcon> iconHint1(g_icon_new_for_string("/usr/share/unity/5/lens-nav-music.svg", NULL));
  glib::Object<GIcon> iconHint2(g_icon_new_for_string("/usr/share/unity/5/lens-nav-home.svg", NULL));

  glib::Object<UnityProtocolPreview> proto_obj(UNITY_PROTOCOL_PREVIEW(unity_protocol_music_preview_new()));

  unity_protocol_preview_set_title(proto_obj, app_name.str().c_str());
  unity_protocol_preview_set_subtitle(proto_obj, subtitle);
  unity_protocol_preview_set_description(proto_obj, description);
  unity_protocol_preview_set_thumbnail(proto_obj, image);
  unity_protocol_preview_add_action(proto_obj, "play-album", "Play Album", NULL, 0);
  unity_protocol_preview_add_info_hint(proto_obj, "run-time", "", iconHint1, g_variant_new("s", "10 Tracks, 37.50 min"));
  unity_protocol_preview_add_info_hint(proto_obj, "genre",  "Energy", iconHint2, g_variant_new("s", "60s, Rock 'n Roll, britpop"));

  glib::Variant v(dee_serializable_serialize(DEE_SERIALIZABLE(proto_obj.RawPtr())),
                  glib::StealRef());

  container_->preview(v, previews::RIGHT);

}

void TestRunner::NavRight()
{
   std::stringstream app_name;
  app_name << "Title " << ++nav_iter;

  const char* subtitle = "The Beatles, 1986";
  const char* description = "";

 // creates a generic preview object
  glib::Object<GIcon> image(g_icon_new_for_string("./Beatles.png", NULL));
  glib::Object<GIcon> iconHint1(g_icon_new_for_string("/usr/share/unity/5/lens-nav-music.svg", NULL));
  glib::Object<GIcon> iconHint2(g_icon_new_for_string("/usr/share/unity/5/lens-nav-home.svg", NULL));

  glib::Object<UnityProtocolPreview> proto_obj(UNITY_PROTOCOL_PREVIEW(unity_protocol_music_preview_new()));

  unity_protocol_preview_set_title(proto_obj, app_name.str().c_str());
  unity_protocol_preview_set_subtitle(proto_obj, subtitle);
  unity_protocol_preview_set_description(proto_obj, description);
  unity_protocol_preview_set_thumbnail(proto_obj, image);
  unity_protocol_preview_add_action(proto_obj, "play-album", "Play Album", NULL, 0);
  unity_protocol_preview_add_info_hint(proto_obj, "run-time", "", iconHint1, g_variant_new("s", "10 Tracks, 37.50 min"));
  unity_protocol_preview_add_info_hint(proto_obj, "genre",  "Energy", iconHint2, g_variant_new("s", "60s, Rock 'n Roll, britpop"));

  glib::Variant v(dee_serializable_serialize(DEE_SERIALIZABLE(proto_obj.RawPtr())),
                  glib::StealRef());

  container_->preview(v, previews::RIGHT);
}

void TestRunner::NavLeft()
{
   std::stringstream app_name;
  app_name << "Title " << --nav_iter;

  const char* subtitle = "The Beatles, 1986";
  const char* description = "";

 // creates a generic preview object
  glib::Object<GIcon> image(g_icon_new_for_string("./Beatles.png", NULL));
  glib::Object<GIcon> iconHint1(g_icon_new_for_string("/usr/share/unity/5/lens-nav-music.svg", NULL));
  glib::Object<GIcon> iconHint2(g_icon_new_for_string("/usr/share/unity/5/lens-nav-home.svg", NULL));

  glib::Object<UnityProtocolPreview> proto_obj(UNITY_PROTOCOL_PREVIEW(unity_protocol_music_preview_new()));

  unity_protocol_preview_set_title(proto_obj, app_name.str().c_str());
  unity_protocol_preview_set_subtitle(proto_obj, subtitle);
  unity_protocol_preview_set_description(proto_obj, description);
  unity_protocol_preview_set_thumbnail(proto_obj, image);
  unity_protocol_preview_add_action(proto_obj, "play-album", "Play Album", NULL, 0);
  unity_protocol_preview_add_info_hint(proto_obj, "run-time", "", iconHint1, g_variant_new("s", "10 Tracks, 37.50 min"));
  unity_protocol_preview_add_info_hint(proto_obj, "genre",  "Energy", iconHint2, g_variant_new("s", "60s, Rock 'n Roll, britpop"));

  glib::Variant v(dee_serializable_serialize(DEE_SERIALIZABLE(proto_obj.RawPtr())),
                  glib::StealRef());

  container_->preview(v, previews::LEFT);
}

void TestRunner::InitWindowThread(nux::NThread* thread, void* InitData)
{
  TestRunner *self =  (TestRunner *) InitData;
  self->Init ();
}

int main(int argc, char **argv)
{
  nux::WindowThread* wt = NULL;

  gtk_init (&argc, &argv);

  nux::NuxInitialize(0);
  nux::logging::configure_logging(::getenv("UNITY_LOG_SEVERITY"));
  nux::logging::Logger("unity").SetLogLevel(nux::logging::Warning);
  // The instances for the pseudo-singletons.
  unity::Settings settings;
  unity::dash::previews::Style panel_style;
  unity::dash::Style dash_style;
  unity::dash::PreviewFactory preview_factory;

  TestRunner *test_runner = new TestRunner ();
  wt = nux::CreateGUIThread(TEXT("Unity Preview"),
                            WIDTH, HEIGHT,
                            0,
                            &TestRunner::InitWindowThread,
                            test_runner);

  wt->Run (NULL);
  delete wt;
  return 0;
}


