# udev man cmake file

# xsltproc flags
set(XSLTPROC_OPT --nonet --xinclude)
set(XSLTPROC_OPT ${XSLTPROC_OPT} --stringparam man.output.quietly 1)
set(XSLTPROC_OPT ${XSLTPROC_OPT} --stringparam funcsynopsis.style ansi)
set(XSLTPROC_OPT ${XSLTPROC_OPT} --stringparam udev.version ${VERSION})
set(XSLTPROC_OPT ${XSLTPROC_OPT} --path '${PROJECT_BINARY_DIR}/man:${CMAKE_SOURCE_DIR}/man')

add_custom_target(man)


macro(add_man)
	add_custom_command(
		TARGET man
		COMMAND ${XSLTPROC}
		ARGS -o ${ARGV0}.${ARGV1} ${XSLTPROC_OPT} ${CMAKE_SOURCE_DIR}/man/custom-man.xsl ${CMAKE_SOURCE_DIR}/man/${ARGV0}.xml
		COMMENT "  XSLT\t${ARGV0}.${ARGV1}" VERBATIM)
	install(FILES ${PROJECT_BINARY_DIR}/${ARGV0}.${ARGV1} DESTINATION share/man/man${ARGV1})
endmacro(add_man)
