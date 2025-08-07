${MINGW}:
	@echo " -----------------------------------------------"
	@echo "| MinGW root directory ($@) not found. Make sure"
	@echo "| the 'MINGW' variable is defined correctly in config.inc"
	@echo " -----------------------------------------------"
	@exit 1

${SRCDIR}:
	@echo " ---------------------------------------------------"
	@echo "| OpenBSD source directory ($@) not found. Make sure"
	@echo "| the 'SRCDIR' variable is defined correctly in config.inc"
	@echo " ---------------------------------------------------"
	@exit 1

${ROOTDIR}/config.inc:
	@echo " ---------------------------------------------------"
	@echo "| Cross-compiler not set up. Please run 'make opt' first."
	@echo " ---------------------------------------------------"
	@exit 1

${INSTALL}:
	@echo " ---------------------------------------------------"
	@echo "| BSD install program ($@) not found."
	@echo "| Your build is incomplete."
	@echo " ---------------------------------------------------"
	@exit 1
