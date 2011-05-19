TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = Shared \
	JupiterLib \
	Editor\Editor.pro \
	Tests

Editor.depends = Shared
Editor.depends = JupiterLib
