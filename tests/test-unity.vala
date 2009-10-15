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

public class Main
{
  public static int main (string[] args)
  {
    Gtk.init (ref args);
    Test.init (ref args);

    add_launcher_tests ();

    Idle.add (() => {
        Test.run ();
        Gtk.main_quit ();
        return false;
        }
      );

    Gtk.main ();

    return 0;
  }

  private static void add_launcher_tests ()
  {
    Test.add_func ("/Unity/Window", () => {
        var window = new Unity.Window ();
        
        assert (window is Gtk.Window);
        assert (window.visible);
      }      
    );
  }
}
