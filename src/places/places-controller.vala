/*
 * Copyright (C) 2009 Canonical Ltd
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
 * Authored by Neil Jagdish Patel <neil.patel@canonical.com>
 *
 */

using Gee;

namespace Unity.Places
{
  public class Controller : Object
  {
    /**
     * This class takes care of reading in the places, creating the view and
     * keeping it up-to-date
     **/
    public  Shell shell { get; construct; }
    private Model model;
    private View view;

    public Controller (Shell shell)
    {
      Object (shell:shell);
    }

    construct
    {
      this.model = new Model ();
      this.view = new View (this.model, this.shell);

      Idle.add (this.load_places);
    }

    private bool load_places ()
    {
      /* Currently we have a static list, in the future we'll be using the
       * utility functions in libunity-places to load in the actual places
       * information from the drive
       */
      var homeplace = new HomePlace ();
      homeplace.activated.connect (this.on_place_activated);
      this.model.add (homeplace);

      var place = new FakePlace ("Applications",
                                 PKGDATADIR + "/applications.png");
      place.activated.connect (this.on_place_activated);
      this.model.add (place);

      place = new FakePlace ("Files & Folders", PKGDATADIR + "/files.png");
      place.activated.connect (this.on_place_activated);
      this.model.add (place);

      this.load_remote_places ();

      return false;
    }

    public void load_remote_places ()
    {
      string placesdir = PKGDATADIR + "/places";

      var dir = GLib.File.new_for_path (placesdir);
      try
        {
          var enumerator = dir.enumerate_children (FILE_ATTRIBUTE_STANDARD_NAME,
                                                   0,
                                                   null);
          FileInfo file_info;
          while ((file_info = enumerator.next_file (null)) != null)
            {
              var filename = placesdir + "/" + file_info.get_name ();
              this.load_place (filename);
            }
        }
      catch (Error error)
        {
          message (@"Unable to read places from $placesdir: %s", error.message);
        }
    }

    private void load_place (string filename)
    {
      string group = "Place";

      var file = new KeyFile ();

      try
        {
          file.load_from_file (filename,
                               KeyFileFlags.KEEP_COMMENTS
                                | KeyFileFlags.KEEP_TRANSLATIONS);

          var name = file.get_string (group, "Name");
          var comment = file.get_string (group, "Comment");
          var icon_name = file.get_string (group, "Icon");
          var path = file.get_string (group, "Path");

          Place place = new PlaceProxy (name, icon_name, comment, path);

          if (place is Place)
            {
              place.activated.connect (this.on_place_activated);
              this.model.add (place);
            }
        }
      catch (Error e)
        {
          warning (@"Unable to load place '$filename': %s", e.message);
        }
    }
    private void on_place_activated (Place place)
    {
      this.view.set_content_view (place.get_view ());
    }

    /* Public Methods */
    public View get_view ()
    {
      return this.view;
    }
  }

  private class HomePlace : Place
  {
    public HomePlace ()
    {
      Object (name:"Home",
              icon_name:PKGDATADIR + "/home.png",
              comment:"");
    }

    public override Clutter.Actor get_view ()
    {
      return new Default.View ();
    }
  }

  private class FakePlace : Place
  {
    public FakePlace (string name, string icon_name)
    {
      Object (name:name, icon_name:icon_name, comment:"");
    }

    public override Clutter.Actor get_view ()
    {
      if (this.name == "Applications")
        return new Application.ApplicationView ();

      return new File.FileView ();
    }
  }
}

