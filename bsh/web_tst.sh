#!/usr/bin/env bash
set -euo pipefail
# runs tests in the web build, in a headless browser (chromium, playwright), as bsh/tst.sh does for the desktop
#   bash bsh/web_tst.sh                         the default set (pure logic, then a world)
#   bash bsh/web_tst.sh colors_hsv maths_vectors
#   bash bsh/web_tst.sh --nobuild colors_hsv    use the last build (bin/zoxel_web_tst)
# - builds bsh/web.sh --release --tests, serves it on 127.0.0.1, opens index.html?args=--tst <name> (the page's
#   args are the game's, src/_/platforms/web/pre.js) and reads the test's PASSED / FAILED from the console
# - each test's console in tst/web_<name>.log, exit 0 if all passed
# - playwright and its chromium go in web/ (gitignored): web/tst_venv, web/playwright, the first run downloads them
# - webgl in a headless browser is software (swiftshader), slow: keep web tests small

root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "${root}"
source bsh/deps_windows.sh

build="1"
timeout="${zox_web_tst_timeout:-120}"
tests=()
for arg in "$@"; do
    if [[ "${arg}" == "--nobuild" ]]; then
        build="0"
    else
        tests+=("${arg}")
    fi
done
# no player needed: maths, colours, then the menus' world (a camera and ui)
[[ ${#tests[@]} -eq 0 ]] && tests=(colors_hsv maths_vectors maths_quaternions maths_matrices maths_bounds)

web_path="bin/zoxel_web_tst"
if [[ "${build}" == "1" ]]; then
    echo "> building [${web_path}]"
    bash bsh/web.sh zoxel --release --tests > "web_tst_build.log" 2>&1 || {
        tail -30 web_tst_build.log
        echo "!! web build failed, see [web_tst_build.log]"
        exit 2
    }
    mkdir -p tst && mv web_tst_build.log tst/web_build.log
fi
[[ -f "${web_path}/index.html" ]] || { echo "!! no build in [${web_path}], run without --nobuild"; exit 2; }

# python for playwright, its own venv in web/
if is_windows_host; then
    install_windows_python
    python="${EMSDK_PYTHON}"
    venv_python="web/tst_venv/Scripts/python.exe"
else
    python="python3"
    venv_python="web/tst_venv/bin/python"
fi
export PLAYWRIGHT_BROWSERS_PATH="${root}/web/playwright"
if [[ ! -x "${venv_python}" ]]; then
    echo "> installing playwright into [web/tst_venv]"
    "${python}" -m venv web/tst_venv
    "${venv_python}" -m pip install --quiet --disable-pip-version-check playwright
fi
if ! ls "${PLAYWRIGHT_BROWSERS_PATH}" 2> /dev/null | grep -q chromium; then
    echo "> installing chromium into [web/playwright]"
    deps_flag=()
    # linux ci: chromium's system libraries too
    [[ "$(uname -s)" == "Linux" && "$(id -u)" == "0" ]] && deps_flag=(--with-deps)
    "${venv_python}" -m playwright install "${deps_flag[@]}" chromium
fi

mkdir -p tst
"${venv_python}" -I - "${root}/${web_path}" "${timeout}" "${root}/tst" "${tests[@]}" <<'PY'
import functools, http.server, sys, threading, time, urllib.parse
from playwright.sync_api import sync_playwright

folder, timeout, log_dir, names = sys.argv[1], float(sys.argv[2]), sys.argv[3], sys.argv[4:]

class Quiet(http.server.SimpleHTTPRequestHandler):
    def log_message(self, *args):
        pass

server = http.server.ThreadingHTTPServer(("127.0.0.1", 0), functools.partial(Quiet, directory=folder))
threading.Thread(target=server.serve_forever, daemon=True).start()
port = server.server_address[1]

failed = []
with sync_playwright() as playwright:
    # webgl 2 in software: swiftshader through angle
    browser = playwright.chromium.launch(args=["--use-angle=swiftshader", "--enable-unsafe-swiftshader",
        "--ignore-gpu-blocklist"])
    for name in names:
        page = browser.new_page()
        lines = []
        result = {}

        def on_console(message, lines=lines, result=result, name=name):
            text = message.text
            lines.append(text)
            if "v" in result:
                return
            if f"[{name}] PASSED" in text:
                result["v"] = "PASSED"
            elif f"[{name}] FAILED" in text or "Aborted(" in text or "[tst] unknown test" in text:
                result["v"] = "FAILED"

        def on_error(error, lines=lines, result=result):
            lines.append(f"page error: {error}")
            # the test's exit throws once it has logged its result
            if "ExitStatus" not in str(error):
                result.setdefault("v", "FAILED")

        page.on("console", on_console)
        page.on("pageerror", on_error)
        args = urllib.parse.quote(f"--tst {name} --tiny --nosaves")
        start = time.time()
        page.goto(f"http://127.0.0.1:{port}/index.html?args={args}")
        while "v" not in result and time.time() - start < timeout:
            page.wait_for_timeout(100)
        verdict = result.get("v", "TIMEOUT")
        seconds = time.time() - start
        with open(f"{log_dir}/web_{name}.log", "w", encoding="utf-8") as log:
            log.write("\n".join(lines) + "\n")
        print(f"[web tst] [{name}] {verdict} in [{seconds:.1f}s]")
        if verdict != "PASSED":
            failed.append(name)
            for line in [l for l in lines if "[tst]" in l or "ERROR" in l or "error" in l][-15:]:
                print(f"    {line}")
        page.close()
    browser.close()
server.shutdown()

print(f"[web tst] passed [{len(names) - len(failed)}/{len(names)}]" + (f", failed {failed}" if failed else ""))
sys.exit(1 if failed else 0)
PY
