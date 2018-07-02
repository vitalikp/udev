# hwdb data cmake file


install(FILES 20-pci-vendor-model.hwdb DESTINATION ${udevhwdbdir})
install(FILES 20-pci-classes.hwdb DESTINATION ${udevhwdbdir})
install(FILES 20-usb-vendor-model.hwdb DESTINATION ${udevhwdbdir})
install(FILES 20-usb-classes.hwdb DESTINATION ${udevhwdbdir})
install(FILES 20-sdio-vendor-model.hwdb DESTINATION ${udevhwdbdir})
install(FILES 20-sdio-classes.hwdb DESTINATION ${udevhwdbdir})
install(FILES 20-bluetooth-vendor-product.hwdb DESTINATION ${udevhwdbdir})
install(FILES 20-acpi-vendor.hwdb DESTINATION ${udevhwdbdir})
install(FILES 20-OUI.hwdb DESTINATION ${udevhwdbdir})
install(FILES 20-net-ifname.hwdb DESTINATION ${udevhwdbdir})
install(FILES 60-keyboard.hwdb DESTINATION ${udevhwdbdir})