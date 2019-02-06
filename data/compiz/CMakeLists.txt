find_package (PkgConfig REQUIRED)

pkg_check_modules (COMPIZCONFIG libcompizconfig)

if (COMPIZCONFIG_FOUND)
    pkg_get_variable (COMPIZCONFIG_CONFIG_DIR libcompizconfig configdir)
    pkg_get_variable (COMPIZCONFIG_UPGRADES_DIR libcompizconfig upgradesdir)

    set (config_files
         unity.conf
         unity.ini
         unity-lowgfx.ini)

    set (upgrade_files
         upgrades/com.canonical.unity.unity.01.upgrade
         upgrades/com.canonical.unity.unity.02.upgrade
         upgrades/com.canonical.unity.unity.03.upgrade
         upgrades/com.canonical.unity.unity.04.upgrade
         upgrades/com.canonical.unity.unity.05.upgrade
         upgrades/com.canonical.unity.unity.06.upgrade
         upgrades/com.canonical.unity.unity.07.upgrade
         upgrades/com.canonical.unity.unity-lowgfx.01.upgrade)

    install (FILES ${config_files} DESTINATION ${COMPIZCONFIG_CONFIG_DIR})
    install (FILES ${upgrade_files} DESTINATION ${COMPIZCONFIG_UPGRADES_DIR})
endif ()
