// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright 2012 Canonical Ltd.
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
 * Authored by: Manuel de la Pena <manuel.delapena@canonical.com>
 *
 */

#ifndef MUSIC_PAYMENT_PREVIEW_H
#define MUSIC_PAYMENT_PREVIEW_H

// key used to find the correct info hint
#define DATA_INFOHINT_ID "album_purchase_preview"

// keys of the data preview
#define DATA_PASSWORD_KEY "password"

// ations ids
#define CHANGE_PAYMENT_ACTION "change_payment_method"
#define FORGOT_PASSWORD_ACTION "forgot_password"
#define CANCEL_PURCHASE_ACTION "cancel_purchase"
#define PURCHASE_ALBUM_ACTION "purchase_album"

#include <Nux/Nux.h>
#include <Nux/AbstractButton.h>
#include <UnityCore/Lens.h>
#include <UnityCore/PaymentPreview.h>
#include "ActionButton.h"
#include "ActionLink.h"
#include "PaymentPreview.h"
#include "unity-shared/IconTexture.h"
#include "unity-shared/TextInput.h"

namespace nux
{
class AbstractPaintLayer;
class StaticCairoText;
class VLayout;
}

namespace unity
{
namespace dash
{
namespace previews
{
class CoverArt;
class PreviewInfoHintWidget;

class MusicPaymentPreview : public PaymentPreview
{
public:
  typedef nux::ObjectPtr<MusicPaymentPreview> Ptr;
  NUX_DECLARE_OBJECT_TYPE(MusicPaymentPreview, Preview);

  MusicPaymentPreview(dash::Preview::Ptr preview_model);
  ~MusicPaymentPreview();

  // From debug::Introspectable
  std::string GetName() const;

private:
  void LoadActions();

protected:
  nux::Layout* GetTitle();
  nux::Layout* GetPrice();
  nux::Layout* GetBody();
  nux::Layout* GetFormLabels();
  nux::Layout* GetFormFields();
  nux::Layout* GetFormActions();
  nux::Layout* GetFooter();

  const char* GetErrorMessage(GVariant *dict);

  void OnActionActivated(ActionButton* button, std::string const& id);
  void OnActionLinkActivated(ActionLink* link, std::string const& id);

  virtual void SetupViews();

  void PreLayoutManagement();

protected:
  // content elements
  nux::ObjectPtr<CoverArt> image_;
  nux::ObjectPtr<StaticCairoText> intro_;
  nux::ObjectPtr<StaticCairoText> title_;
  nux::ObjectPtr<StaticCairoText> subtitle_;
  nux::ObjectPtr<StaticCairoText> email_label_;
  nux::ObjectPtr<StaticCairoText> email_;
  nux::ObjectPtr<StaticCairoText> payment_label_;
  nux::ObjectPtr<StaticCairoText> payment_;
  nux::ObjectPtr<StaticCairoText> password_label_;
  nux::ObjectPtr<TextInput> password_entry_;
  nux::ObjectPtr<StaticCairoText> purchase_hint_;
  nux::ObjectPtr<StaticCairoText> purchase_prize_;
  nux::ObjectPtr<StaticCairoText> purchase_type_;
  nux::ObjectPtr<StaticCairoText> change_payment_;
  nux::ObjectPtr<StaticCairoText> forgotten_password_;
  nux::ObjectPtr<StaticCairoText> error_label_;
  nux::ObjectPtr<nux::HLayout> form_layout_;

  dash::PaymentPreview* payment_preview_model_;
  // do we want to type?
  const char* error_message_;

  // actions
  std::map<std::string, nux::ObjectPtr<nux::AbstractButton>> sorted_buttons_;

  // lock texture
  nux::ObjectPtr<IconTexture> lock_texture_;

  typedef std::unique_ptr<nux::AbstractPaintLayer> LayerPtr;
  LayerPtr details_bg_layer_;
};

}
}
}

#endif // MUSIC_PAYMENT_PREVIEW_H
