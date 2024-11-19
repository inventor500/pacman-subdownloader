pkgname='pacman-subdownloader'
pkgver='0.1'
pkgrel=1
pkgdesc="Download packages over a proxy while print the file name"
arch=('any')
url="https://github.com/inventor500/pacman-subdownloader"
license=('GPL3')
depends=('pacman' 'curl')
makedepends=('go')
source=('cmd/pacman-subdownloader/main.go')
sha256sums=('cbb91395d47bc84d943482f7dc47d50a26da392c523bd0e8a65eb658c13902f0')

prepare() {
	mkdir "$pkgname"
	cd "$pkgname"
	mv ../main.go .
	go mod init "${url#https://}" # strip https:// from canonical URL
	go mod tidy
}

build() {
	export CGO_CPPFLAGS="${CPPFLAGS}"
	export CGO_CFLAGS="${CFLAGS}"
	export CGO_CXXFLAGS="${CXXFLAGS}"
	export CGO_LDFLAGS="${LDFLAGS}"
	export GOFLAGS="-buildmode=pie -trimpath -ldflags=-linkmode=external -mod=readonly -modcacherw"
	cd "$pkgname"
	go build
}

package() {
	cd "$pkgname"
	install -Dm755 $pkgname "$pkgdir/usr/bin/$pkgname"
}
