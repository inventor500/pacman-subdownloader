package main

// #cgo pkg-config: libalpm
// #include <sys/utsname.h>
// #include <alpm.h>
import "C"
import (
	"flag"
	"fmt"
	"io"
	"os"
	"os/exec"
)

func main() {
	os.Exit(mainFunc())
}

func stdinAvailable() bool {
	stat, _ := os.Stdin.Stat()
	return stat.Mode()&os.ModeCharDevice == 0
}

type args struct {
	Proxy string
	Url   string
	File  string
}

func parseArgs() *args {
	var proxy string
	flag.StringVar(&proxy, "socks5hostname", "", "The socks5 hostname to use as a proxy.")
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

func buildUserAgent() string {
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
	userAgent := buildUserAgent()
	cmd := makeCommand(args, userAgent)
	available_stdin := stdinAvailable()
	var stdin io.WriteCloser
	if available_stdin {
		var err error
		if stdin, err = cmd.StdinPipe(); err != nil {
			fmt.Fprintf(os.Stderr, "Failed to get pipe: %s\n", err)
			return 1
		}
	}
	if err := cmd.Start(); err != nil {
		fmt.Fprintf(os.Stderr, "Failed to start curl: %s\n", err)
		return 1
	}
	if available_stdin {
		if _, err := io.Copy(stdin, os.Stdin); err != nil {
			fmt.Fprintf(os.Stderr, "Failed to copy stdin\n")
			return 1
		}
	}
	if err := cmd.Wait(); err != nil { // This also closes stdin
		fmt.Fprintf(os.Stderr, "Curl failed to execute correctly\n")
		return 1
	}
	return 0

}
