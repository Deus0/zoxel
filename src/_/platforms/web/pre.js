// web saves (bsh/web.sh --pre-js): /saves is kept in the browser's IndexedDB (emscripten's IDBFS), so saves outlive
//  a reload; get_save_directory puts them there on web (src/_/pathing/fun/save_dir.c)
//   - loaded before main() (a run dependency), so the main menu sees them (its continue button)
//   - written back every 2 seconds and when the page is hidden; syncfs only writes what changed
Module.preRun = Module.preRun || [];
Module.preRun.push(function () {
    FS.mkdir('/saves');
    FS.mount(IDBFS, {}, '/saves');
    addRunDependency('zox_saves');
    FS.syncfs(true, function (error) {
        if (error) {
            console.error('[saves] loading failed', error);
        }
        removeRunDependency('zox_saves');
    });
    let syncing = false;
    const write_saves = function () {
        if (syncing) {
            return;
        }
        syncing = true;
        FS.syncfs(false, function (error) {
            syncing = false;
            if (error) {
                console.error('[saves] writing failed', error);
            }
        });
    };
    setInterval(write_saves, 2000);
    document.addEventListener('visibilitychange', function () {
        if (document.visibilityState === 'hidden') {
            write_saves();
        }
    });
});
