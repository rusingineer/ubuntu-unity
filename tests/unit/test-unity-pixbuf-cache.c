/* test-unity-pixbuf-cache.c generated by valac 0.9.5, the Vala compiler
 * generated from test-unity-pixbuf-cache.vala, do not modify */

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
#include <stdlib.h>
#include <string.h>
#include <unity-private.h>
#include <unity.h>
#include <gdk-pixbuf/gdk-pixdata.h>
#include <clutk/clutk.h>
#include <gtk/gtk.h>


#define UNITY_TESTS_UNIT_TYPE_UNITY_PIXBUF_CACHE_SUITE (unity_tests_unit_unity_pixbuf_cache_suite_get_type ())
#define UNITY_TESTS_UNIT_UNITY_PIXBUF_CACHE_SUITE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), UNITY_TESTS_UNIT_TYPE_UNITY_PIXBUF_CACHE_SUITE, UnityTestsUnitUnityPixbufCacheSuite))
#define UNITY_TESTS_UNIT_UNITY_PIXBUF_CACHE_SUITE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), UNITY_TESTS_UNIT_TYPE_UNITY_PIXBUF_CACHE_SUITE, UnityTestsUnitUnityPixbufCacheSuiteClass))
#define UNITY_TESTS_UNIT_IS_UNITY_PIXBUF_CACHE_SUITE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UNITY_TESTS_UNIT_TYPE_UNITY_PIXBUF_CACHE_SUITE))
#define UNITY_TESTS_UNIT_IS_UNITY_PIXBUF_CACHE_SUITE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), UNITY_TESTS_UNIT_TYPE_UNITY_PIXBUF_CACHE_SUITE))
#define UNITY_TESTS_UNIT_UNITY_PIXBUF_CACHE_SUITE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), UNITY_TESTS_UNIT_TYPE_UNITY_PIXBUF_CACHE_SUITE, UnityTestsUnitUnityPixbufCacheSuiteClass))

typedef struct _UnityTestsUnitUnityPixbufCacheSuite UnityTestsUnitUnityPixbufCacheSuite;
typedef struct _UnityTestsUnitUnityPixbufCacheSuiteClass UnityTestsUnitUnityPixbufCacheSuiteClass;
typedef struct _UnityTestsUnitUnityPixbufCacheSuitePrivate UnityTestsUnitUnityPixbufCacheSuitePrivate;
#define _g_object_unref0(var) ((var == NULL) ? NULL : (var = (g_object_unref (var), NULL)))

struct _UnityTestsUnitUnityPixbufCacheSuite {
	GObject parent_instance;
	UnityTestsUnitUnityPixbufCacheSuitePrivate * priv;
};

struct _UnityTestsUnitUnityPixbufCacheSuiteClass {
	GObjectClass parent_class;
};


static gpointer unity_tests_unit_unity_pixbuf_cache_suite_parent_class = NULL;

GType unity_tests_unit_unity_pixbuf_cache_suite_get_type (void) G_GNUC_CONST;
enum  {
	UNITY_TESTS_UNIT_UNITY_PIXBUF_CACHE_SUITE_DUMMY_PROPERTY
};
#define UNITY_TESTS_UNIT_UNITY_PIXBUF_CACHE_SUITE_DOMAIN "/Unit/Unity/PixbufCache"
static void unity_tests_unit_unity_pixbuf_cache_suite_test_allocation (UnityTestsUnitUnityPixbufCacheSuite* self);
static void _unity_tests_unit_unity_pixbuf_cache_suite_test_allocation_gdata_test_func (gpointer self);
static void unity_tests_unit_unity_pixbuf_cache_suite_test_lookup (UnityTestsUnitUnityPixbufCacheSuite* self);
static void _unity_tests_unit_unity_pixbuf_cache_suite_test_lookup_gdata_test_func (gpointer self);
static void unity_tests_unit_unity_pixbuf_cache_suite_test_setting_async (UnityTestsUnitUnityPixbufCacheSuite* self);
static void _unity_tests_unit_unity_pixbuf_cache_suite_test_setting_async_gdata_test_func (gpointer self);
UnityTestsUnitUnityPixbufCacheSuite* unity_tests_unit_unity_pixbuf_cache_suite_new (void);
UnityTestsUnitUnityPixbufCacheSuite* unity_tests_unit_unity_pixbuf_cache_suite_construct (GType object_type);



static void _unity_tests_unit_unity_pixbuf_cache_suite_test_allocation_gdata_test_func (gpointer self) {
	unity_tests_unit_unity_pixbuf_cache_suite_test_allocation (self);
}


static void _unity_tests_unit_unity_pixbuf_cache_suite_test_lookup_gdata_test_func (gpointer self) {
	unity_tests_unit_unity_pixbuf_cache_suite_test_lookup (self);
}


static void _unity_tests_unit_unity_pixbuf_cache_suite_test_setting_async_gdata_test_func (gpointer self) {
	unity_tests_unit_unity_pixbuf_cache_suite_test_setting_async (self);
}


UnityTestsUnitUnityPixbufCacheSuite* unity_tests_unit_unity_pixbuf_cache_suite_construct (GType object_type) {
	UnityTestsUnitUnityPixbufCacheSuite * self;
	self = (UnityTestsUnitUnityPixbufCacheSuite*) g_object_new (object_type, NULL);
	unity_testing_logging_init_fatal_handler ();
	g_test_add_data_func (UNITY_TESTS_UNIT_UNITY_PIXBUF_CACHE_SUITE_DOMAIN "/Allocation", self, _unity_tests_unit_unity_pixbuf_cache_suite_test_allocation_gdata_test_func);
	g_test_add_data_func (UNITY_TESTS_UNIT_UNITY_PIXBUF_CACHE_SUITE_DOMAIN "/Lookup", self, _unity_tests_unit_unity_pixbuf_cache_suite_test_lookup_gdata_test_func);
	g_test_add_data_func (UNITY_TESTS_UNIT_UNITY_PIXBUF_CACHE_SUITE_DOMAIN "/Setting", self, _unity_tests_unit_unity_pixbuf_cache_suite_test_setting_async_gdata_test_func);
	return self;
}


UnityTestsUnitUnityPixbufCacheSuite* unity_tests_unit_unity_pixbuf_cache_suite_new (void) {
	return unity_tests_unit_unity_pixbuf_cache_suite_construct (UNITY_TESTS_UNIT_TYPE_UNITY_PIXBUF_CACHE_SUITE);
}


static void unity_tests_unit_unity_pixbuf_cache_suite_test_allocation (UnityTestsUnitUnityPixbufCacheSuite* self) {
	UnityPixbufCache* cache;
	UnityPixbufCache* _tmp0_;
	g_return_if_fail (self != NULL);
	cache = unity_pixbuf_cache_get_default ();
	g_assert (UNITY_IS_PIXBUF_CACHE (cache));
	g_assert (cache == (_tmp0_ = unity_pixbuf_cache_get_default ()));
	_g_object_unref0 (_tmp0_);
	_g_object_unref0 (cache);
}


static void unity_tests_unit_unity_pixbuf_cache_suite_test_lookup (UnityTestsUnitUnityPixbufCacheSuite* self) {
	GdkPixbuf* pixbuf;
	UnityPixbufCache* cache;
	GdkPixbuf* _tmp0_;
	GdkPixbuf* _tmp1_;
	GdkPixbuf* _tmp2_;
	GdkPixbuf* _tmp3_;
	GdkPixbuf* _tmp4_;
	g_return_if_fail (self != NULL);
	pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, 69, 69);
	cache = unity_pixbuf_cache_get_default ();
	g_assert (UNITY_IS_PIXBUF_CACHE (cache));
	g_assert (unity_pixbuf_cache_get_size (cache) == 0);
	unity_pixbuf_cache_set (cache, "foo", pixbuf, 48);
	g_assert (unity_pixbuf_cache_get_size (cache) == 1);
	g_assert ((_tmp0_ = unity_pixbuf_cache_get (cache, "foo", 48)) == pixbuf);
	_g_object_unref0 (_tmp0_);
	g_assert ((_tmp1_ = unity_pixbuf_cache_get (cache, "foo", 24)) == NULL);
	_g_object_unref0 (_tmp1_);
	g_assert ((_tmp2_ = unity_pixbuf_cache_get (cache, "bar", 48)) == NULL);
	_g_object_unref0 (_tmp2_);
	unity_pixbuf_cache_clear (cache);
	g_assert (unity_pixbuf_cache_get_size (cache) == 0);
	g_assert ((_tmp3_ = unity_pixbuf_cache_get (cache, "foo", 48)) == NULL);
	_g_object_unref0 (_tmp3_);
	g_assert ((_tmp4_ = unity_pixbuf_cache_get (cache, "bar", 48)) == NULL);
	_g_object_unref0 (_tmp4_);
	_g_object_unref0 (cache);
	_g_object_unref0 (pixbuf);
}


static void unity_tests_unit_unity_pixbuf_cache_suite_test_setting_async (UnityTestsUnitUnityPixbufCacheSuite* self) {
	CtkImage* image;
	GdkPixbuf* pixbuf;
	UnityPixbufCache* cache;
	GdkPixbuf* _tmp0_;
	g_return_if_fail (self != NULL);
	image = g_object_ref_sink ((CtkImage*) ctk_image_new ((guint) 48));
	pixbuf = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, 69, 69);
	cache = unity_pixbuf_cache_get_default ();
	g_assert (UNITY_IS_PIXBUF_CACHE (cache));
	g_assert (unity_pixbuf_cache_get_size (cache) == 0);
	unity_pixbuf_cache_set (cache, "foo", pixbuf, 48);
	g_assert (unity_pixbuf_cache_get_size (cache) == 1);
	g_assert ((_tmp0_ = unity_pixbuf_cache_get (cache, "foo", 48)) == pixbuf);
	_g_object_unref0 (_tmp0_);
	g_assert (ctk_image_get_pixbuf (image) != pixbuf);
	unity_pixbuf_cache_set_image_from_icon_name (cache, image, "foo", 48, NULL, NULL);
	while (TRUE) {
		if (!gtk_events_pending ()) {
			break;
		}
		gtk_main_iteration ();
	}
	g_assert (ctk_image_get_pixbuf (image) == pixbuf);
	if (g_test_trap_fork ((guint64) 0, G_TEST_TRAP_SILENCE_STDOUT | G_TEST_TRAP_SILENCE_STDERR)) {
		unity_pixbuf_cache_set_image_from_icon_name (cache, image, "bar", 24, NULL, NULL);
		while (TRUE) {
			if (!gtk_events_pending ()) {
				break;
			}
			gtk_main_iteration ();
		}
		exit (0);
	}
	g_test_trap_has_passed ();
	g_test_trap_assert_stderr ("*Unable to load icon_name*");
	g_assert (ctk_image_get_pixbuf (image) == pixbuf);
	_g_object_unref0 (cache);
	_g_object_unref0 (pixbuf);
	_g_object_unref0 (image);
}


static void unity_tests_unit_unity_pixbuf_cache_suite_class_init (UnityTestsUnitUnityPixbufCacheSuiteClass * klass) {
	unity_tests_unit_unity_pixbuf_cache_suite_parent_class = g_type_class_peek_parent (klass);
}


static void unity_tests_unit_unity_pixbuf_cache_suite_instance_init (UnityTestsUnitUnityPixbufCacheSuite * self) {
}


GType unity_tests_unit_unity_pixbuf_cache_suite_get_type (void) {
	static volatile gsize unity_tests_unit_unity_pixbuf_cache_suite_type_id__volatile = 0;
	if (g_once_init_enter (&unity_tests_unit_unity_pixbuf_cache_suite_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (UnityTestsUnitUnityPixbufCacheSuiteClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) unity_tests_unit_unity_pixbuf_cache_suite_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (UnityTestsUnitUnityPixbufCacheSuite), 0, (GInstanceInitFunc) unity_tests_unit_unity_pixbuf_cache_suite_instance_init, NULL };
		GType unity_tests_unit_unity_pixbuf_cache_suite_type_id;
		unity_tests_unit_unity_pixbuf_cache_suite_type_id = g_type_register_static (G_TYPE_OBJECT, "UnityTestsUnitUnityPixbufCacheSuite", &g_define_type_info, 0);
		g_once_init_leave (&unity_tests_unit_unity_pixbuf_cache_suite_type_id__volatile, unity_tests_unit_unity_pixbuf_cache_suite_type_id);
	}
	return unity_tests_unit_unity_pixbuf_cache_suite_type_id__volatile;
}




