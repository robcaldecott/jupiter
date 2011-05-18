TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS = \
	Shared \
	JupiterLib \
	Editor \
	Tests

Editor.depends = Shared
Editor.depends = JupiterLib
