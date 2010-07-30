/* gtkhotkey-1.0.vapi generated by vapigen, do not modify. */

[CCode (cprefix = "GtkHotkey", lower_case_cprefix = "gtk_hotkey_")]
namespace GtkHotkey {
	[CCode (cheader_filename = "gtkhotkey.h")]
	public class Info : GLib.Object {
		[CCode (has_construct_function = false)]
		public Info (string app_id, string key_id, string signature, GLib.AppInfo? app_info);
		public bool bind () throws GLib.Error;
		[CCode (cname = "gtk_hotkey_info_activated")]
		public void do_activated (uint event_time);
		public bool equals (GtkHotkey.Info hotkey2, bool sloppy_equals);
		public unowned GLib.AppInfo get_app_info ();
		public unowned string get_application_id ();
		public unowned string get_description ();
		public unowned string get_key_id ();
		public unowned string get_signature ();
		public bool is_bound ();
		public void set_description (string description);
		public bool unbind () throws GLib.Error;
		public GLib.AppInfo app_info { get; construct; }
		public string application_id { get; construct; }
		[NoAccessorMethod]
		public bool bound { get; }
		public string description { get; set; }
		public string key_id { get; construct; }
		public string signature { get; construct; }
		public virtual signal void activated (uint p0);
	}
	[CCode (cheader_filename = "gtkhotkey.h")]
	public class KeyFileRegistry : GtkHotkey.Registry {
	}
	[CCode (cheader_filename = "gtkhotkey.h")]
	public class Listener : GLib.Object {
		public virtual bool bind_hotkey (GtkHotkey.Info hotkey) throws GLib.Error;
		[CCode (cname = "gtk_hotkey_listener_activated")]
		public void do_activated (GtkHotkey.Info hotkey, uint event_time);
		public static GLib.Quark error_quark ();
		public static unowned GtkHotkey.Listener get_default ();
		public virtual bool unbind_hotkey (GtkHotkey.Info hotkey) throws GLib.Error;
		public virtual signal void activated (GtkHotkey.Info p0, uint p1);
	}
	[CCode (cheader_filename = "gtkhotkey.h")]
	public class Registry : GLib.Object {
		public virtual bool delete_hotkey (string app_id, string key_id) throws GLib.Error;
		[CCode (cname = "gtk_hotkey_registry_hotkey_deleted")]
		public void do_hotkey_deleted (GtkHotkey.Info hotkey);
		[CCode (cname = "gtk_hotkey_registry_hotkey_stored")]
		public void do_hotkey_stored (GtkHotkey.Info hotkey);
		public static GLib.Quark error_quark ();
		public virtual unowned GLib.List get_all_hotkeys ();
		public virtual unowned GLib.List get_application_hotkeys (string app_id) throws GLib.Error;
		public static unowned GtkHotkey.Registry get_default ();
		public virtual unowned GtkHotkey.Info get_hotkey (string app_id, string key_id) throws GLib.Error;
		public virtual bool has_hotkey (string app_id, string key_id);
		public virtual bool store_hotkey (GtkHotkey.Info info) throws GLib.Error;
		public virtual signal void hotkey_deleted (GLib.Object info);
		public virtual signal void hotkey_stored (GLib.Object info);
	}
	[CCode (cheader_filename = "gtkhotkey.h")]
	public class X11Listener : GtkHotkey.Listener {
	}
	[CCode (cprefix = "GTK_HOTKEY_LISTENER_ERROR_", has_type_id = false, cheader_filename = "gtkhotkey-1.0.h")]
	public enum ListenerError {
		BIND,
		UNBIND
	}
	[CCode (cprefix = "GTK_HOTKEY_REGISTRY_ERROR_", has_type_id = false, cheader_filename = "gtkhotkey-1.0.h")]
	public enum RegistryError {
		UNKNOWN_APP,
		UNKNOWN_KEY,
		MALFORMED_MEDIUM,
		IO,
		UNKNOWN,
		BAD_SIGNATURE,
		MISSING_APP
	}
}
