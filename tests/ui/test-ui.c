/* test-ui.c generated by valac, the Vala compiler
 * generated from test-ui.vala, do not modify */

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
 * Authored by Neil Jagdish Patel <neil.patel@canonical.com>
 *
 */

#include <glib.h>
#include <glib-object.h>
#include <unity-private.h>
#include <gtk/gtk.h>
#include <clutk/clutk.h>
#include <stdlib.h>
#include <string.h>
#include <gobject/gvaluecollector.h>


#define TYPE_MAIN (main_get_type ())
#define MAIN(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_MAIN, Main))
#define MAIN_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_MAIN, MainClass))
#define IS_MAIN(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_MAIN))
#define IS_MAIN_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_MAIN))
#define MAIN_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_MAIN, MainClass))

typedef struct _Main Main;
typedef struct _MainClass MainClass;
typedef struct _MainPrivate MainPrivate;

#define UNITY_TESTS_UI_TYPE_QUICKLIST_SUITE (unity_tests_ui_quicklist_suite_get_type ())
#define UNITY_TESTS_UI_QUICKLIST_SUITE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), UNITY_TESTS_UI_TYPE_QUICKLIST_SUITE, UnityTestsUIQuicklistSuite))
#define UNITY_TESTS_UI_QUICKLIST_SUITE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), UNITY_TESTS_UI_TYPE_QUICKLIST_SUITE, UnityTestsUIQuicklistSuiteClass))
#define UNITY_TESTS_UI_IS_QUICKLIST_SUITE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UNITY_TESTS_UI_TYPE_QUICKLIST_SUITE))
#define UNITY_TESTS_UI_IS_QUICKLIST_SUITE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), UNITY_TESTS_UI_TYPE_QUICKLIST_SUITE))
#define UNITY_TESTS_UI_QUICKLIST_SUITE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), UNITY_TESTS_UI_TYPE_QUICKLIST_SUITE, UnityTestsUIQuicklistSuiteClass))

typedef struct _UnityTestsUIQuicklistSuite UnityTestsUIQuicklistSuite;
typedef struct _UnityTestsUIQuicklistSuiteClass UnityTestsUIQuicklistSuiteClass;
#define _unity_testing_logging_unref0(var) ((var == NULL) ? NULL : (var = (unity_testing_logging_unref (var), NULL)))
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))
typedef struct _ParamSpecMain ParamSpecMain;

struct _Main {
	GTypeInstance parent_instance;
	volatile int ref_count;
	MainPrivate * priv;
};

struct _MainClass {
	GTypeClass parent_class;
	void (*finalize) (Main *self);
};

struct _ParamSpecMain {
	GParamSpec parent_instance;
};


static gpointer main_parent_class = NULL;

gpointer main_ref (gpointer instance);
void main_unref (gpointer instance);
GParamSpec* param_spec_main (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags);
void value_set_main (GValue* value, gpointer v_object);
void value_take_main (GValue* value, gpointer v_object);
gpointer value_get_main (const GValue* value);
GType main_get_type (void);
enum  {
	MAIN_DUMMY_PROPERTY
};
GType unity_tests_ui_quicklist_suite_get_type (void);
UnityTestsUIQuicklistSuite* unity_tests_ui_quicklist_suite_new (void);
UnityTestsUIQuicklistSuite* unity_tests_ui_quicklist_suite_construct (GType object_type);
static gboolean _lambda0_ (void);
static gboolean __lambda0__gsource_func (gpointer self);
gint main_main (char** args, int args_length1);
Main* main_new (void);
Main* main_construct (GType object_type);
static void main_finalize (Main* obj);



static gboolean _lambda0_ (void) {
	gboolean result = FALSE;
	g_test_run ();
	gtk_main_quit ();
	result = FALSE;
	return result;
}


static gboolean __lambda0__gsource_func (gpointer self) {
	return _lambda0_ ();
}


gint main_main (char** args, int args_length1) {
	gint result = 0;
	UnityTestingLogging* logger;
	UnityTestsUIQuicklistSuite* quicklist_suite;
	UnityTestingLogging* _tmp0_;
	UnityTestsUIQuicklistSuite* _tmp1_;
	logger = NULL;
	quicklist_suite = NULL;
	g_setenv ("UNITY_DISABLE_TRAY", "1", TRUE);
	g_setenv ("UNITY_DISABLE_IDLES", "1", TRUE);
	gtk_init (&args_length1, &args);
	g_object_set (gtk_settings_get_default (), "gtk-xft-dpi", 96 * 1024, NULL);
	ctk_init (&args_length1, &args);
	g_test_init (&args_length1, &args, NULL);
	logger = (_tmp0_ = unity_testing_logging_new (), _unity_testing_logging_unref0 (logger), _tmp0_);
	quicklist_suite = (_tmp1_ = unity_tests_ui_quicklist_suite_new (), _g_object_unref0 (quicklist_suite), _tmp1_);
	g_timeout_add_seconds_full (G_PRIORITY_DEFAULT, (guint) 3, __lambda0__gsource_func, NULL, NULL);
	gtk_main ();
	result = 0;
	_unity_testing_logging_unref0 (logger);
	_g_object_unref0 (quicklist_suite);
	return result;
}


int main (int argc, char ** argv) {
	g_type_init ();
	return main_main (argv, argc);
}


Main* main_construct (GType object_type) {
	Main* self;
	self = (Main*) g_type_create_instance (object_type);
	return self;
}


Main* main_new (void) {
	return main_construct (TYPE_MAIN);
}


static void value_main_init (GValue* value) {
	value->data[0].v_pointer = NULL;
}


static void value_main_free_value (GValue* value) {
	if (value->data[0].v_pointer) {
		main_unref (value->data[0].v_pointer);
	}
}


static void value_main_copy_value (const GValue* src_value, GValue* dest_value) {
	if (src_value->data[0].v_pointer) {
		dest_value->data[0].v_pointer = main_ref (src_value->data[0].v_pointer);
	} else {
		dest_value->data[0].v_pointer = NULL;
	}
}


static gpointer value_main_peek_pointer (const GValue* value) {
	return value->data[0].v_pointer;
}


static gchar* value_main_collect_value (GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	if (collect_values[0].v_pointer) {
		Main* object;
		object = collect_values[0].v_pointer;
		if (object->parent_instance.g_class == NULL) {
			return g_strconcat ("invalid unclassed object pointer for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		} else if (!g_value_type_compatible (G_TYPE_FROM_INSTANCE (object), G_VALUE_TYPE (value))) {
			return g_strconcat ("invalid object type `", g_type_name (G_TYPE_FROM_INSTANCE (object)), "' for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		}
		value->data[0].v_pointer = main_ref (object);
	} else {
		value->data[0].v_pointer = NULL;
	}
	return NULL;
}


static gchar* value_main_lcopy_value (const GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	Main** object_p;
	object_p = collect_values[0].v_pointer;
	if (!object_p) {
		return g_strdup_printf ("value location for `%s' passed as NULL", G_VALUE_TYPE_NAME (value));
	}
	if (!value->data[0].v_pointer) {
		*object_p = NULL;
	} else if (collect_flags && G_VALUE_NOCOPY_CONTENTS) {
		*object_p = value->data[0].v_pointer;
	} else {
		*object_p = main_ref (value->data[0].v_pointer);
	}
	return NULL;
}


GParamSpec* param_spec_main (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags) {
	ParamSpecMain* spec;
	g_return_val_if_fail (g_type_is_a (object_type, TYPE_MAIN), NULL);
	spec = g_param_spec_internal (G_TYPE_PARAM_OBJECT, name, nick, blurb, flags);
	G_PARAM_SPEC (spec)->value_type = object_type;
	return G_PARAM_SPEC (spec);
}


gpointer value_get_main (const GValue* value) {
	g_return_val_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, TYPE_MAIN), NULL);
	return value->data[0].v_pointer;
}


void value_set_main (GValue* value, gpointer v_object) {
	Main* old;
	g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, TYPE_MAIN));
	old = value->data[0].v_pointer;
	if (v_object) {
		g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, TYPE_MAIN));
		g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
		value->data[0].v_pointer = v_object;
		main_ref (value->data[0].v_pointer);
	} else {
		value->data[0].v_pointer = NULL;
	}
	if (old) {
		main_unref (old);
	}
}


void value_take_main (GValue* value, gpointer v_object) {
	Main* old;
	g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, TYPE_MAIN));
	old = value->data[0].v_pointer;
	if (v_object) {
		g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, TYPE_MAIN));
		g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
		value->data[0].v_pointer = v_object;
	} else {
		value->data[0].v_pointer = NULL;
	}
	if (old) {
		main_unref (old);
	}
}


static void main_class_init (MainClass * klass) {
	main_parent_class = g_type_class_peek_parent (klass);
	MAIN_CLASS (klass)->finalize = main_finalize;
}


static void main_instance_init (Main * self) {
	self->ref_count = 1;
}


static void main_finalize (Main* obj) {
	Main * self;
	self = MAIN (obj);
}


GType main_get_type (void) {
	static volatile gsize main_type_id__volatile = 0;
	if (g_once_init_enter (&main_type_id__volatile)) {
		static const GTypeValueTable g_define_type_value_table = { value_main_init, value_main_free_value, value_main_copy_value, value_main_peek_pointer, "p", value_main_collect_value, "p", value_main_lcopy_value };
		static const GTypeInfo g_define_type_info = { sizeof (MainClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) main_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (Main), 0, (GInstanceInitFunc) main_instance_init, &g_define_type_value_table };
		static const GTypeFundamentalInfo g_define_type_fundamental_info = { (G_TYPE_FLAG_CLASSED | G_TYPE_FLAG_INSTANTIATABLE | G_TYPE_FLAG_DERIVABLE | G_TYPE_FLAG_DEEP_DERIVABLE) };
		GType main_type_id;
		main_type_id = g_type_register_fundamental (g_type_fundamental_next (), "Main", &g_define_type_info, &g_define_type_fundamental_info, 0);
		g_once_init_leave (&main_type_id__volatile, main_type_id);
	}
	return main_type_id__volatile;
}


gpointer main_ref (gpointer instance) {
	Main* self;
	self = instance;
	g_atomic_int_inc (&self->ref_count);
	return instance;
}


void main_unref (gpointer instance) {
	Main* self;
	self = instance;
	if (g_atomic_int_dec_and_test (&self->ref_count)) {
		MAIN_GET_CLASS (self)->finalize (self);
		g_type_free_instance ((GTypeInstance *) self);
	}
}




