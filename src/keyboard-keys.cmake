# keyboard-keys cmake file

set(KEYS_TXT src/keyboard-keys.txt)
set(KEYS_GPERF src/keyboard-keys-from-name.gperf)
set(KEYS_FROM_H src/keyboard-keys-from-name.h)
set(KEYS_TO_H src/keyboard-keys-to-name.h)
execute_process(
	COMMAND gcc -E -dM -include linux/input.h - 
	COMMAND ${AWK} "/^#define[ \t]+KEY_[^ ]+[ \t]+[0-9]/ { if ($2 != \"KEY_MAX\") { print $2 } }" 
	INPUT_FILE /dev/null
	OUTPUT_FILE ${KEYS_TXT}
)
message("-- generate ${KEYS_TXT}")
execute_process(
	COMMAND ${AWK} "BEGIN{ print \"struct key { const char* name; unsigned short id; };\"; print \"%null-strings\"; print \"%%\";} { print tolower(substr($$1 ,5)) \", \" $$1 }"
	INPUT_FILE ${KEYS_TXT}
	OUTPUT_FILE ${KEYS_GPERF}
)
message("-- generate ${KEYS_GPERF}")
execute_process(
	COMMAND ${GPERF} -L ANSI-C -t -N keyboard_lookup_key -H hash_key_name -p -C
	INPUT_FILE ${KEYS_GPERF}
	OUTPUT_FILE ${KEYS_FROM_H}
)
message("-- generate ${KEYS_FROM_H}")
execute_process(
	COMMAND ${AWK} "BEGIN{ print \"const char* const key_names[KEY_CNT] = { \"} { print \"[\" $$1 \"] = \\\"\" $$1 \"\\\",\" } END{print \"};\"}"
	INPUT_FILE ${KEYS_TXT}
	OUTPUT_FILE ${KEYS_TO_H}
)
message("-- generate ${KEYS_TO_H}")
include_directories(${PROJECT_BINARY_DIR}/src)