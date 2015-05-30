# udev man cmake file

# xsltproc flags
set(XSLTPROC_OPT --nonet --xinclude)
set(XSLTPROC_OPT ${XSLTPROC_OPT} --stringparam man.output.quietly 1)
set(XSLTPROC_OPT ${XSLTPROC_OPT} --stringparam funcsynopsis.style ansi)
set(XSLTPROC_OPT ${XSLTPROC_OPT} --stringparam udev.version ${VERSION})
set(XSLTPROC_OPT ${XSLTPROC_OPT} --path '${PROJECT_BINARY_DIR}/man:${CMAKE_SOURCE_DIR}/man')

add_custom_target(man ALL)


macro(add_man name section)
	add_custom_command(
		TARGET man
		COMMAND ${XSLTPROC}
		ARGS -o ${name}.${section} ${XSLTPROC_OPT} ${CMAKE_SOURCE_DIR}/man/custom-man.xsl ${CMAKE_SOURCE_DIR}/man/${name}.xml
		COMMENT "  XSLT\t${name}.${section}" VERBATIM)
	install(FILES ${PROJECT_BINARY_DIR}/${name}.${section} DESTINATION share/man/man${section})
endmacro(add_man)
