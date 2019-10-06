#!/bin/bash
COLOR_REST='\e[0m';
COLOR_BLUE='\e[1;34m';

DIFF_EXE=""
CLANG_FORMATTER=""
AUTO_FIX="false"

if [ "${1}" == '-i' ]; then
	AUTO_FIX="true"
fi

if command -v colordiff > /dev/null; then
	DIFF_EXE=$(command -v colordiff)
elif command -v diff > /dev/null; then
	DIFF_EXE=$(command -v diff)
fi

if command -v clang-format > /dev/null; then
	CLANG_FORMATTER=$(command -v clang-format)
else
	echo -e "Missing clang-format, please install it with: \n    ${COLOR_BLUE}sudo apt-get install clang-format${COLOR_REST}"
	exit 1
fi

file_list=$(find include src \( -name '*.h' -or -name '*.cpp' \))
return_code=0
for f in ${file_list}; do
    checker_cmd="${DIFF_EXE} -u <(cat ${f}) <(${CLANG_FORMATTER} ${f} -style=file)"
	if ! diff=$(bash -c "${checker_cmd}"); then
		echo -e ">>>>> In file ${COLOR_BLUE}${f}${COLOR_REST} <<<<<"
		echo "${diff}" | tail -n +4
		return_code=1
		if [ "${AUTO_FIX}" == "true" ]; then
			bash -c "${CLANG_FORMATTER} ${f} -i -style=file"
			echo -e ">>>>> Auto-format ${COLOR_BLUE}${f}${COLOR_REST} done. <<<<<"
		fi
	fi
done

exit ${return_code}