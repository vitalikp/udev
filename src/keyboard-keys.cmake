# keyboard-keys cmake file

set(KEYS_TXT keyboard-keys.txt)
set(KEYS_GPERF keyboard-keys-from-name.gperf)
set(KEYS_FROM_H keyboard-keys-from-name.h)
set(KEYS_TO_H keyboard-keys-to-name.h)
add_custom_target(
	${KEYS_TXT} ALL
	COMMAND gcc -E -dM -include linux/input.h - < /dev/null | ${AWK} "'/^#define[" \t]+KEY_[^ ]+[ \t]+[0-9]/ { if ($$2 != \"KEY_MAX\") { print $$2 } "}'" > ${KEYS_TXT}
	#WORKING_DIRECTORY ...
)
add_custom_target(
	${KEYS_GPERF} ALL
	COMMAND ${AWK} 'BEGIN{ print \"struct key { const char* name\; unsigned short id\; }\;\"\; print \"%null-strings\"\; print \"%%\"\;} { print tolower(substr($$1 ,5)) \", \" $$1 }' < ${KEYS_TXT} > ${KEYS_GPERF}
	DEPENDS ${KEYS_TXT}
	#WORKING_DIRECTORY ...
)
add_custom_target(
	${KEYS_FROM_H} ALL
	COMMAND ${GPERF} -L ANSI-C -t -N keyboard_lookup_key -H hash_key_name -p -C < ${KEYS_GPERF} > ${KEYS_FROM_H}
	DEPENDS ${KEYS_GPERF}
	#WORKING_DIRECTORY ...
)
add_custom_target(
	${KEYS_TO_H} ALL
	COMMAND ${AWK} 'BEGIN{ print \"const char* const key_names[KEY_CNT] = { \"} { print \"[\" $$1 \"] = \\\"\" $$1 \"\\\",\" } END{print \"}\;\"}' < ${KEYS_TXT} > ${KEYS_TO_H}
	DEPENDS ${KEYS_TXT}
	#WORKING_DIRECTORY ...
)
include_directories(${PROJECT_BINARY_DIR}/src)