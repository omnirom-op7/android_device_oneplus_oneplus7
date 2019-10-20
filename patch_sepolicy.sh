#!/bin/bash

# Exports
SYSDIR=$OUT/system

echo "(allow hal_graphics_allocator_default hal_sensors_default (binder (transfer)))" >> $SYSDIR/etc/selinux/plat_sepolicy.cil
echo "(allow hal_sensors_default binder_device_28_0 (chr_file (ioctl read write getattr lock append map open)))" >> $SYSDIR/etc/selinux/plat_sepolicy.cil
echo "(allow hal_sensors_default hal_graphics_allocator_default (binder (call transfer)))" >> $SYSDIR/etc/selinux/plat_sepolicy.cil
echo "(allow hal_sensors_default hal_graphics_allocator_default (fd (use)))" >> $SYSDIR/etc/selinux/plat_sepolicy.cil
echo "(allow hal_sensors_default hal_graphics_allocator_hwservice_28_0 (hwservice_manager (find)))" >> $SYSDIR/etc/selinux/plat_sepolicy.cil
echo "(allow hal_sensors_default hal_graphics_mapper_hwservice_28_0 (hwservice_manager (find)))" >> $SYSDIR/etc/selinux/plat_sepolicy.cil
echo "(allow hal_sensors_default gpu_device_28_0 (chr_file (ioctl read write getattr lock append map open)))" >> $SYSDIR/etc/selinux/plat_sepolicy.cil
echo "(allow hal_sensors_default oem_file (dir (ioctl read write getattr lock add_name remove_name search open)))" >> $SYSDIR/etc/selinux/plat_sepolicy.cil
echo "(allow hal_sensors_default oem_file (file (ioctl read write getattr lock append map open)))" >> $SYSDIR/etc/selinux/plat_sepolicy.cil
echo "(allow hal_sensors_default servicemanager_28_0 (binder (call transfer)))" >> $SYSDIR/etc/selinux/plat_sepolicy.cil
echo "(allow hal_sensors_default surfaceflinger_28_0 (binder (call transfer)))" >> $SYSDIR/etc/selinux/plat_sepolicy.cil
echo "(allow hal_sensors_default surfaceflinger_28_0 (fd (use)))" >> $SYSDIR/etc/selinux/plat_sepolicy.cil
echo "(allow hal_sensors_default surfaceflinger_service_28_0 (service_manager (find)))" >> $SYSDIR/etc/selinux/plat_sepolicy.cil
echo "(allow servicemanager_28_0 hal_sensors_default (dir (search)))" >> $SYSDIR/etc/selinux/plat_sepolicy.cil
echo "(allow servicemanager_28_0 hal_sensors_default (file (read open)))" >> $SYSDIR/etc/selinux/plat_sepolicy.cil
echo "(allow servicemanager_28_0 hal_sensors_default (process (getattr)))" >> $SYSDIR/etc/selinux/plat_sepolicy.cil
echo "(allow surfaceflinger_28_0 hal_sensors_default (binder (transfer)))" >> $SYSDIR/etc/selinux/plat_sepolicy.cil


