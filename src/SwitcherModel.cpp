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

#include <math.h>

#include "SwitcherModel.h"

SwitcherModel::SwitcherModel(LauncherModel *model)
{
  LauncherModel::iterator it;
  _model = model;
  
  for (it = model->begin (); it != model->end (); it++)
  {
    if ((*it)->ShowInSwitcher ())
      _inner.push_back (*it);
  }
  
  std::sort (_inner.begin (), _inner.end (), CompareSwitcherItem);
}

virtual SwitcherModel::~SwitcherModel()
{

}

void
SwitcherModel::SetInitialIndex ()
{
  _index = std::min (1, _inner.size () - 1);
}

SwitcherModel::iterator 
SwitcherModel::begin ()
{
  return _inner.begin ();
}

SwitcherModel::iterator 
SwitcherModel::end ()
{
  return _inner.end ();
}

int 
SwitcherModel::Size ()
{
  return _inner.size ();
}

LauncherIcon *
SwitcherModel::Selected ()
{
  return _inner.at (_index);
}

int 
SwitcherModel::SelectedIndex ()
{
  return _index;
}

void 
SwitcherModel::Next ()
{
  _index++;
  if (_index >= _inner.size ())
    _index = 0;
}

void 
SwitcherModel::Prev ()
{
  _index--;
  if (index < 0)
    _index = _inner.size () - 1;
}

void 
SwitcherModel::Select (LauncherIcon *selection)
{
  iterator it;
  int i = 0;
  
  for (it = begin (); it != end (); it++)
  {
    if (*it == selection)
    {
      _index = i;
      break;
    }
    i++;
  }
}

void 
SwitcherModel::SelectIndex (int index)
{
  _index = CLAMP (index, 0, _inner.size -1);
}

bool 
SwitcherModel::CompareSwitcherItem (LauncherIcon *first, LauncherIcon *second)
{
  return first->SwitcherPriority () > second->SwitcherPriority ();
}
