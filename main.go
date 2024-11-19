package main

// #cgo pkg-config: libalpm
// #include <sys/utsname.h>
// #include <alpm.h>
import "C"
import (
	"flag"
	"fmt"
	"os"
	"os/exec"
)

func main() {
	os.Exit(mainFunc())
}

type args struct {
	Proxy string
	Url   string
	File  string
}

func parseArgs() *args {
	var proxy string
	flag.StringVar(&proxy, "socks5-hostname", "", "The socks5 hostname to use as a proxy.")
	flag.Parse()
	if len(flag.Args()) != 2 {
		flag.Usage()
		os.Exit(1)
	}
	return &args{
		proxy,
		flag.Args()[0],
		flag.Args()[1],
	}
}

func makeCommand(args *args, userAgent string) *exec.Cmd {
	if args.Proxy != "" {
		return exec.Command("/usr/bin/curl", "--user-agent", userAgent, "--progress-bar", "--socks5-hostname", args.Proxy, "--location", "--continue-at", "-", "--fail", "--output", args.File, args.Url)
	} else {
		return exec.Command("/usr/bin/curl", "--user-agent", userAgent, "--progress-bar", "--location", "--continue-at", "-", "--fail", "--output", args.File, args.Url)
	}
}

func makeUserAgent() string {
	un := C.struct_utsname{}
	C.uname(&un)
	// Pacman's PACKAGE_VERSION is hard-coded using a macro
	// TODO: Query libalpm for the pacman package version?
	userAgent := fmt.Sprintf("pacman/%s (%s %s) libalpm/%s", "7.0.0", un.sysname, un.machine, C.alpm_version())
	return userAgent
}

func mainFunc() int {
	args := parseArgs()
	fmt.Printf("Downloading %s...\n", args.Url)
	userAgent := makeUserAgent()
	cmd := makeCommand(args, userAgent)
	cmd.Stdin = os.Stdin
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	if err := cmd.Start(); err != nil {
		fmt.Fprintf(os.Stderr, "Failed to start curl: %s\n", err)
		return 1
	}
	if err := cmd.Wait(); err != nil { // This also closes stdin
		fmt.Fprintf(os.Stderr, "Curl failed to execute correctly\n")
		return 1
	}
	return 0

}
