// -*- Mode: C++; indent-tabs-mode: nil; tab-width: 2 -*-
/*
 * Copyright (C) 2013 Canonical Ltd
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

#ifndef UNITY_USER_AUTHENTICATOR_PAM_H
#define UNITY_USER_AUTHENTICATOR_PAM_H

#include <UnityCore/GLibSource.h>

#include "UserAuthenticator.h"

// Forward declarations
struct pam_handle;
struct pam_message;
struct pam_response;

namespace unity
{
namespace lockscreen
{

class UserAuthenticatorPam : public UserAuthenticator
{
public:
  UserAuthenticatorPam() = default;
  bool AuthenticateStart(std::string const& username, AuthenticateEndCallback const&) override;
  void AuthenticateCancel() override;

private:
  UserAuthenticatorPam(UserAuthenticatorPam const&) = delete;
  UserAuthenticatorPam& operator=(UserAuthenticatorPam const&) = delete;

  static gpointer AuthenticationThreadFunc(gpointer);
  bool InitPam();

  static int ConversationFunction(int num_msg,
                                  const pam_message** msg,
                                  pam_response** resp,
                                  void* appdata_ptr);

  std::string username_;
  AuthenticateEndCallback authenticate_cb_;

  int status_ = 0;
  bool first_prompt_ = true;
  bool cancelled_ = false;
  pam_handle* pam_handle_ = nullptr;
  glib::SourceManager source_manager_;
};

} // lockscreen
} // unity

#endif // UNITY_USER_AUTHENTICATOR_PAM_H
