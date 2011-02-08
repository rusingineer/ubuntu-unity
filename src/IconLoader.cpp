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
* Authored by: Neil Jagdish Patel <neil.patel@canonical.com>
*/

#include "IconLoader.h"

#define _TEMPLATE_ "%s:%d"

IconLoader::IconLoader ()
: _idle_id (0)
{
  _tasks = g_queue_new ();
  _theme = gtk_icon_theme_get_default ();
}

IconLoader::~IconLoader ()
{
  g_queue_free (_tasks);
}

IconLoader *
IconLoader::GetDefault ()
{
  static IconLoader *default_loader = NULL;

  if (G_UNLIKELY (!default_loader))
    default_loader = new IconLoader ();

  return default_loader;
}

void
IconLoader::LoadFromIconName (const char        *icon_name,
                              guint              size,
                              IconLoaderCallback slot)
{
  char *key;

  g_return_if_fail (icon_name);
  g_return_if_fail (size > 1);

  key = Hash (icon_name, size);

  if (CacheLookup (key, icon_name, size, slot))
  {
    g_free (key);
    return;
  }

  QueueTask (key, icon_name, size, slot, REQUEST_TYPE_ICON_NAME);

  g_free (key);
}

void
IconLoader::LoadFromGIconString (const char        *gicon_string,
                                 guint              size,
                                 IconLoaderCallback slot)
{
  char *key;

  g_return_if_fail (gicon_string);
  g_return_if_fail (size > 1);

  key = Hash (gicon_string, size);

  if (CacheLookup (key, gicon_string, size, slot))
  {
    g_free (key);
    return;
  }

  QueueTask (key, gicon_string, size, slot, REQUEST_TYPE_GICON_STRING);

  g_free (key);
}

void
IconLoader::LoadFromFilename (const char        *filename,
                              guint              size,
                              IconLoaderCallback slot)
{
  char      *key;

  g_return_if_fail (filename);
  g_return_if_fail (size > 1);

  key = Hash (filename, size);

  if (CacheLookup (key, filename, size, slot))
  {
    g_free (key);
    return;
  }

  QueueTask (key, filename, size, slot, REQUEST_TYPE_FILENAME);

  g_free (key);
}

//
// Private Methods
//

void
IconLoader::QueueTask (const char           *key,
                       const char           *data,
                       guint                 size,
                       IconLoaderCallback    slot,
                       IconLoaderRequestType type)
{
  IconLoaderTask *task;

  task = g_slice_new0 (IconLoaderTask);
  task->key = g_strdup (key);
  task->data = g_strdup (data);
  task->size = size;
  task->slot = slot;
  task->type = type;

  g_queue_push_tail (_tasks, task);

  if (_idle_id < 1)
  {
    _idle_id = g_idle_add ((GSourceFunc)Loop, this);
  }
}

char *
IconLoader::Hash (const gchar *data, guint size)
{
  return g_strdup_printf (_TEMPLATE_, data, size);
}

bool
IconLoader::CacheLookup (const char *key,
                         const char *data,
                         guint       size,
                         IconLoaderCallback slot)
{
  GdkPixbuf *pixbuf;

  pixbuf = _cache[key];
  if (GDK_IS_PIXBUF (pixbuf))
  {
    slot (data, size, pixbuf);
    return true;
  }
  return false;
}

bool
IconLoader::ProcessTask (IconLoaderTask *task)
{
  GdkPixbuf *pixbuf = NULL;
  bool       task_complete = false;

  // First thing we do is check the cache again, as previous tasks might have wanted the same
  if (CacheLookup (task->key, task->data, task->size, task->slot))
    return true;

  if (task->type == REQUEST_TYPE_ICON_NAME)
  {
    task_complete = ProcessIconNameTask (task);
  }
  else if (task->type == REQUEST_TYPE_GICON_STRING)
  {
    task_complete = ProcessGIconTask (task);
  }
  else if (task->type == REQUEST_TYPE_FILENAME)
  {
    task_complete = ProcessFilenameTask (task);
  }
  else
  {
    g_warning ("%s: Request type %d is not supported (%s %d)",
               G_STRFUNC,
               task->type,
               task->data,
               task->size);
    task->slot (task->data, task->size, pixbuf);
    task_complete = true;
  }

  return task_complete;
}

bool
IconLoader::ProcessIconNameTask (IconLoaderTask *task)
{
  GtkIconInfo *info;

  info = gtk_icon_theme_lookup_icon (_theme,
                                     task->data,
                                     task->size,
                                     (GtkIconLookupFlags)0);
  if (info)
  {
    GdkPixbuf *pixbuf;
    GError    *error = NULL;

    pixbuf = gtk_icon_info_load_icon (info, &error);
    if (GDK_IS_PIXBUF (pixbuf))
    {
      _cache[task->key] = pixbuf;
      task->slot (task->data, task->size, pixbuf);

      gtk_icon_info_free (info);
      return true;
    }
    else
    {
      g_warning ("%s: Unable to load icon %s at size %d: %s",
                  G_STRFUNC,
                  task->data,
                  task->size,
                  error->message);
      g_error_free (error);
      gtk_icon_info_free (info);
      return true;
    }
  }
  else
  {
    g_warning ("%s: Unable to load icon %s at size %d",
                G_STRFUNC,
                task->data,
                task->size);
    return true;
  }
}

bool
IconLoader::ProcessGIconTask (IconLoaderTask *task)
{
  return true;
}

bool
IconLoader::ProcessFilenameTask (IconLoaderTask *task)
{
  return true;
}

bool
IconLoader::Iteration ()
{
  bool is_empty;
  
  for (int i = 0; i < 4; i++)
  {
    IconLoaderTask *task;

    task = static_cast<IconLoaderTask *> (g_queue_pop_head (_tasks));
    if (!task)
      break;

    if (ProcessTask (task))
    {
      g_free (task->key);
      g_free (task->data);
      g_slice_free (IconLoaderTask, task);
    }
  }
  
  is_empty = g_queue_is_empty (_tasks);
  if (is_empty)
    _idle_id = 0;

  return !is_empty;
}

bool
IconLoader::Loop (IconLoader *self)
{
  return static_cast<IconLoader *> (self)->Iteration ();
}
