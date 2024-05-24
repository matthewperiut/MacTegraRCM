#!/bin/bash

# Variables
APP_NAME="../../cmake-build-release/MacTegraRCM.app"
VOL_NAME="MacTegraRCMVolume"
TMP_DIR="temp_dmg"
DMG_TMP="temp.dmg"
DMG_FINAL="../../cmake-build-release/MacTegraRCM.dmg"

# Create temporary directory
mkdir ${TMP_DIR}
cp -r "${APP_NAME}" ${TMP_DIR}
ln -s /Applications ${TMP_DIR}/Applications

# Create the disk image
hdiutil create -volname "${VOL_NAME}" -srcfolder ${TMP_DIR} -ov -format UDZO "${DMG_TMP}"

# Mount the disk image and get the device
DEVICE=$(hdiutil attach -readwrite -noverify -noautoopen "${DMG_TMP}" | egrep '^/dev/' | sed 1q | awk '{print $1}')

# Optional: Add a background image (if you have one)
# cp "${BACKGROUND_PIC}" /Volumes/"${VOL_NAME}"
# Set background image, icon size, etc.

# Sleep for a bit to allow the disk to mount
sleep 2

# Copy the application and make a shortcut to the Applications folder
cp -r "${APP_NAME}" /Volumes/"${VOL_NAME}"
ln -s /Applications /Volumes/"${VOL_NAME}"

# Unmount the disk image
hdiutil detach ${DEVICE}

# Convert the temporary disk image to a compressed final image
hdiutil convert "${DMG_TMP}" -format UDZO -o "${DMG_FINAL}"

# Clean up
rm -rf ${TMP_DIR}
rm ${DMG_TMP}

echo "Created ${DMG_FINAL} successfully."

