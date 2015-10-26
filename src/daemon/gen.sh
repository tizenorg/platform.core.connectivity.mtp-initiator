gdbus-codegen --interface-prefix org.tizen.mtp. \
	--generate-c-code mtp_gdbuslib \
	--c-namespace mtp_gdbuslib \
	--c-generate-object-manager \
	--generate-docbook generated-docs \
	mtp_gdbuslib.xml