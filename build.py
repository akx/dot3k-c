import glob, os, subprocess, sys, itertools

def call(cmd):
    print("++ %s" % cmd)
    subprocess.check_call(cmd, shell=True)


def should_build(target, sources):
    try:
        target_stat = os.stat(target)
        for source in sources:
            source_stat = os.stat(source)
            if source_stat.st_mtime > target_stat.st_mtime:
                return True
    except OSError:
        return True


def ensure_file_dir(path):
    dirname = os.path.dirname(path)
    if not os.path.isdir(dirname):
        os.makedirs(dirname)


def build_lib(flavor, sources):
    obj_dir = "obj/%s" % flavor
    obj_paths = []
    for src_path in sources:
        obj_name = os.path.basename(os.path.splitext(src_path)[0]) + ".o"
        obj_path = os.path.join(obj_dir, obj_name)
        ensure_file_dir(obj_path)
        if should_build(obj_path, [src_path]):
            cflags = "-Wall -Wextra -Wno-unused-parameter -O2 -g -Isrc/lib.hat -Iinclude -std=c99 -c"
            if flavor == "shared":
                cflags += " -fPIC"
            call("gcc %s -o %s %s" % (cflags, obj_path, src_path))
        obj_paths.append(obj_path)
            
    
    if flavor == "shared":
        if should_build("libdot3k.so", obj_paths):
            call("gcc -shared -o libdot3k.so %s" % " ".join(obj_paths))
    else:
        if should_build("libdot3k.a", obj_paths):
            call("ar rcs libdot3k.a %s" % " ".join(obj_paths))    


def build_test(src_file):
    test_name = os.path.splitext(os.path.basename(src_file))[0]
    bin_path = "bin/%s" % test_name
    ensure_file_dir(bin_path)
    if test_name == "midi-test":
        libs = "-lportmidi -lm"
    else:
        libs = "-lm"
    if should_build(bin_path, [src_file]):
        call("gcc -Wall -Wextra -O2 -g -Iinclude -std=c99 -o %s %s libdot3k.a %s" % (bin_path, src_file, libs))


def build_tests():
    for test_src in test_sources:
        build_test(test_src)

def clean():
    targets = itertools.chain(
        glob.glob("obj/shared/*.o"),
        glob.glob("obj/static/*.o"),
        glob.glob("./libdot3k.*"),
        glob.glob("bin/*-test"),
    )
    to_clean = set()
    for target in targets:
        if os.path.isfile(target):
            to_clean.add(target)
    if to_clean:
        call("rm %s" % " ".join(to_clean))

lib_sources = glob.glob("src/lib.hat/*.c")
test_sources = glob.glob("src/test/*.c")

TARGETS = {
    "shared": lambda: build_lib("shared", lib_sources),
    "static": lambda: build_lib("static", lib_sources),
    "tests": build_tests,
    "clean": clean
}

targets = sys.argv[1:] or ["shared", "static", "tests"]

for target in targets:
    print("~~~", target)
    TARGETS[target]()
